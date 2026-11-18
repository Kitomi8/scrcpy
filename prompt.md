# Prompt : Implémentation du Mode "Full Clavier & Réseaux Sociaux" (F10) dans scrcpy v5.0.1

## Contexte du projet
Le projet est `scrcpy v5.0.1` (utilisant **SDL3** côté client C dans `app/src/`).
Des fonctionnalités personnalisées ont déjà été intégrées et **doivent être intégralement préservées** :
1. `--auto-reconnect` (`options.h`, `options.c`, `cli.c`, `main.c`)
2. Touche `F12` pour réveiller et rallumer l'écran (`input_manager.c`)
3. `TCP_NODELAY` (`net.c`) et basse latence (`SurfaceEncoder.java`)
4. Affichage dynamique des FPS dans la barre de titre SDL (`events.h`, `fps_counter.c`, `screen.c`)

## Objectif
Ajouter un **Mode Navigation & Réseaux Sociaux (TikTok, Instagram Lite, Facebook Lite, Reels, Stories & Contrôle Système)** activable et désactivable à la volée avec la touche **`F10`**.
- Quand le mode est **DÉSACTIVÉ** (par défaut) : le clavier fonctionne normalement pour saisir du texte sur Android.
- Quand le mode est **ACTIVÉ (`F10`)** : les touches du clavier déclenchent des gestes tactiles proportionnels à la résolution de l'écran (`im->screen->frame_size`) ou des commandes système Android.

---

## Spécifications techniques à implémenter

### 1. Indicateur visuel dans la barre de titre (`app/src/screen.h` & `app/src/screen.c`)
- Ajouter un booléen ou une fonction permettant d'indiquer dans la barre de titre de la fenêtre SDL si le mode `F10` est actif.
- Format du titre attendu :
  - Mode normal : `NomAppareil [60 FPS]` (ou `NomAppareil` si FPS inactif)
  - Mode F10 actif : `NomAppareil [60 FPS] [SOCIAL: ON]` (ou `NomAppareil [SOCIAL: ON]`)
- Mettre à jour le titre immédiatement lors de l'appui sur `F10`.

### 2. Moteur de gestes tactiles proportionnels (`app/src/input_manager.c`)
Implémenter des fonctions utilitaires internes utilisant `sc_controller_push_msg(im->controller, &msg)` avec `SC_CONTROL_MSG_TYPE_INJECT_TOUCH_EVENT` et des coordonnées relatives (`float rx, float ry` entre `0.0f` et `1.0f`) calculées dynamiquement sur `im->screen->frame_size.width` et `im->screen->frame_size.height` :

1. **Tap simple (`sc_social_tap`)** :
   - `AMOTION_EVENT_ACTION_DOWN` (`pressure = 1.0f`) -> court délai (`25 ms`) -> `AMOTION_EVENT_ACTION_UP` (`pressure = 0.0f`).
   - Utiliser `POINTER_ID_GENERIC_FINGER` (ou `(uint64_t)-1` / constante existante dans `control_msg.h`).
2. **Double-Tap rapide (`sc_social_double_tap`)** :
   - Deux appels à `sc_social_tap` séparés de `55 ms` pour déclencher le "Like" universel sur TikTok et Instagram Reels.
3. **Swipe fluide interpolé (`sc_social_swipe`)** :
   - Interpoler le mouvement entre `(rx1, ry1)` et `(rx2, ry2)` en **6 à 8 étapes** (`AMOTION_EVENT_ACTION_MOVE`) avec un délai d'environ `10 à 14 ms` par étape avant `AMOTION_EVENT_ACTION_UP`, afin que le système Android détecte une véritable vélocité de défilement (fling) pour passer au Reel/TikTok suivant.
4. **Légère variation aléatoire (Jitter ±1%)** :
   - Ajouter une micro-variation aléatoire de quelques pixels sur les coordonnées `x` et `y` pour rendre les interactions naturelles.

### 3. Table de mappage des touches (Quand le mode F10 est ACTIVÉ)
Dans `sc_input_manager_process_key()` (`app/src/input_manager.c`), intercepter `SDLK_F10` (sans modificateur) sur `down && !repeat` pour basculer l'état du mode et afficher un `LOGI(...)` clair.
Lorsque le mode est actif et que `shortcut_mod` n'est pas pressé, intercepter les touches suivantes (en utilisant les constantes **SDL3** compatibles de `<SDL3/SDL_keycode.h>`) :

#### A. Navigation Vidéos Courtes (TikTok / Reels / Shorts) & Défilement
- **`Flèche Bas` ou `J`** : Vidéo suivante / Défiler vers le bas -> Swipe vertical du bas vers le haut (`rx=0.50, ry1=0.78` vers `rx=0.50, ry2=0.22`).
- **`Flèche Haut` ou `K`** : Vidéo précédente / Défiler vers le haut -> Swipe vertical du haut vers le bas (`rx=0.50, ry1=0.22` vers `rx=0.50, ry2=0.78`).
- **`Espace`** : Pause / Lecture vidéo -> Tap simple au centre (`rx=0.50, ry=0.50`).

#### B. Navigation Stories (Instagram Lite / Facebook Lite / TikTok)
- **`Flèche Droite` ou `D`** (sans Shift) : Story suivante -> Tap court à droite de l'écran (`rx=0.85, ry=0.50`).
- **`Flèche Gauche` ou `Q`** (sans Shift) : Story précédente -> Tap court à gauche de l'écran (`rx=0.15, ry=0.50`).
- **`Shift + Flèche Droite` ou `Shift + D`** : Passer au compte/catégorie de Story suivant -> Swipe horizontal de droite à gauche (`rx1=0.85, ry=0.50` vers `rx2=0.15, ry=0.50`).
- **`Shift + Flèche Gauche` ou `Shift + Q`** : Revenir au compte/catégorie de Story précédent -> Swipe horizontal de gauche à droite (`rx1=0.15, ry=0.50` vers `rx2=0.85, ry=0.50`).

#### C. Interactions Réseaux Sociaux (Like, Commentaire, Partage, Profil, Messages)
- **`L`** : Like universel (TikTok / Reels / Posts) -> Double-tap au centre (`rx=0.50, ry=0.50`).
- **`F`** : Bouton Like latéral (TikTok / Reels / Facebook Lite) -> Tap simple sur la colonne d'icônes à droite (`rx=0.92, ry=0.53`).
- **`C`** : Ouvrir les Commentaires -> Tap sur l'icône Commentaires à droite (`rx=0.92, ry=0.64`).
- **`S`** : Partager la publication -> Tap sur l'icône Partager à droite (`rx=0.92, ry=0.75`).
- **`P`** (sans Shift) : Voir le Profil du créateur actuel -> Swipe de droite à gauche ou tap sur l'avatar créateur (`rx=0.92, ry=0.42`).
- **`Shift + P`** : Ouvrir mon propre Profil -> Tap dans le coin inférieur droit de la barre de navigation (`rx=0.90, ry=0.94`).
- **`M`** (sans Shift) : Ouvrir la Messagerie / DM (Instagram Lite / Facebook Lite) -> Tap dans le coin supérieur droit (`rx=0.91, ry=0.07`).
- **`Shift + M`** : Ouvrir la Boîte de réception (TikTok Inbox) -> Tap sur l'onglet inférieur (`rx=0.70, ry=0.94`).
- **`R`** : Actualiser le fil (Pull-to-refresh) -> Swipe long du haut vers le bas (`rx=0.50, ry1=0.20` vers `rx=0.50, ry2=0.82`).
- **`A`** : Onglet Accueil de l'application (Feed principal) -> Tap dans le coin inférieur gauche (`rx=0.10, ry=0.94`).

#### D. Contrôle Global du Téléphone & Utilitaires Android
- **`N`** (sans Shift) : Dérouler le panneau de Notifications (`SC_CONTROL_MSG_TYPE_EXPAND_NOTIFICATION_PANEL`).
- **`Shift + N`** : Replier le panneau de Notifications/Paramètres (`SC_CONTROL_MSG_TYPE_COLLAPSE_PANELS`).
- **`U`** : Ouvrir les Paramètres rapides Android (`SC_CONTROL_MSG_TYPE_EXPAND_SETTINGS_PANEL`).
- **`Backspace` ou `Escape`** : Replier les panneaux ouverts + Injecter `AKEYCODE_BACK` (Quitter les commentaires, une story ou revenir en arrière).
- **`H`** : Injecter `AKEYCODE_HOME` (Retour à l'écran d'accueil Android).
- **`Tab`** : Injecter `AKEYCODE_APP_SWITCH` (Afficher le multitâche / applications récentes).
- **`PageUp`** : Injecter `AKEYCODE_VOLUME_UP` (Monter le volume).
- **`PageDown`** : Injecter `AKEYCODE_VOLUME_DOWN` (Baisser le volume).
- **`V`** : Capture d'écran Android native (`AKEYCODE_SYSRQ`).

---

## Contraintes de qualité et vérification
1. Inspecter d'abord `app/src/input_manager.c`, `app/src/input_manager.h`, `app/src/screen.c`, `app/src/screen.h`, `app/src/control_msg.h` et `app/src/android/keycodes.h` pour respecter strictement les signatures et conventions du projet `scrcpy v5.0.1` sous SDL3.
2. Vérifier que les constantes de touches SDL3 (`SDLK_A`..`SDLK_Z`, `SDL_KMOD_SHIFT`) compilent sans warning avec `-Werror` ou les flags du `meson.build`.
3. Ne pas bloquer les raccourcis natifs existants utilisant `shortcut_mod` (`Alt` / `Super`) ni la touche `F12` ajoutée précédemment.