# QTerminal for WSL

## Overview

This is a WSL port of [QTerminal](https://github.com/lxde/qterminal).

This project is licensed under the terms of the [GPLv2](https://www.gnu.org/licenses/gpl-2.0.en.html) or any later version. See the LICENSE file for the full text of the license.

[Screen shots](https://imgur.com/a/VMXp1)

## Download

See [Releases](https://github.com/kghost/qterminal/releases)

## Features

 * Multiple window
 * Multiple tab
 * Multiple panel (verticl/horizontal split)

XTerm features comapre to CMD or any CMD based terminal:

 * Bracketed paste: let the application know you are pasting not typing to prevent it doing something stupid.

![Paste](https://raw.githubusercontent.com/kghost/qterminal/assets/paste.gif)

 * Cursor styles: the application can change cursor styles (BLOCK/BAR/UNDERLINE)

![Cursor](https://raw.githubusercontent.com/kghost/qterminal/assets/cursor.gif)

### TODO

 * Multiple WSL instance support

## Build

Dependencies:

 * [Qt](https://www.qt.io)
 * [lxqt-build-tools](https://github.com/kghost/lxqt-build-tools)
 * [QTermWidget for WSL](https://github.com/kghost/qtermwidget)
 * [utf8proc](https://github.com/kghost/utf8proc)
 * [tcppty](https://github.com/kghost/tcppty), a wsl backend act as a bridge.

## FAQ

### Why I am getting "The publisher could not be verified" warning ?

To avoid the message, the exe file must be properly signed, it will take about $200-$300 per year for the certification to sign the exe file. Currently I don't think it is worth to sign the file, at least at this stage. Also you are free to download the source code and compile it yourself.

### Where the user setting is stored ?

User setting is stored under per user roaming directory, eg: C:\\Users\\${username}\\AppData\\Roaming\\qterminal.org

### My Home/End key doesn't work.

<<<<<<< HEAD
Ensure that default kaytab is used (File -> Preferences -> Behavior -> Emulation), or try other key maps, even modify kaytab define files, located \share\qtermwidget5\kb-layouts\ inside program directory.

### Can I run CMD inside QTerminal ?
=======
Dependencies are qtx11extras ≥ 5.2 and [QTermWidget](https://github.com/lxqt/qtermwidget).
In order to build CMake ≥ 3.0.2 and [lxqt-build-tools](https://github.com/lxqt/lxqt-build-tools) are needed as well as optionally Git to pull latest VCS checkouts. The localization files were outsourced to repository [lxqt-l10n](https://github.com/lxqt/lxqt-l10n) so the corresponding dependencies are needed, too. Please refer to this repository's `README.md` for further information.

Code configuration is handled by CMake. Building out of source is required. CMake variable `CMAKE_INSTALL_PREFIX` will normally have to be set to `/usr`.
>>>>>>> 0.9.0

No, you can't. You can try run CMD.exe under WSL, it kinda works. But I won't put any work on it.

## Bug Report

Please use github [issue tracker](https://github.com/kghost/qterminal/issues)
