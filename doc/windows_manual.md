# Scrcpy User Manual for Windows

This manual provides a comprehensive guide to using **scrcpy** on Windows, including all essential command-line options, configuration tips, batch script setups, keyboard shortcuts, and newly integrated features such as **real-time phone temperature monitoring** and the **Back (<kbd>Esc</kbd>) shortcut**.

---

## Table of Contents
1. [Quick Start](#1-quick-start)
2. [Launching Scrcpy on Windows](#2-launching-scrcpy-on-windows)
3. [Newly Added Features](#3-newly-added-features)
4. [Connection & Wireless (TCP/IP)](#4-connection--wireless-tcpip)
5. [Video & Display Options](#5-video--display-options)
6. [Audio Options](#6-audio-options)
7. [Device Control & Physical Screen](#7-device-control--physical-screen)
8. [Window Customization](#8-window-customization)
9. [Recording](#9-recording)
10. [Physical Keyboard, Mouse & OTG Mode](#10-physical-keyboard-mouse--otg-mode)
11. [Complete Keyboard Shortcuts Table](#11-complete-keyboard-shortcuts-table)
12. [Ready-to-use Windows Batch Scripts (.bat)](#12-ready-to-use-windows-batch-scripts-bat)
13. [Troubleshooting on Windows](#13-troubleshooting-on-windows)

---

## 1. Quick Start

### Prerequisites
1. **Enable Developer Options & USB Debugging** on your Android device:
   - Go to *Settings* > *About phone* and tap *Build number* 7 times.
   - Go to *Settings* > *System* > *Developer options* and turn on **USB debugging**.
2. **Connect via USB**:
   - Plug your device into your Windows PC.
   - Authorize USB debugging when prompted on your phone screen (*Always allow from this computer*).
3. **Ensure ADB Driver is installed**:
   - Scrcpy on Windows includes `adb.exe`. If your device is not detected, install the appropriate USB OEM driver for your phone brand.

---

## 2. Launching Scrcpy on Windows

In the scrcpy folder, you will find:
- **`open_a_terminal_here.bat`**: Double-click to open a command prompt already pointing to this directory. From here, you can type commands like `scrcpy --help` or `scrcpy [options]`.
- **`scrcpy.exe`**: Double-click to launch with default settings.
- **`scrcpy-noconsole.vbs`**: Double-click to launch scrcpy in the background without opening a black command prompt window.

---

## 3. Newly Added Features

### 3.1. Phone Temperature Monitor
Monitor the real-time battery/device temperature directly in the window title bar (e.g. `Pixel 8 [34.5°C]`).

| Option / Shortcut | Description |
|---|---|
| `--temperature` or `--show-temperature` | Starts scrcpy with the real-time temperature monitor enabled in the window title bar. |
| `--print-temperature` | Continuously logs temperature measurements to the terminal. |
| <kbd>Alt</kbd>+<kbd>t</kbd> | Toggle the temperature monitor display in the title bar on and off at runtime. |

*Example:*
```cmd
scrcpy --temperature
```

### 3.2. Back Key Shortcut (<kbd>Esc</kbd>)
Pressing <kbd>Esc</kbd> directly triggers the Android **BACK** action.

| Shortcut | Action |
|---|---|
| <kbd>Esc</kbd> | Press Android BACK button (returns to previous screen, closes popups). |
| <kbd>Alt</kbd>+<kbd>Esc</kbd> | Alternative Back shortcut using the modifier key. |
| Right-click (mouse) | Also triggers Back when the device screen is on. |

### 3.3. Seamless Automatic Reconnection
Automatic reconnection is **enabled by default**. If the USB cable is accidentally bumped, disconnected, or unplugged:
- The scrcpy window **remains open** (does not quit or disappear).
- The window displays the disconnected icon and the title bar updates to `[Disconnected - Reconnecting...]`.
- As soon as the cable is plugged back in, scrcpy detects the device and automatically resumes video mirroring in the exact same window.
- Screen off (`-S`) and your other options are automatically re-applied upon reconnection.

To disable this behavior and immediately exit on disconnection:
```cmd
scrcpy --no-auto-reconnect
```

### 3.4. Thermal Protection & Battery Telemetry
Scrcpy now integrates advanced thermal and battery health telemetry directly into the window title:
`NomAppareil [60 FPS | 35°C | 82%] [SOCIAL: TIKTOK] [REC] [MUTED] [HOT] [CHARGE >= 80%]`

- **Real-Time Battery Telemetry**: Shows device battery percentage and charging state automatically.
- **Battery Preservation Alert (`[CHARGE >= 80%]`)**: When the device is charging and reaches 80% or more, a prominent badge is shown in the title bar alongside a console notice to remind you to disconnect the cable or switch to Wi-Fi to avoid premature battery degradation.
- **Automatic Anti-Overheat Protection (`[HOT]`)**: If the device temperature reaches or exceeds **42.0°C**, scrcpy displays `[HOT]` and automatically sends a power-saving command to turn off the physical screen (while mirroring remains active and responsive on PC), quickly bringing temperatures back to safe operational limits.

### 3.5. Social Media Mode with Switchable INI Profiles (<kbd>F10</kbd>, <kbd>F9</kbd>, <kbd>Shift</kbd>+<kbd>F9</kbd>)
Press <kbd>F10</kbd> to toggle between normal keyboard typing mode and dedicated social media navigation.
- **Switch Profiles (<kbd>F9</kbd>)**: Instantly switch between `[TIKTOK]`, `[IG_LITE]`, `[FB_LITE]`, and `[YOUTUBE]` profiles. The active profile name is shown in the title bar (e.g. `[SOCIAL: TIKTOK]`).
- **Hot-Reload (<kbd>Shift</kbd>+<kbd>F9</kbd>)**: Edit coordinate ratios in `scrcpy_social.ini` and press <kbd>Shift</kbd>+<kbd>F9</kbd> to reload them live without restarting.
- **Asynchronous Gestures**: Swipes and taps run on a dedicated background worker thread, guaranteeing smooth 60 FPS video without micro-stutters during swipe sequences.

- **Reels & TikTok**: <kbd>↓</kbd> / <kbd>j</kbd> (Next), <kbd>↑</kbd> / <kbd>k</kbd> (Prev), <kbd>Space</kbd> (Pause/Play), <kbd>l</kbd> (Double-tap Like).
- **Stories**: <kbd>→</kbd> / <kbd>d</kbd> (Next story), <kbd>←</kbd> / <kbd>q</kbd> (Prev story), <kbd>Shift</kbd>+<kbd>→</kbd> / <kbd>Shift</kbd>+<kbd>←</kbd> (Next/Prev account).
- **Interactions**: <kbd>e</kbd> / <kbd>Shift</kbd>+<kbd>e</kbd> (Insert emoji / Heart reaction), <kbd>f</kbd> (Like icon), <kbd>c</kbd> (Comments), <kbd>s</kbd> (Share), <kbd>p</kbd> (Creator profile), <kbd>Shift</kbd>+<kbd>p</kbd> (My profile), <kbd>m</kbd> (DMs), <kbd>Shift</kbd>+<kbd>m</kbd> (TikTok Inbox), <kbd>r</kbd> (Pull-to-refresh), <kbd>a</kbd> (Home feed).
- **System controls**: <kbd>n</kbd> / <kbd>Shift</kbd>+<kbd>n</kbd> (Notifications), <kbd>u</kbd> (Quick Settings), <kbd>Backspace</kbd> / <kbd>Esc</kbd> (Back & dismiss), <kbd>h</kbd> (Home), <kbd>Tab</kbd> (App Switcher), <kbd>PageUp</kbd> / <kbd>PageDown</kbd> (Volume), <kbd>v</kbd> (Screenshot).

### 3.6. Multimedia & Desktop Controls
- **Audio Mute (<kbd>F8</kbd>)**: Instantly silence or restore audio output. Displays `[MUTED]` in the title bar when active.
- **Desktop Screenshot (<kbd>MOD</kbd>+<kbd>y</kbd>)**: Encodes the currently displayed frame to high-quality PNG in the `captures/` folder (`captures/scrcpy_YYYYMMDD_HHMMSS.png`).
- **On-the-Fly MP4 Recording (<kbd>MOD</kbd>+<kbd>Shift</kbd>+<kbd>r</kbd>)**: Start and stop recording the session without restarting scrcpy. Starts cleanly on the next video IDR keyframe and saves to `captures/scrcpy_rec_YYYYMMDD_HHMMSS.mp4` with a `[REC]` title badge.
- **Window Opacity (<kbd>MOD</kbd>+<kbd>Shift</kbd>+<kbd>o</kbd>)**: Cycle transparency levels: `100%` → `85%` → `70%` → `50%`.
- **Mini-Player Sticky Mode (<kbd>MOD</kbd>+<kbd>Shift</kbd>+<kbd>m</kbd>)**: Shrinks the window into a compact picture-in-picture player (~320px width) locked always-on-top.

### 3.7. Mouse Ergonomics & USB to Wi-Fi Hot-Switch
- **Mouse Side Buttons**: Button <kbd>X1</kbd> (Back) navigates back; Button <kbd>X2</kbd> (Forward) opens the App Switcher.
- **Mouse Wheel Volume**: Hold <kbd>MOD</kbd> (<kbd>Alt</kbd>) and scroll the mouse wheel to raise or lower Android volume.
- **USB to Wi-Fi Hot-Switch (<kbd>MOD</kbd>+<kbd>w</kbd>)**: Automatically detects device Wi-Fi IP, configures `adb tcpip 5555`, and connects asynchronously. You can then immediately unplug the USB cable and continue without interruption.
- **Desktop System Tray**: Right-click the scrcpy tray icon in your Windows taskbar to wake the screen (<kbd>F12</kbd>), toggle Social mode (<kbd>F10</kbd>), cycle profiles (<kbd>F9</kbd>), mute audio (<kbd>F8</kbd>), toggle recording, or quit.

---

## 4. Connection & Wireless (TCP/IP)

### 4.1. Selecting a Device
When multiple Android devices or emulators are connected:

| Option | Description |
|---|---|
| `-d` | Use the only USB-connected device. |
| `-e` | Use the only TCP/IP (network/emulator) device. |
| `-s <serial>` | Target a specific device by its serial number (run `adb devices` to see list). |
| `--select-usb` | Prompt an interactive selection menu for USB devices. |
| `--select-tcpip` | Prompt an interactive selection menu for TCP/IP devices. |

### 4.2. Wireless Connection over Wi-Fi
Connect your device without any USB cable:

1. Connect device with USB cable once.
2. Enable TCP/IP mode:
   ```cmd
   scrcpy --tcpip
   ```
   (Scrcpy queries the device IP address, switches ADB to TCP/IP port 5555, and connects wirelessly).
3. Unplug the USB cable.
4. To reconnect subsequently directly over Wi-Fi:
   ```cmd
   scrcpy --tcpip=192.168.1.50:5555
   ```

---

## 5. Video & Display Options

| Option | Default | Description |
|---|---|---|
| `-m, --max-size=<val>` | *0 (native)* | Limit maximum video dimension (width or height) to `<val>` (e.g. `1024`, `1440`). Reduces CPU/GPU and network load. |
| `-b, --video-bit-rate=<val>` | `8M` | Set video encoding bitrate (e.g. `4M`, `12M`, `20M`). |
| `--max-fps=<val>` | *unlimited* | Cap video framerate (e.g. `--max-fps=60` or `--max-fps=30`). |
| `--video-codec=<codec>` | `h264` | Choose video codec: `h264`, `h265`, `av1` (hardware-accelerated via D3D11VA). |
| `--video-encoder=<name>` | *auto* | Select a specific hardware encoder on the device (list with `scrcpy --list-encoders`). |
| `--crop=<w>:<h>:<x>:<y>` | *full* | Mirror only a specific crop region of the screen (e.g. `--crop=1080:1920:0:0`). |
| `--display-id=<id>` | `0` | Mirror a secondary or virtual display (list with `scrcpy --list-displays`). |
| `--video-buffer=<ms>` | `0` | Add buffer delay (in milliseconds) to smooth video playback if network has jitter. |

*Example for high-performance low-latency gaming:*
```cmd
scrcpy -m 1280 --max-fps=60 -b 12M --video-codec=h265
```

---

## 6. Audio Options

> Audio forwarding is supported on **Android 11 and above**.

| Option | Default | Description |
|---|---|---|
| `--no-audio` | *false* | Disable all audio forwarding. |
| `--audio-source=<src>` | `output` | Audio source: `output` (app sound), `mic` (microphone), `playback` (captured playback). |
| `--audio-codec=<codec>`| `opus` | Audio codec: `opus`, `aac`, `flac`, or `raw`. |
| `--audio-bit-rate=<val>`| `128K` | Set audio bitrate (e.g. `64K`, `192K`). |
| `--audio-buffer=<ms>` | `50` | Buffer delay for audio in milliseconds. Lower values decrease delay; higher values prevent stutter. |
| `--audio-dup` | *false* | Play audio on both the computer and the device simultaneously (requires `--audio-source=playback`). |

*Example for streaming microphone only:*
```cmd
scrcpy --audio-source=mic
```

---

## 7. Battery Preservation, Screen-Off Mode & Thermal Management

Using a smartphone for long mirroring sessions while plugged into a PC can generate heat and cause battery wear over time. Scrcpy provides built-in mechanisms to eliminate screen wear, minimize power consumption, and protect battery health.

### 7.1. Recommended Command for Battery Protection
```cmd
scrcpy -S -w --temperature --auto-reconnect
```

- **`-S` (`--turn-screen-off`)**: Powers OFF the physical OLED/LCD display on the phone immediately when mirroring begins. The phone remains fully controllable on your PC window while its physical screen stays completely black.
- **`-w` (`--stay-awake`)**: Keeps the Android OS active while plugged in so it won't go to sleep or lock up, allowing seamless PC interaction without touching the physical phone.
- **`--temperature`**: Monitors device temperature in real-time in the title bar (e.g. `[34.2°C]`).
- **`--auto-reconnect`** *(enabled by default)*: Automatically reconnects if the cable is disconnected; the window stays open with a reconnecting screen and automatically resumes display once re-plugged. (Use `--no-auto-reconnect` to exit on disconnect).

### 7.2. Toggling the Physical Screen at Runtime
You do not need to restart scrcpy to toggle the phone's physical screen:
- **Turn phone screen OFF**: <kbd>Alt</kbd> + <kbd>o</kbd>
- **Turn phone screen ON**: <kbd>Alt</kbd> + <kbd>Shift</kbd> + <kbd>o</kbd>
- **Toggle temperature display**: <kbd>Alt</kbd> + <kbd>t</kbd>

### 7.3. Further Optimizations to Reduce Heat & Battery Wear
1. **Reduce CPU/GPU rendering load**:
   Cap resolution and frame rate to decrease video encoder workload on the device:
   ```cmd
   scrcpy -S -w -m 1280 --max-fps=60 -b 4M --temperature
   ```
2. **Auto-lock device upon closing scrcpy**:
   Add `--power-off-on-close` so that when you close the scrcpy window on your PC, the phone locks itself and enters sleep mode automatically.
3. **USB Power Tips**:
   If your phone gets warm while connected, connect it to a standard USB 2.0 port (500mA) or disable fast USB charging in Android settings while mirroring to prevent high-current charging heat.

| Option | Shortcut | Description |
|---|---|---|
| `-S, --turn-screen-off` | <kbd>Alt</kbd>+<kbd>o</kbd> | Turn physical phone screen off while keeping PC mirroring active (saves battery and limits heat). |
| `-w, --stay-awake` | - | Prevent phone from sleeping while plugged in. |
| `--temperature` / `--show-temperature` | <kbd>Alt</kbd>+<kbd>t</kbd> | Display real-time device temperature in window title bar. |
| `--print-temperature` | - | Print real-time device temperature measurements to console. |
| `--power-off-on-close` | - | Automatically put phone to sleep when scrcpy is closed. |
| `--show-touches` | - | Show visual touch indicators on device. |
| `--no-control` | - | Read-only mirror: disable all mouse, keyboard, and touch input forwarding. |

*Recommended command for daily PC usage:*
```cmd
scrcpy --turn-screen-off --stay-awake --temperature
```

---

## 8. Window Customization

| Option | Default | Description |
|---|---|---|
| `-f, --fullscreen` | *windowed* | Start in fullscreen mode. |
| `--always-on-top` | *false* | Keep scrcpy window above all other Windows applications. |
| `--window-title="Text"` | *Device name* | Set a custom window title. |
| `--window-borderless` | *false* | Remove window borders and title bar. |
| `--window-x=<val>` | *auto* | Initial horizontal window position. |
| `--window-y=<val>` | *auto* | Initial vertical window position. |
| `--window-width=<val>` | *auto* | Initial window width. |
| `--window-height=<val>`| *auto* | Initial window height. |

*Example:*
```cmd
scrcpy --always-on-top --window-title="My Phone" --window-width=450
```

---

## 9. Recording

Record your phone screen and audio directly into high-quality MP4 or MKV files on your PC:

| Option | Description |
|---|---|
| `--record=filename.mp4` | Record screen and audio to an MP4 file. |
| `--record=filename.mkv` | Record screen and audio to a Matroska (MKV) file. |
| `--record-orientation=<val>` | Force recording orientation (`0`, `90`, `180`, `270`). |
| `--no-playback` | Record in the background without displaying a mirror window on screen. |
| `--time-limit=<seconds>` | Automatically stop recording after specified seconds. |

*Example: Record 60-second clip without display window:*
```cmd
scrcpy --record=gameplay.mp4 --no-playback --time-limit=60
```

---

## 10. Physical Keyboard, Mouse & OTG Mode

### 10.1. Keyboard & Mouse Modes
By default, scrcpy injects input via Android events. For games or full special-key support, use UHID or AOA mode:

- **UHID Keyboard & Mouse**:
  ```cmd
  scrcpy --keyboard=uhid --mouse=uhid
  ```
  Simulates a physical USB keyboard and mouse connected directly to the device.

### 10.2. USB OTG Mode (No ADB required on phone)
Turn your PC keyboard and mouse into an actual physical hardware controller for your phone via USB:

```cmd
scrcpy --otg
```
*(No screen mirroring, only physical keyboard/mouse pass-through. Requires WinUSB driver via Zadig on Windows).*

---

## 11. Complete Keyboard Shortcuts Table

The default modifier key (<kbd>MOD</kbd>) on Windows is **(Left) <kbd>Alt</kbd>**.

| Shortcut | Action |
|---|---|
| <kbd>Esc</kbd> \| Mouse <kbd>X1</kbd> | **Android BACK** |
| Mouse <kbd>X2</kbd> | **Android APP_SWITCH** (Recent Apps) |
| <kbd>F8</kbd> | **Mute / Unmute Audio** (silence output with `[MUTED]` badge) |
| <kbd>F9</kbd> | **Switch Social Profile** (`TIKTOK`, `IG_LITE`, `FB_LITE`, `YOUTUBE`) |
| <kbd>Shift</kbd>+<kbd>F9</kbd> | **Hot-Reload scrcpy_social.ini** without restarting |
| <kbd>F10</kbd> | **Toggle Social & Navigation Mode** (TikTok, Reels, Stories) |
| <kbd>F12</kbd> | **Wake up device & turn screen on** |
| <kbd>MOD</kbd>+<kbd>y</kbd> | **Take Desktop Screenshot** (PNG saved in `captures/`) |
| <kbd>MOD</kbd>+<kbd>Shift</kbd>+<kbd>r</kbd> | **Toggle On-the-Fly MP4 Recording** (saved in `captures/`) |
| <kbd>MOD</kbd>+<kbd>w</kbd> | **Hot Switch USB to Wi-Fi** (connects `adb tcpip 5555` wirelessly) |
| <kbd>MOD</kbd>+<kbd>Shift</kbd>+<kbd>o</kbd> | **Cycle Window Opacity** (100% → 85% → 70% → 50%) |
| <kbd>MOD</kbd>+<kbd>Shift</kbd>+<kbd>m</kbd> | **Toggle Mini-Player Sticky Mode** (~320px width, always-on-top) |
| <kbd>MOD</kbd>+<kbd>Wheel Up</kbd> / <kbd>Down</kbd> | **Volume UP / Volume DOWN** via mouse wheel |
| <kbd>MOD</kbd>+<kbd>t</kbd> | **Toggle Phone Temperature in Title Bar** |
| <kbd>MOD</kbd>+<kbd>f</kbd> or <kbd>F11</kbd> | Toggle Fullscreen |
| <kbd>MOD</kbd>+<kbd>h</kbd> | Android HOME |
| <kbd>MOD</kbd>+<kbd>s</kbd> | Android App Switcher (Recent Apps) |
| <kbd>MOD</kbd>+<kbd>b</kbd> / <kbd>MOD</kbd>+<kbd>Backspace</kbd> | Android BACK (alternative) |
| <kbd>MOD</kbd>+<kbd>p</kbd> | Android POWER button (turn device screen on/off) |
| <kbd>MOD</kbd>+<kbd>o</kbd> | Turn physical phone screen OFF (mirroring stays active) |
| <kbd>MOD</kbd>+<kbd>Shift</kbd>+<kbd>o</kbd> | Turn physical phone screen ON |
| <kbd>MOD</kbd>+<kbd>Up</kbd> | Volume UP |
| <kbd>MOD</kbd>+<kbd>Down</kbd> | Volume DOWN |
| <kbd>MOD</kbd>+<kbd>r</kbd> | Rotate device display |
| <kbd>MOD</kbd>+<kbd>Left</kbd> / <kbd>MOD</kbd>+<kbd>Right</kbd> | Rotate mirror window left / right |
| <kbd>MOD</kbd>+<kbd>Shift</kbd>+<kbd>w</kbd> | Resize window to remove black borders |
| <kbd>MOD</kbd>+<kbd>n</kbd> | Expand notification panel |
| <kbd>MOD</kbd>+<kbd>Shift</kbd>+<kbd>n</kbd> | Collapse notification panel |
| <kbd>MOD</kbd>+<kbd>c</kbd> | Copy device clipboard to PC |
| <kbd>MOD</kbd>+<kbd>v</kbd> | Paste PC clipboard to device |
| <kbd>MOD</kbd>+<kbd>Shift</kbd>+<kbd>v</kbd> | Inject PC clipboard text as keystrokes |
| <kbd>MOD</kbd>+<kbd>i</kbd> | Toggle FPS counter display |
| <kbd>MOD</kbd>+<kbd>q</kbd> | Quit scrcpy |
| <kbd>Ctrl</kbd> + click & drag | Pinch to zoom |
| Drag & Drop APK file | Install APK onto Android device |
| Drag & Drop any file | Push file to device (`/sdcard/Download/`) |

---

## 12. Ready-to-use Windows Batch Scripts (.bat)

Create `.bat` files in your scrcpy folder to launch your preferred profiles with a single double-click.

### 🔋 0. Battery Saver & Screen-Off Profile (`battery_saver.bat`)
Powers off physical phone screen, stays awake, monitors temperature, and auto-locks on close:
```bat
@echo off
scrcpy -S -w -m 1280 --max-fps=60 -b 4M --temperature --power-off-on-close --auto-reconnect
pause
```

### 🎮 1. Gaming Profile (`gaming.bat`)
Optimized for 60 FPS, low latency, phone screen turned off to prevent overheating:
```bat
@echo off
scrcpy -m 1280 --max-fps=60 -b 10M --turn-screen-off --stay-awake --temperature --keyboard=uhid --mouse=uhid
pause
```

### 💼 2. Work & Office Profile (`work.bat`)
Keeps scrcpy pinned on top with a compact window and auto-reconnect:
```bat
@echo off
scrcpy --always-on-top --window-width=450 --turn-screen-off --stay-awake --temperature --auto-reconnect
pause
```

### 📡 3. Wireless Wi-Fi Profile (`wifi.bat`)
Connects wirelessly with automatic reconnection:
```bat
@echo off
scrcpy --tcpip --temperature --stay-awake --auto-reconnect
pause
```

### 🎥 4. Screen Recording Profile (`record.bat`)
Records high-quality 1080p MP4 file with device temperature check:
```bat
@echo off
set TIMESTAMP=%DATE:~6,4%-%DATE:~3,2%-%DATE:~0,2%_%TIME:~0,2%-%TIME:~3,2%-%TIME:~6,2%
set TIMESTAMP=%TIMESTAMP: =0%
scrcpy --record="recording_%TIMESTAMP%.mp4" --temperature
pause
```

---

## 13. Troubleshooting on Windows

### Device unauthorized
- **Symptom**: `adb server version doesn't match this client` or `error: device unauthorized`.
- **Solution**: Unplug USB cable, plug it back in, unlock your phone screen, and check the prompt: **"Allow USB debugging?"**. Check the box *Always allow from this computer* and tap *Allow*.

### Device not detected (`error: no devices/emulators found`)
1. Ensure your USB cable supports **data transfer** (not charge-only).
2. Check Windows Device Manager: if your phone shows a yellow warning triangle, install the official OEM USB driver (Samsung, Google, Xiaomi, etc.).
3. Try a different USB port (prefer USB 3.0 or direct motherboard ports rather than hubs).

### Black screen or video glitch
- Try specifying a lower resolution or changing the video codec:
  ```cmd
  scrcpy -m 1024 --video-codec=h264
  ```

### Wi-Fi connection fails
- Ensure both your Windows PC and your Android phone are on the **same Wi-Fi network**.
- Verify that your Windows Firewall is not blocking inbound connections for `adb.exe`.
