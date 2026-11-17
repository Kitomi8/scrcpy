Tu es un ingénieur système expert en C (SDL2, FFmpeg, sockets POSIX/Winsock) et en développement Android bas niveau (Java, MediaCodec). 
Nous travaillons sur mon fork local de `scrcpy`. Je souhaite que tu appliques directement dans l'espace de travail les optimisations de latence et les nouvelles fonctionnalités suivantes, en respectant strictement le style de codage et l'architecture existante du projet.

### 1. Fonctionnalité : Auto-Reconnexion (`--auto-reconnect`)
- **Fichier `app/src/options.h`** : Ajoute `bool auto_reconnect;` dans `struct scrcpy_options` (initialisé à `false` par défaut dans `app/src/options.c`).
- **Fichier `app/src/cli.c`** : Ajoute le flag `--auto-reconnect` (sans argument) pour activer `opts->auto_reconnect = true;`.
- **Fichier `app/src/main.c`** : Dans `main_scrcpy()`, englobe l'appel `enum scrcpy_exit_code ret = scrcpy(&args.opts);` dans une boucle `do { ... } while`. Si `ret == SCRCPY_EXIT_DISCONNECTED && args.opts.auto_reconnect`, affiche un log (`LOGI("Device disconnected. Reconnecting in 2 seconds...");`), attends 2000 ms (`SDL_Delay(2000)`), et relance la connexion proprement.

### 2. Fonctionnalité : Touche dédiée (`F12`) pour réveiller et allumer l'écran
- **Fichier `app/src/input_manager.c`** : Dans `sc_input_manager_process_key()`, intercepte la touche `SDLK_F12` (sans nécessiter de touche modificatrice `MOD`).
- Lors d'un événement `SDL_KEYDOWN` (sans `repeat`) et si `im->controller` est actif :
  1. Appelle `sc_input_manager_req_set_display_power(im, true);` pour rallumer physiquement la dalle de l'écran.
  2. Crée et envoie un `struct sc_control_msg` de type `SC_CONTROL_MSG_TYPE_INJECT_KEYCODE` avec `AKEYCODE_WAKEUP` (`AKEY_EVENT_ACTION_DOWN` suivi de `AKEY_EVENT_ACTION_UP`) via `sc_controller_push_msg()` pour réveiller le système Android s'il est en veille.

### 3. Optimisation Réseau : Désactivation de l'algorithme de Nagle (`TCP_NODELAY`)
- **Fichier `app/src/util/net.c`** : Assure-toi que pour toutes les sockets de contrôle et vidéo connectées (notamment dans `net_connect` ou lors de la configuration des sockets TCP), l'option `TCP_NODELAY` (`IPPROTO_TCP`) est forcée à `1` via `setsockopt()` afin de réduire à zéro la latence d'envoi des événements souris/clavier en mode ADB sans fil (TCP/IP).

### 4. Optimisation Vidéo Bas-Latence (Serveur Android)
- **Fichier `server/src/main/java/com/genymobile/scrcpy/video/SurfaceEncoder.java`** : Dans la configuration du `MediaFormat`, ajoute de manière sécurisée (pour les versions d'Android compatibles) les clés de réduction de latence :
  - `KEY_LATENCY` réglé à `0`.
  - `KEY_PRIORITY` réglé à `0` (priorité temps réel).
  - Réduis `KEY_I_FRAME_INTERVAL` à `1` ou `2` secondes pour accélérer la récupération d'image en cas de perte de paquets Wi-Fi.

### 5. Affichage des FPS en direct dans la barre de titre (Optionnel / UI)
- **Fichiers `app/src/fps_counter.c` et `app/src/screen.c`** : Lorsque le compteur de FPS est actif (`--print-fps`), mets également à jour le titre de la fenêtre SDL avec les FPS actuels (ex: `"NomAppareil [60 FPS]"`).

### Instructions d'exécution :
1. Inspecte chacun des fichiers mentionnés avant de les modifier pour vérifier les signatures exactes des fonctions de cette version de `scrcpy`.
2. Applique les modifications fichier par fichier.
3. Vérifie que la compilation du client C passe sans aucun warning ni erreur via `ninja -Cx build`.