# QTerminal for WSL

## Overview

This is a WSL port of [QTerminal](https://github.com/lxde/qterminal).

This project is licensed under the terms of the [GPLv2](https://www.gnu.org/licenses/gpl-2.0.en.html) or any later version. See the LICENSE file for the full text of the license.

[Screen shots](https://imgur.com/a/VMXp1)

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

## Installation

### Compiling sources

Dependencies:

 * [Qt](https://www.qt.io)
 * [lxqt-build-tools](https://github.com/kghost/lxqt-build-tools)
 * [QTermWidget for WSL](https://github.com/kghost/qtermwidget)
 * [utf8proc](https://github.com/kghost/utf8proc)
 * [tcppty](https://github.com/kghost/tcppty), a wsl backend act as a bridge.

### Binary packages

See [Releases](https://github.com/kghost/qterminal/releases)

## FAQ

### Why I am getting "The publisher could not be verified" warning ?

To avoid the message, the exe file must be properly signed, it will take about $200-$300 per year for the certification to sign the exe file. Currently I don't think it is worth to sign the file, at least at this stage. Also you are free to download the source code and compile it yourself.

### Where the user setting is stored ?

User setting is stored under per user roaming directory, eg: C:\\Users\\${username}\\AppData\\Roaming\\qterminal.org

### My Home/End key doesn't work.

Ensure that default kaytab is used (File -> Preferences -> Behavior -> Emulation), or try other key maps, even modify kaytab define files, located \share\qtermwidget5\kb-layouts\ inside program directory.

## Bug Report

Please use github [issue tracker](https://github.com/kghost/qterminal/issues)
