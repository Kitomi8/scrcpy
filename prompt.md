# Prompt Master : Suite complète de fonctionnalités avancées pour scrcpy v5.0.1 (SDL3)

## Contexte & Règles strictes d'exécution
Tu travailles sur un fork personnalisé de `scrcpy v5.0.1` (client C sous **SDL3**, serveur Java sous `server/`).
**IMPORTANT — Préservation obligatoire :**
Ne casse aucune des fonctionnalités déjà implémentées :
- `--auto-reconnect` et correctif Windows `CreateProcessW` (`app/src/adb/adb.c`, `app/src/scrcpy.c`, `app/src/main.c`, `app/src/sys/win/process.c`)
- Réveil écran `F12` (`app/src/input_manager.c`)
- Mode Navigation & Réseaux Sociaux `F10` compatible avec les rotations d'écran (`sc_screen_convert_relative_to_frame_coords` dans `app/src/screen.c`)
- `TCP_NODELAY` (`app/src/util/net.c`) et basse latence (`SurfaceEncoder.java`)
- Affichage dynamique dans la barre de titre (`app/src/screen.c`, `app/src/events.h`) et `app/src/temp_monitor.c`

**Méthode de travail obligatoire (implémentation incrémentale) :**
1. Lis d'abord les fichiers existants concernés avant chaque modification pour respecter les signatures exactes, les conventions mémoire (`sc_*`), la syntaxe C11 et l'API **SDL3**.
2. Implémente un module à la fois dans l'ordre ci-dessous, en vérifiant la compilation et l'absence de warnings après chaque module.
3. Si tu ajoutes de nouveaux fichiers `.c`, enregistre-les dans `app/meson.build`.
4. Assure la compatibilité complète **Linux (GCC)** et **cross-compilation Windows (MinGW-w64)**.

---

## Étape 0 : Nettoyage du dépôt
- Supprimer le fichier vide `scrcpy-server-prebuilt` (0 octet) à la racine s'il est suivi par Git ou vide.
- Ajouter `scrcpy-server-prebuilt`, `scrcpy_social.ini` et `captures/` dans `.gitignore` afin de garder le dépôt propre.

---

## Module 1 : Protection thermique et télémétrie batterie active
**Fichiers :** `app/src/temp_monitor.h`, `app/src/temp_monitor.c`, `app/src/events.h`, `app/src/screen.h`, `app/src/screen.c`, `app/src/scrcpy.c`

1. **Extension de `temp_monitor.c` :**
   - Lors de la lecture périodique ADB (`dumpsys battery`), extraire non seulement la température (°C), mais aussi le **pourcentage de batterie (`level`)** et l'**état de charge (`status` / `AC powered` / `USB powered`)**.
   - Envoyer ces données au thread principal via un événement SDL thread-safe dans `events.h`.
2. **Télémétrie unifiée dans la barre de titre (`screen.c`) :**
   - Centraliser le formatage du titre dans une fonction unique `sc_screen_update_title(struct sc_screen *screen)` qui combine proprement tous les états actifs sans s'écraser mutuellement :
     `NomAppareil [60 FPS | 35°C | 82%] [SOCIAL: TIKTOK] [REC] [MUTED] [HOT]`
   - N'afficher chaque badge que lorsque l'état correspondant est actif ou disponible.
3. **Alerte de préservation batterie (seuil 80 %) :**
   - Si le téléphone est en charge et que `battery_level >= 80`, afficher `[CHARGE >= 80%]` dans la barre de titre et émettre un `LOGW` clair (une seule fois par franchissement de seuil, sans spammer les logs).
4. **Protection anti-surchauffe automatique (seuil 42 °C) :**
   - Si la température dépasse `42.0°C` (constante configurable), afficher `[HOT]` dans la barre de titre et envoyer automatiquement un message de contrôle `SC_CONTROL_MSG_TYPE_SET_DISPLAY_POWER` avec `on = false` pour éteindre l'écran physique du téléphone et limiter la chauffe.

---

## Module 2 : Profils `.ini` commutables (`F9`) & Swipes asynchrones pour le mode `F10`
**Fichiers :** `app/src/input_manager.c`, `app/src/input_manager.h`, `app/src/screen.c` (et optionnellement `app/src/social_config.h` / `app/src/social_config.c`)

1. **Configuration externe `scrcpy_social.ini` :**
   - Créer un parseur INI léger en C pur (sans dépendance externe) qui charge `scrcpy_social.ini` depuis le dossier courant (ou le dossier de l'exécutable).
   - Si `scrcpy_social.ini` n'existe pas, utiliser les coordonnées par défaut intégrées et proposer une option ou génération automatique propre au premier chargement.
   - Supporter au moins 4 profils : `[TIKTOK]`, `[IG_LITE]`, `[FB_LITE]` et `[YOUTUBE]`, avec leurs coordonnées relatives `(rx, ry)` configurables (`like_x/y`, `comment_x/y`, `share_x/y`, `profile_x/y`, `my_profile_x/y`, `dm_x/y`, `feed_x/y`).
2. **Changement de profil à la volée (`F9`) :**
   - `F9` : passe au profil suivant (`TIKTOK` -> `IG_LITE` -> `FB_LITE` -> `YOUTUBE`) et met à jour le badge `[SOCIAL: <NOM>]` dans la barre de titre.
   - `Shift + F9` : recharge `scrcpy_social.ini` à chaud depuis le disque sans redémarrer `scrcpy`.
3. **Exécution asynchrone des gestes tactiles (Zéro freeze vidéo) :**
   - Remplacer tout `SDL_Delay()` bloquant sur le thread principal SDL lors des swipes / double-taps du mode `F10` par une file ou un worker asynchrone (thread léger `sc_thread` dédié ou timers SDL3 `SDL_AddTimer`), afin que le rendu vidéo reste à 60 FPS constants pendant les gestes.
   - Conserver obligatoirement la conversion via `sc_screen_convert_relative_to_frame_coords()` pour maintenir la compatibilité totale avec les rotations (`Alt + ←/→`).

---

## Module 3 : Contrôles multimédia et bureau à la volée
**Fichiers :** `app/src/audio_player.h`, `app/src/audio_player.c`, `app/src/screen.c`, `app/src/input_manager.c`, `app/src/recorder.c`, `app/src/scrcpy.c`

1. **Mute / Unmute audio instantané (`F8`) :**
   - Ajouter un indicateur atomique (`atomic_bool muted`) dans `struct sc_audio_player`.
   - Quand `muted` est actif, injecter du silence (buffer à zéro) dans le flux SDL3 audio sans détruire le pipeline audio.
   - Lier la bascule à la touche `F8` (sans modificateur) et afficher `[MUTED]` dans la barre de titre.
2. **Capture d'écran PNG légère (`Alt + Y`) :**
   - Lors de `MOD + Y`, capturer l'image courante dans `screen.c`, l'encoder en **PNG** via `libavcodec` (`AV_CODEC_ID_PNG`, déjà lié dans `app/meson.build` via FFmpeg) et l'enregistrer dans `captures/scrcpy_YYYYMMDD_HHMMSS.png` (créer le dossier `captures/` automatiquement sous Linux et Windows).
   - En cas d'indisponibilité de l'encodeur PNG, retomber proprement en `.bmp` via `SDL_SaveBMP`.
3. **Mode Mini-lecteur & Opacité de la fenêtre :**
   - `MOD + Shift + O` : fait cycler la transparence de la fenêtre via `SDL_SetWindowOpacity()` (`1.00f` -> `0.85f` -> `0.70f` -> `0.50f`).
   - `MOD + Shift + M` : bascule un mode « Mini-lecteur » (sauvegarde la taille/position actuelle, réduit la fenêtre à une largeur compacte ~320px en conservant le ratio, et active `SDL_SetWindowAlwaysOnTop(window, true)` ; un second appui restaure l'état initial).
4. **Enregistrement vidéo à la volée (`MOD + Shift + R`) :**
   - Permettre de démarrer/stopper un enregistrement `.mp4` horodaté (`captures/scrcpy_rec_YYYYMMDD_HHMMSS.mp4`) pendant une session active sans avoir lancé `--record` au démarrage.
   - Attacher/détacher dynamiquement un `sc_recorder` en mode thread-safe sur le flux vidéo (et audio si actif), ou gérer proprement le cycle `sc_recorder_init` / `sc_recorder_start` / `sc_recorder_stop` sur la prochaine keyframe (SPS/PPS + IDR) afin que le fichier `.mp4` soit immédiatement lisible.
   - Afficher `[REC]` dans la barre de titre pendant l'enregistrement.

---

## Module 4 : Ergonomie souris, bascule USB -> Wi-Fi à chaud & Icône Tray SDL3
**Fichiers :** `app/src/input_manager.c`, `app/src/adb/adb.h`, `app/src/adb/adb.c`, `app/src/screen.c`, `app/src/scrcpy.c`, `app/src/events.h`

1. **Boutons latéraux de la souris & Volume à la molette :**
   - Dans `input_manager.c`, intercepter `SDL_BUTTON_X1` (bouton Précédent) pour injecter `AKEYCODE_BACK`, et `SDL_BUTTON_X2` (bouton Suivant) pour injecter `AKEYCODE_APP_SWITCH` (en mode SDK, lorsque la capture souris relative n'est pas active).
   - Intercepter `shortcut_mod` (`Alt`) + molette verticale (`SDL_EVENT_MOUSE_WHEEL`) pour injecter `AKEYCODE_VOLUME_UP` / `AKEYCODE_VOLUME_DOWN`.
2. **Raccourci « Bascule USB vers Wi-Fi » (`MOD + W`) :**
   - Ajouter un helper dans `app/src/adb/adb.c` qui récupère l'adresse IP Wi-Fi de l'appareil connecté (ex: via `adb shell ip route` ou `dumpsys wifi`), exécute `adb tcpip 5555`, puis tente `adb connect <IP>:5555`.
   - Déclencher cette action de façon asynchrone sur `MOD + W` et afficher dans les logs l'IP connectée pour indiquer que l'utilisateur peut débrancher le câble USB et laisser `--auto-reconnect` prendre le relais.
3. **Icône dans la barre des tâches (SDL3 System Tray) :**
   - Utiliser l'API native SDL3 (`SDL_CreateTray`, `SDL_CreateTrayMenu`, `SDL_InsertTrayEntryAt`) avec l'icône existante de `scrcpy` (`app/src/icon.c`).
   - Ajouter les entrées de menu suivantes (renvoyant des événements SDL thread-safe vers la boucle principale) :
     - *Réveiller l'écran (F12)*
     - *Basculer Mode Social (F10)*
     - *Changer profil Social (F9)*
     - *Couper / Activer le son (F8)*
     - *Enregistrer vidéo (Alt+Shift+R)*
     - *Quitter*
   - Protéger l'initialisation pour qu'un échec de `SDL_CreateTray` (ex: environnement sans systray) ne bloque jamais le lancement de `scrcpy`, et libérer proprement via `SDL_DestroyTray()` à la fermeture.

---

## Module 5 : Documentation & Validation finale
1. Mettre à jour `doc/shortcuts.md` et `doc/windows_manual.md` avec le tableau complet de tous les nouveaux raccourcis (`F8`, `F9`, `Shift+F9`, `F10`, `F12`, `Alt+Y`, `Alt+W`, `Alt+Shift+O`, `Alt+Shift+M`, `Alt+Shift+R`, `X1/X2`, `Alt+Molette`) et un exemple de fichier `scrcpy_social.ini`.
2. Compiler avec `ninja -C build` (vérifier 0 erreur et 0 warning critique) et exécuter la suite de tests `meson test -C build`.
3. Vérifier qu'aucun appel spécifique POSIX ne casse la cross-compilation MinGW-w64 Windows (`mkdir` -> `_mkdir` sous `_WIN32`, etc.).
4. Fournir à la fin un résumé clair des fichiers modifiés/créés et de l'état de la compilation.