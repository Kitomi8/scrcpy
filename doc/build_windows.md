# Build scrcpy for Windows 64-bit (Win64)

This guide provides step-by-step instructions to compile `scrcpy.exe` for Windows 64-bit, either by cross-compiling from Linux (the recommended official method) or natively on Windows using MSYS2.

---

## 1. Method 1: Cross-compile from Linux (recommended)

This method produces a 64-bit Windows executable and a complete self-contained portable package directly from a Linux machine (Debian, Ubuntu, or derivatives).

### 1.1. System Requirements

Install MinGW-w64 cross-compilers, NASM, Meson, Ninja, and the required build utilities:

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

### 1.2. MinGW pkg-config wrapper

Ensure a `x86_64-w64-mingw32-pkg-config` wrapper script is available in your `PATH` (for instance in `~/.local/bin`):

```bash
mkdir -p ~/.local/bin
cat << 'EOF' > ~/.local/bin/x86_64-w64-mingw32-pkg-config
#!/bin/sh
exec pkg-config "$@"
EOF
chmod +x ~/.local/bin/x86_64-w64-mingw32-pkg-config
```

### 1.3. Build

From the root directory of the scrcpy repository, run the Windows 64-bit release build script:

```bash
# Build all dependencies (SDL3, FFmpeg, dav1d, LibUSB, ADB) and scrcpy.exe
./release/build_windows.sh 64
```

This script automatically performs the following steps:
1. Downloads and extracts ADB for Windows (`app/deps/adb_windows.sh`).
2. Builds the SDL3 multimedia library as a 64-bit DLL (`app/deps/sdl.sh win64 cross shared`).
3. Builds the dav1d AV1 decoder as a static library (`app/deps/dav1d.sh win64 cross shared`).
4. Builds FFmpeg (libavcodec, libavformat, libavutil, libswresample with D3D11VA hardware acceleration) as 64-bit DLLs (`app/deps/ffmpeg.sh win64 cross shared`).
5. Builds LibUSB as a 64-bit DLL for USB OTG and HID simulation (`app/deps/libusb.sh win64 cross shared`).
6. Configures Meson using `cross_win64.txt` with portable release settings (`-Dportable=true`).
7. Compiles `scrcpy.exe` using Ninja and gathers all binaries and DLLs into `release/work/build-win64/dist/`.

### 1.4. Package into a portable ZIP archive

To bundle `scrcpy.exe`, `scrcpy-server`, and all required DLLs into a standalone ZIP archive:

```bash
# Provide the scrcpy-server binary (if not compiled locally)
mkdir -p release/work/build-server/server
# Download matching scrcpy-server v5.0.1:
curl -L -o release/work/build-server/server/scrcpy-server https://github.com/Genymobile/scrcpy/releases/download/v5.0.1/scrcpy-server-v5.0.1

# Generate the standalone ZIP package
./release/package_client.sh win64 zip
```

The ZIP archive will be created in `release/output/scrcpy-win64-<version>.zip`.

---

## 2. Method 2: Native build on Windows with MSYS2

If you prefer building directly on Windows, you can use the **MSYS2** environment.

### 2.1. Install tools in MSYS2

Open an **MSYS2 MINGW64** terminal and install the required packages:

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

### 2.2. Configure and compile

In the **MSYS2 MINGW64** terminal:

```bash
# Download prebuilt scrcpy-server
curl -L -o scrcpy-server https://github.com/Genymobile/scrcpy/releases/download/v5.0.1/scrcpy-server-v5.0.1

# Configure Meson in portable mode without building the server
meson setup build-win --buildtype=release --strip -Db_lto=true \
    -Dcompile_server=false \
    -Dportable=true

# Compile
ninja -C build-win
```

### 2.3. Deployment

To run scrcpy outside the MSYS2 terminal:
1. Create a destination folder.
2. Copy `build-win/app/scrcpy.exe` and `scrcpy-server` into that folder.
3. Copy the required DLLs from `/mingw64/bin/`:
   - `SDL3.dll`
   - `avcodec-*.dll`, `avformat-*.dll`, `avutil-*.dll`, `swresample-*.dll`
   - `libusb-1.0.dll`
   - MinGW runtime DLLs (`libwinpthread-1.dll`, `libgcc_s_seh-1.dll`, etc.)
4. Download the Google Android *platform-tools* to extract `adb.exe`, `AdbWinApi.dll`, and `AdbWinUsbApi.dll` into the same folder.

---

## 3. Distribution package contents (Win64)

A complete standalone portable distribution contains:

| File | Description |
|---|---|
| `scrcpy.exe` | 64-bit Windows scrcpy client |
| `scrcpy-server` | Java/Android server binary executed on the device |
| `SDL3.dll` | Video rendering and keyboard/mouse event handling |
| `avcodec-63.dll` | FFmpeg video and audio decoder |
| `avformat-63.dll` | FFmpeg stream demuxer |
| `avutil-61.dll` | FFmpeg core utilities |
| `swresample-7.dll` | FFmpeg audio resampling |
| `libusb-1.0.dll` | Native USB communication (OTG mode) |
| `adb.exe` | Android Debug Bridge executable |
| `AdbWinApi.dll` | ADB Windows library |
| `AdbWinUsbApi.dll` | ADB WinUSB driver interface |
| `scrcpy-noconsole.vbs` | Quiet launcher without opening a command prompt window |
| `open_a_terminal_here.bat` | Shortcut to open a terminal prompt in the folder |

---

## 4. Usage on Windows

1. Connect your Android phone with **USB debugging** enabled.
2. Launch `scrcpy.exe`.
3. **Features and Shortcuts:**
   - <kbd>Esc</kbd> or <kbd>Alt</kbd>+<kbd>Esc</kbd>: Android BACK key.
   - <kbd>Alt</kbd>+<kbd>t</kbd>: Toggle real-time phone temperature in the window title bar.
   - `--temperature` / `--show-temperature`: Enable the phone temperature monitor on startup.
   - `--print-temperature`: Print device temperature measurements to console.
   - `--auto-reconnect`: Automatically reconnect on connection loss.
