# Shortcuts

Actions can be performed on the scrcpy window using keyboard and mouse
shortcuts.

In the following list, <kbd>MOD</kbd> is the shortcut modifier. By default, it's
(left) <kbd>Alt</kbd> or (left) <kbd>Super</kbd>.

It can be changed using `--shortcut-mod`. Possible keys are `lctrl`, `rctrl`,
`lalt`, `ralt`, `lsuper` and `rsuper`. For example:

```bash
# use RCtrl for shortcuts
scrcpy --shortcut-mod=rctrl

# use either LCtrl or LSuper for shortcuts
scrcpy --shortcut-mod=lctrl,lsuper
```

_<kbd>[Super]</kbd> is typically the <kbd>Windows</kbd> or <kbd>Cmd</kbd> key._

[Super]: https://en.wikipedia.org/wiki/Super_key_(keyboard_button)

 | Action                                      |   Shortcut
 | ------------------------------------------- |:-----------------------------
 | Quit                                        | <kbd>MOD</kbd>+<kbd>q</kbd>
 | Switch fullscreen mode                      | <kbd>MOD</kbd>+<kbd>f</kbd> \| <kbd>F11</kbd>
 | Rotate display left                         | <kbd>MOD</kbd>+<kbd>←</kbd> _(left)_
 | Rotate display right                        | <kbd>MOD</kbd>+<kbd>→</kbd> _(right)_
 | Flip display horizontally                   | <kbd>MOD</kbd>+<kbd>Shift</kbd>+<kbd>←</kbd> _(left)_ \| <kbd>MOD</kbd>+<kbd>Shift</kbd>+<kbd>→</kbd> _(right)_
 | Flip display vertically                     | <kbd>MOD</kbd>+<kbd>Shift</kbd>+<kbd>↑</kbd> _(up)_ \| <kbd>MOD</kbd>+<kbd>Shift</kbd>+<kbd>↓</kbd> _(down)_
 | Pause or re-pause display                   | <kbd>MOD</kbd>+<kbd>z</kbd>
 | Unpause display                             | <kbd>MOD</kbd>+<kbd>Shift</kbd>+<kbd>z</kbd>
 | Reset video capture/encoding                | <kbd>MOD</kbd>+<kbd>Shift</kbd>+<kbd>r</kbd>
 | Resize window to 1:1 (pixel-perfect)        | <kbd>MOD</kbd>+<kbd>g</kbd>
 | Resize window to remove black borders       | <kbd>MOD</kbd>+<kbd>w</kbd> \| _Double-left-click¹_
 | Click on `HOME`                             | <kbd>MOD</kbd>+<kbd>h</kbd> \| _Middle-click_
 | Click on `BACK`                             | <kbd>MOD</kbd>+<kbd>b</kbd> \| <kbd>MOD</kbd>+<kbd>Backspace</kbd> \| <kbd>Esc</kbd> \| _Right-click²_
 | Click on `APP_SWITCH`                       | <kbd>MOD</kbd>+<kbd>s</kbd> \| _4th-click³_
 | Click on `MENU` (unlock screen)⁴            | <kbd>MOD</kbd>+<kbd>m</kbd>
 | Click on `VOLUME_UP`                        | <kbd>MOD</kbd>+<kbd>↑</kbd> _(up)_
 | Click on `VOLUME_DOWN`                      | <kbd>MOD</kbd>+<kbd>↓</kbd> _(down)_
 | Click on `POWER`                            | <kbd>MOD</kbd>+<kbd>p</kbd>
 | Power on                                    | _Right-click²_
 | Turn device screen off (keep mirroring)     | <kbd>MOD</kbd>+<kbd>o</kbd>
 | Turn device screen on                       | <kbd>MOD</kbd>+<kbd>Shift</kbd>+<kbd>o</kbd>
 | Rotate device screen                        | <kbd>MOD</kbd>+<kbd>r</kbd>
 | Expand notification panel                   | <kbd>MOD</kbd>+<kbd>n</kbd> \| _5th-click³_
 | Expand settings panel                       | <kbd>MOD</kbd>+<kbd>n</kbd>+<kbd>n</kbd> \| _Double-5th-click³_
 | Collapse panels                             | <kbd>MOD</kbd>+<kbd>Shift</kbd>+<kbd>n</kbd>
 | Copy to clipboard⁵                          | <kbd>MOD</kbd>+<kbd>c</kbd>
 | Cut to clipboard⁵                           | <kbd>MOD</kbd>+<kbd>x</kbd>
 | Synchronize clipboards and paste⁵           | <kbd>MOD</kbd>+<kbd>v</kbd>
 | Inject computer clipboard text              | <kbd>MOD</kbd>+<kbd>Shift</kbd>+<kbd>v</kbd>
 | Open keyboard settings (HID keyboard only)  | <kbd>MOD</kbd>+<kbd>k</kbd>
 | Enable/disable FPS counter (on stdout)      | <kbd>MOD</kbd>+<kbd>i</kbd>
 | Enable/disable temperature monitor          | <kbd>MOD</kbd>+<kbd>t</kbd>
 | Insert Emoji (cycle trending emojis)        | <kbd>F9</kbd> or <kbd>MOD</kbd>+<kbd>e</kbd>
 | Insert Heart emoji (❤️)                     | <kbd>Shift</kbd>+<kbd>F9</kbd> or <kbd>MOD</kbd>+<kbd>Shift</kbd>+<kbd>e</kbd>
 | Toggle Social & Navigation Mode             | <kbd>F10</kbd>
 | Wake up device & turn screen on             | <kbd>F12</kbd>
 | Pinch-to-zoom/rotate                        | <kbd>Ctrl</kbd>+_click-and-move_
 | Tilt vertically (slide with 2 fingers)      | <kbd>Shift</kbd>+_click-and-move_
 | Tilt horizontally (slide with 2 fingers)    | <kbd>Ctrl</kbd>+<kbd>Shift</kbd>+_click-and-move_
 | Drag & drop APK file                        | Install APK from computer
 | Drag & drop non-APK file                    | [Push file to device](control.md#push-file-to-device)
 | Turn on the camera torch (camera mode only) | <kbd>MOD</kbd>+<kbd>t</kbd>
 | Turn off the camera torch (camera mode only)| <kbd>MOD</kbd>+<kbd>Shift</kbd>+<kbd>t</kbd>
 | Zoom camera in (camera mode only)           | <kbd>MOD</kbd>+<kbd>↑</kbd> _(up)_
 | Zoom camera out (camera mode only)          | <kbd>MOD</kbd>+<kbd>↓</kbd> _(down)_

_¹Double-click on black borders to remove them._  
_²Right-click turns the screen on if it was off, presses BACK otherwise._  
_³4th and 5th mouse buttons, if your mouse has them._  
_⁴For react-native apps in development, `MENU` triggers development menu._  
_⁵Only on Android >= 7._

Shortcuts with repeated keys are executed by releasing and pressing the key a
second time. For example, to execute "Expand settings panel":

 1. Press and keep pressing <kbd>MOD</kbd>.
 2. Then double-press <kbd>n</kbd>.
 3. Finally, release <kbd>MOD</kbd>.

All <kbd>Ctrl</kbd>+_key_ shortcuts are forwarded to the device, so they are
handled by the active application.

## Social & Navigation Mode (<kbd>F10</kbd>)

Press <kbd>F10</kbd> to toggle between normal keyboard typing mode and **Social & Navigation Mode** (optimized for TikTok, Instagram Lite, Facebook Lite, Reels, Stories, and general Android navigation).

When active, `[SOCIAL: ON]` is indicated in the window title bar, and the following keys trigger proportional touch gestures and Android commands:

| Key | Action | Gesture / Command |
|---|---|---|
| <kbd>↓</kbd> or <kbd>j</kbd> | Next video / Scroll down | Vertical swipe up (`rx=0.50`, `ry: 0.78 → 0.22`) |
| <kbd>↑</kbd> or <kbd>k</kbd> | Previous video / Scroll up | Vertical swipe down (`rx=0.50`, `ry: 0.22 → 0.78`) |
| <kbd>Space</kbd> | Pause / Play video | Center tap (`rx=0.50`, `ry=0.50`) |
| <kbd>→</kbd> or <kbd>d</kbd> | Next story | Tap right side (`rx=0.85`, `ry=0.50`) |
| <kbd>←</kbd> or <kbd>q</kbd> | Previous story | Tap left side (`rx=0.15`, `ry=0.50`) |
| <kbd>Shift</kbd>+<kbd>→</kbd> / <kbd>Shift</kbd>+<kbd>d</kbd> | Next story account / category | Horizontal swipe right-to-left |
| <kbd>Shift</kbd>+<kbd>←</kbd> / <kbd>Shift</kbd>+<kbd>q</kbd> | Previous story account / category | Horizontal swipe left-to-right |
| <kbd>l</kbd> | Universal Like (TikTok / Reels / Posts) | Double-tap center (`rx=0.50`, `ry=0.50`) |
| <kbd>e</kbd> | Insert Emoji (cycle trending emojis) | Inject emoji (😂, ❤️, 🔥, 👍, 😍, 😭, ✨, 👏, 🙏, 🎉...) |
| <kbd>Shift</kbd>+<kbd>e</kbd> | Quick Heart reaction | Inject ❤️ |
| <kbd>f</kbd> | Lateral Like button | Tap icon column (`rx=0.92`, `ry=0.53`) |
| <kbd>c</kbd> | Open Comments | Tap comments icon (`rx=0.92`, `ry=0.64`) |
| <kbd>s</kbd> | Share post | Tap share icon (`rx=0.92`, `ry=0.75`) |
| <kbd>p</kbd> | View creator profile | Tap creator avatar (`rx=0.92`, `ry=0.42`) |
| <kbd>Shift</kbd>+<kbd>p</kbd> | Open own Profile tab | Tap bottom right navigation bar (`rx=0.90`, `ry=0.94`) |
| <kbd>m</kbd> | Open Messages / DM | Tap top right icon (`rx=0.91`, `ry=0.07`) |
| <kbd>Shift</kbd>+<kbd>m</kbd> | Open TikTok Inbox | Tap bottom tab (`rx=0.70`, `ry=0.94`) |
| <kbd>r</kbd> | Refresh feed (Pull-to-refresh) | Long downward swipe (`ry: 0.20 → 0.82`) |
| <kbd>a</kbd> | Home Feed tab | Tap bottom left navigation bar (`rx=0.10`, `ry=0.94`) |
| <kbd>n</kbd> | Expand notification panel | Android notification panel |
| <kbd>Shift</kbd>+<kbd>n</kbd> | Collapse panels | Android collapse panels |
| <kbd>u</kbd> | Expand quick settings | Android settings panel |
| <kbd>Backspace</kbd> / <kbd>Esc</kbd> | Dismiss panel & Android BACK | Collapse panels + BACK key |
| <kbd>h</kbd> | Android HOME | Android HOME key |
| <kbd>Tab</kbd> | Android App Switcher | Android APP_SWITCH key |
| <kbd>PageUp</kbd> | Volume UP | Android VOLUME_UP key |
| <kbd>PageDown</kbd> | Volume DOWN | Android VOLUME_DOWN key |
| <kbd>v</kbd> | Native screenshot | Android SYSRQ key |

