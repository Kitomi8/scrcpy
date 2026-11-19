# Prompt : Correctif Auto-Reconnect Windows (`CreateProcessW error 2`) + Rotations d'écran Mode Social (`F10`)

## Contexte du projet
Dépôt `scrcpy v5.0.1` (client C sous SDL3 dans `app/src/`).
Conserver intégralement les fonctionnalités déjà présentes : `--auto-reconnect`, touche `F12` (réveil écran), `TCP_NODELAY`, affichage FPS / état dans le titre de la fenêtre et `temp_monitor.c`.

---

## Tâche 1 : Corriger le bug de reconnexion Windows (`ERROR: CreateProcessW() error 2`)

### Symptôme observé
Lors de la première connexion sous Windows, `scrcpy` fonctionne normalement. Après débranchement/déconnexion, la fenêtre passe en état `[Disconnected]` et la boucle `--auto-reconnect` échoue en boucle avec :
- `ERROR: CreateProcessW() error 2`
- `ERROR: Command not found:` (chaîne vide après les deux-points)
- `ERROR: Could not execute "adb devices -l"`

### Cause probable et correction exigée
Ce comportement indique que le chemin de l'exécutable `adb` stocké en mémoire est libéré (`free()`) ou corrompu avant la tentative de reconnexion (use-after-free / cycle de vie `sc_adb_init()` vs `sc_adb_destroy()`).

1. **Inspecter `app/src/adb/adb.c` et `app/src/adb/adb.h`** :
   - Vérifier comment le chemin `adb` (`adb_command` ou variable statique équivalente) est initialisé, utilisé et libéré.
   - Dans `sc_adb_destroy()`, remettre systématiquement le pointeur libéré à `NULL` après `free(...)` pour empêcher toute lecture d'une zone mémoire libérée.
   - Dans la fonction qui retourne/résout la commande `adb` (avant d'appeler `sc_process_execute`), vérifier que le pointeur n'est ni `NULL` ni une chaîne vide `""` ; s'il vaut `NULL`, réinitialiser automatiquement via `sc_adb_init()` (ou re-résoudre `getenv("ADB")` / `"adb"`).
2. **Inspecter `app/src/scrcpy.c` et `app/src/main.c`** :
   - Vérifier où `sc_adb_init()` et `sc_adb_destroy()` sont appelés par rapport à la boucle de reconnexion (`--auto-reconnect` et `adb devices -l`).
   - S'assurer que `sc_adb_destroy()` n'est **jamais** appelé tant que la boucle d'attente/reconnexion peut encore exécuter `adb devices -l` ou relancer `scrcpy()`.
   - Si la boucle tourne autour de `scrcpy()` dans `main.c`, appeler `sc_adb_init()` une fois avant la boucle et `sc_adb_destroy()` une seule fois après la sortie définitive, ou réinitialiser proprement `sc_adb_init()` à chaque itération.
3. **Vérification Windows (`CreateProcessW`)** :
   - Vérifier que le chemin passé à `sc_adb_execute()` / `sc_process_execute()` est toujours valide et non vide après une déconnexion suivie d'une reconnexion sous Windows.

---

## Tâche 2 : Rendre le Mode Navigation & Réseaux Sociaux (`F10`) compatible avec les rotations d'écran

### Problème à corriger
Quand on pivote la fenêtre avec `Alt + Flèche Gauche` ou `Alt + Flèche Droite` (`MOD + Left / Right`), `scrcpy` change `screen->orientation` et `screen->rect` côté client sans modifier `screen->frame_size`. Les coordonnées relatives `(rx, ry)` du mode `F10` ne suivent donc plus l'affichage pivoté.

### Modifications attendues
1. **Dans `app/src/screen.h` et `app/src/screen.c`** :
   - Ajouter la fonction :
     `struct sc_point sc_screen_convert_relative_to_frame_coords(struct sc_screen *screen, float rx, float ry);`
   - Clamper `rx` et `ry` entre `0.0f` et `1.0f`.
   - Calculer le point visuel dans `screen->rect` (`px = screen->rect.x + rx * screen->rect.w`, `py = screen->rect.y + ry * screen->rect.h`).
   - Prendre en compte le ratio HiDPI entre `SDL_GetWindowSize` et `SDL_GetWindowSizeInPixels` (SDL3), puis réutiliser `sc_screen_convert_window_to_frame_coords(screen, wx, wy)` afin de gérer automatiquement toutes les orientations (`0°`, `90°`, `180°`, `270°` et variantes `FLIP_*`) ainsi que les bandes noires.
   - Si `screen->rect.w <= 0 || screen->rect.h <= 0`, utiliser un fallback sécurisé sur `screen->frame_size`.
   - Clamper le `sc_point` final dans `[0, frame_size.width - 1]` et `[0, frame_size.height - 1]`.

2. **Dans `app/src/input_manager.c`** :
   - Utiliser `sc_screen_convert_relative_to_frame_coords(im->screen, rx, ry)` dans la fonction d'injection tactile du mode `F10` au lieu du calcul direct sur `frame_size`.
   - Conserver le jitter aléatoire en le clampant dans les bornes de `im->screen->frame_size`.
   - Garantir que `Alt + Flèche Gauche`, `Alt + Flèche Droite` et `Alt + R` (`shortcut_mod`) ne sont jamais bloqués lorsque le mode `F10` est activé.

---

## Vérification finale
1. Vérifier que tous les fichiers modifiés (`app/src/adb/adb.c`, `app/src/scrcpy.c`, `app/src/main.c`, `app/src/screen.h`, `app/src/screen.c`, `app/src/input_manager.c`) compilent sans erreur ni warning sous Linux (GCC) et en cross-compilation Windows (MinGW-w64 / SDL3).
2. Ne casser aucun test unitaire existant dans `app/tests/`.
