# Building NSMBW-Updated

This page explains how to get a NSMBW-Updated Riivolution patch.


## Release builds?

No release builds are provided. You need to build a Riivolution patch yourself by following the instructions below.

This is because most of the files in the Riivolution patch are just slightly edited copies of NSMBW's files, and are therefore still copyrighted by Nintendo and can't be redistributed. The build process generates these files from your own copy of the game.

For the same reason, please don't redistribute NSMBW-Updated Riivolution patches yourself.


## Overview

Prerequisites:

* A recent version of Python 3
* [Kamek](https://github.com/Treeki/Kamek) and the appropriate version of CodeWarrior for it (follow the download/setup instructions included with Kamek)
* [Ninja](https://ninja-build.org)
* If you're on a non-Windows system, [Wine](https://www.winehq.org/) is needed to run CodeWarrior.

The high-level workflow looks like this:

0. Build Kamek and get a copy of CodeWarrior, if you haven't already. (See Kamek's documentation for details.)
1. Run configure.py to generate build.ninja.
2. Run `ninja` to build a NSMBW-Updated Riivolution patch.

configure.py's arguments logically fall into two categories: ones that describe the environment ("where is Kamek"), and ones that configure NSMBW-Updated itself ("disable the patch for bug C00100").


## Environment configuration options

Required environment-configuration arguments include:

* `--kamek KAMEK`: the Kamek folder, containing the Kamek binary ("Kamek.exe" or "Kamek") and the k_stdlib directory
* `--cw MWCCEPPC`: the CodeWarrior folder, containing mwcceppc.exe, mwasmeppc.exe, and license.dat, at minimum

In addition, you should provide the paths to one or more extracted NSMBW root folders (a folder with "Effect", "Env", "HomeButton2", etc. subdirectories) with `--game-root DIR`. While you *can* build NSMBW-Updated without any, it's not recommended, because only the code patches can be built with this configuration. Providing at least one game root folder is required to build the patches for asset files, such as levels and tilesets.

Providing one game root is enough to get most of the asset patches, but if you have multiple versions of the game, providing more will allow additional version-specific assets (such as credits/staffroll files) to be patched. You can do this by adding more `--game-root DIR` arguments to configure.py (order doesn't matter).


## Bug selection options

configure.py also has arguments to control which bugfixes are built:

* By default, all bugfixes that don't match any of the more specific arguments below are enabled. You can change that with `--default off`, so that only bugfixes you explicitly enable by tag (explained below) or ID will be built.
* You can override the default for all bugs that have a particular tag with `--tag-default`. For example, you can disable the patches for all "fun" bugs with `--tag-default F off`.
    * The order matters: the last `--tag-default` argument that matches a given bug takes precedence. For example, if you specify `--tag-default B off` `--tag-default F on` in that order, any bugs that have *both* of those tags will be *enabled*.
* You can override all of the above and directly enable or disable an individual bugfix with the `--bug` argument (example: `--bug C00600 off`). If a bugfix has more options than just "on" or "off", this is also how you can select between those.


## Building with Ninja

After running configure.py, run `ninja` in the same directory to build a NSMBW-Updated Riivolution patch in the `build` folder.
