# Guide de compilation pour Windows 64-bit (Win64)

Ce guide décrit en détail les étapes pour compiler `scrcpy.exe` en 64-bit pour Windows, soit par cross-compilation depuis Linux (méthode officielle et la plus rapide), soit nativement sous Windows avec MSYS2.

---

## 1. Méthode 1 : Cross-compilation depuis Linux (recommandée)

Cette méthode permet de générer un exécutable Windows 64-bit ainsi qu'un package portable autonome complet directement depuis une machine Linux (Debian / Ubuntu / dérivés).

### 1.1. Dépendances système requises

Installez les outils de compilation croisée MinGW-w64, NASM, Meson, Ninja et les utilitaires nécessaires :

```bash
sudo apt update
sudo apt install -y \
    gcc-mingw-w64-x86-64 \
    g++-mingw-w64-x86-64 \
    mingw-w64-tools \
    libz-mingw-w64-dev \
    nasm \
    meson \
    ninja-build \
    pkg-config \
    zip \
    wget \
    cmake \
    autoconf \
    automake \
    libtool
```

### 1.2. Wrapper pkg-config pour MinGW

Assurez-vous que le wrapper `x86_64-w64-mingw32-pkg-config` est présent et accessible dans votre variable d'environnement `PATH` (par exemple dans `~/.local/bin`) :

```bash
mkdir -p ~/.local/bin
cat << 'EOF' > ~/.local/bin/x86_64-w64-mingw32-pkg-config
#!/bin/sh
exec pkg-config "$@"
EOF
chmod +x ~/.local/bin/x86_64-w64-mingw32-pkg-config
```

### 1.3. Lancement de la compilation

À la racine du dépôt scrcpy, exécutez le script de release pour Windows 64-bit :

```bash
# Compiler toutes les dépendances (SDL3, FFmpeg, dav1d, LibUSB, ADB) et scrcpy.exe
./release/build_windows.sh 64
```

Ce script effectue automatiquement les opérations suivantes :
1. Télécharge et extrait ADB pour Windows (`app/deps/adb_windows.sh`).
2. Compile la bibliothèque multimédia SDL3 en DLL 64-bit (`app/deps/sdl.sh win64 cross shared`).
3. Compile la bibliothèque de décodage AV1 dav1d en bibliothèque statique (`app/deps/dav1d.sh win64 cross shared`).
4. Compile FFmpeg (libavcodec, libavformat, libavutil, libswresample avec support D3D11VA) en DLLs 64-bit (`app/deps/ffmpeg.sh win64 cross shared`).
5. Compile LibUSB en DLL 64-bit pour le mode USB OTG / HID (`app/deps/libusb.sh win64 cross shared`).
6. Configure le projet scrcpy avec Meson en utilisant `cross_win64.txt` et `-Dportable=true`.
7. Compile `scrcpy.exe` via Ninja et rassemble tous les binaires et DLLs dans `release/work/build-win64/dist/`.

### 1.4. Génération de l'archive ZIP autonome

Pour regrouper `scrcpy.exe`, `scrcpy-server` et toutes les DLLs dans une archive ZIP distribuable :

```bash
# Placer le binaire scrcpy-server (si non compilé localement)
mkdir -p release/work/build-server/server
# Copier ou télécharger scrcpy-server v5.0.1 :
curl -L -o release/work/build-server/server/scrcpy-server https://github.com/Genymobile/scrcpy/releases/download/v5.0.1/scrcpy-server-v5.0.1

# Créer le package ZIP autonome
./release/package_client.sh win64 zip
```

L'archive ZIP sera générée dans `release/output/scrcpy-win64-<version>.zip`.

---

## 2. Méthode 2 : Compilation native sous Windows avec MSYS2

Si vous préférez compiler directement sous Windows, vous pouvez utiliser l'environnement **MSYS2**.

### 2.1. Installation des outils dans MSYS2

Ouvrez un terminal **MSYS2 MINGW64** et installez les paquets requis :

```bash
pacman -Syu
pacman -S --needed \
    mingw-w64-x86_64-toolchain \
    mingw-w64-x86_64-meson \
    mingw-w64-x86_64-ninja \
    mingw-w64-x86_64-pkg-config \
    mingw-w64-x86_64-sdl3 \
    mingw-w64-x86_64-ffmpeg \
    mingw-w64-x86_64-libusb \
    mingw-w64-x86_64-dav1d \
    git \
    unzip \
    curl
```

### 2.2. Configuration et compilation

Toujours dans le terminal **MSYS2 MINGW64** :

```bash
# Récupérer scrcpy-server précompilé
curl -L -o scrcpy-server https://github.com/Genymobile/scrcpy/releases/download/v5.0.1/scrcpy-server-v5.0.1

# Configurer Meson en mode portable
meson setup build-win --buildtype=release --strip -Db_lto=true \
    -Dcompile_server=false \
    -Dportable=true

# Compiler
ninja -C build-win
```

### 2.3. Déploiement

Pour exécuter scrcpy en dehors du terminal MSYS2 :
1. Créez un dossier de destination.
2. Copiez `build-win/app/scrcpy.exe` et `scrcpy-server` dans ce dossier.
3. Copiez les DLLs depuis `/mingw64/bin/` :
   - `SDL3.dll`
   - `avcodec-*.dll`, `avformat-*.dll`, `avutil-*.dll`, `swresample-*.dll`
   - `libusb-1.0.dll`
   - Les DLLs d'exécution MinGW (`libwinpthread-1.dll`, `libgcc_s_seh-1.dll`, etc.)
4. Téléchargez les *platform-tools* Google pour obtenir `adb.exe`, `AdbWinApi.dll` et `AdbWinUsbApi.dll` et placez-les dans le même dossier.

---

## 3. Contenu du package de distribution (Win64)

Un déploiement fonctionnel de scrcpy pour Windows doit comporter :

| Fichier | Description |
|---|---|
| `scrcpy.exe` | Client scrcpy pour Windows 64-bit |
| `scrcpy-server` | Binaire serveur Java/Android exécuté sur l'appareil |
| `SDL3.dll` | Affichage vidéo et gestion des événements clavier / souris |
| `avcodec-63.dll` | Décodeur vidéo/audio FFmpeg |
| `avformat-63.dll` | Démultiplexeur de flux FFmpeg |
| `avutil-61.dll` | Utilitaires FFmpeg |
| `swresample-7.dll` | Rééchantillonnage audio FFmpeg |
| `libusb-1.0.dll` | Communication USB native (mode OTG) |
| `adb.exe` | Outil de commande Android Debug Bridge |
| `AdbWinApi.dll` | Bibliothèque Windows pour ADB |
| `AdbWinUsbApi.dll` | Pilote WinUSB pour ADB |
| `scrcpy-noconsole.vbs` | Lanceur discret sans invite de commande noire |
| `open_a_terminal_here.bat` | Raccourci pour ouvrir un terminal de commande dans le dossier |

---

## 4. Utilisation sous Windows

1. Branchez votre téléphone Android avec le débogage USB activé.
2. Lancez `scrcpy.exe`.
3. **Fonctionnalités et raccourcis :**
   - <kbd>Échap</kbd> ou <kbd>Alt</kbd>+<kbd>Échap</kbd> : Touche Retour Android.
   - <kbd>Alt</kbd>+<kbd>t</kbd> : Afficher/masquer le moniteur de température du téléphone en temps réel dans la barre de titre.
   - `--temperature` / `--show-temperature` : Activer le moniteur de température au démarrage.
   - `--print-temperature` : Afficher la température mesurée dans la console.
   - `--auto-reconnect` : Reconnexion automatique en cas de déconnexion.
