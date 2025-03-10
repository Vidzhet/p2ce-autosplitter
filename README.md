# p2ce-autosplitter **[> Installation](#Installation)**
### p2ce-autosplitter is internal cheat-like program for strata source portal mods.

## Since portal revolution has updated, p2ce-autosplitter is broken. Ill rewrite many things to make it be compatible with any p2ce mod. Thank you for visiting this page.

## **Currently supports:**
- **Portal Revolution**

## **Starting a Speedrun:**
**You can use *Start Speedrun* button in main menu or *Restart Speedrun* in pause menu. You can also just type "reset" command in console.**
**Please note that it requires "wakeup_save.sav" save to work properly. You can find it on speedrun.com game resources page.**

## Features:
- ### **Auto-Recording demos to separate run folder**
- ### **Auto-Splitting using local tcp server**
- ### **5 New console commands:**
**reset** - start/restart speedrun
|-|
**alloc_console** - enable debug information
**cheats_toggle** - toggle sv_cheats 1 0
**stop_speedrun** - terminate current run
**democontrol** - toggle new demoui
- ### **Brand new DemoControl ui replacement for obsolete demoui!**
| ![img](images/democontrol_preview.png) |
|-|
**DemoControl also includes list feature!**
**You can open a folder which contents demos and iterating on them without worry!**
| ![img](images/democontrol_list_preview.png) |
**"List" supports auto-switching to next demo once its done playing aswell**

### Usage:
**To toggle open the ui, you can press *RightShift* or use default *Shift+F2*.**
**To open a folder - go to *List* menu and click *Open*. Then you can iterate on loaded demos by double clicking on them or by using *Select***
- ### **Game ui changes:**
| ![img](images/mainmenu_ui_preview.png) | ![img](images/mainmenu_ui_preview2.png) |
|-|-|

## Setting up LiveSplit:
**Open your LiveSplit and right click on it, Control -> Start TCP Server**
|-|
| ![img](images/livesplit_setup.png) |
**Make sure to set port to "16834" in LiveSplit settings. This value is used by the default.**

**Then go back to game and you will see *LiveSplit Connected* in upper right corner if it does. Reconnect without restarting the game is supported.**
## Installation:
**Download and run Installer.exe from [Releases](https://github.com/Vidzhet/p2ce-autosplitter/releases). Or download auto-launcher.dll with game executable(Manual installation.zip) and manually dragndrop it to your \bin\win64 with replacement**

**IMPORTANT: if you experiencing "updated" popup every game launch, thats meaning the auto-launcher was updated. So because it can not update itself, please reinstall auto-launcher.dll using Installer/manual installation or just click [Here](https://github.com/Vidzhet/p2ce-autosplitter/raw/refs/heads/master/update/auto-launcher.dll).**

### Contact me: @vidzhet (discord)
