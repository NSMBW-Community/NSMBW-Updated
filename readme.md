# NSMBW-Updated

A collection of unofficial bugfixes for *New Super Mario Bros. Wii.*


## License

MIT license. See license.txt for details.


## Release builds?

No release builds are provided. You need to build a Riivolution patch yourself by following the instructions below.

This is because most of the files in the Riivolution patch are just slightly edited copies of NSMBW's files, and are therefore still copyrighted by Nintendo and can't be redistributed. The build process generates these files from your own copy of the game.

For the same reason, please don't redistribute NSMBW-Updated Riivolution patches yourself.


## Build instructions

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

### Environment configuration options

Required environment-configuration arguments include:

* `--kamek-root KAMEK_ROOT`: the Kamek directory (with "k_stdlib", "Kamek", etc. subdirectories)
* `--cw MWCCEPPC`: CodeWarrior's "mwcceppc.exe"
* `--loader-root DIR`: the Kamek loader folder (should contain "kamekLoader.cpp" and "nsmbw.cpp")

In addition, you should provide the paths to one or more extracted NSMBW root folders (a folder with "Effect", "Env", "HomeButton2", etc. subdirectories) with `--game-root DIR`. While you *can* build NSMBW-Updated without any, it's not recommended, because only the code patches can be built with this configuration. Providing at least one game root folder is required to build the patches for asset files, such as levels and tilesets.

Providing one game root is enough to get most of the asset patches, but if you have multiple versions of the game, providing more will allow additional version-specific assets (such as credits/staffroll files) to be patched. You can do this by adding more `--game-root DIR` arguments to configure.py (order doesn't matter).

### Bug selection options

configure.py also has arguments to control which bugfixes are built:

* By default, all bugfixes that don't match any of the more specific arguments below are enabled. You can change that with `--default off`, so that only bugfixes you explicitly enable by tag (explained below) or ID will be built.
* You can override the default for all bugs that have a particular tag with `--tag-default`. For example, you can disable the patches for all "fun" bugs with `--tag-default F off`.
    * The order matters: the last `--tag-default` argument that matches a given bug takes precedence. For example, if you specify `--tag-default B off` `--tag-default F on` in that order, any bugs that have *both* of those tags will be *enabled*.
* You can override all of the above and directly enable or disable an individual bugfix with the `--bug` argument (example: `--bug C00600 off`). If a bugfix has more options than just "on" or "off", this is also how you can select between those.

### Building with Ninja

After running configure.py, run `ninja` in the same directory to build a NSMBW-Updated Riivolution patch in the `build` folder.


## Todo

See [todo.md](todo.md).


## Bug organization

Information about fixed bugs is kept in `db.txt` in the root folder, and also rendered as a nice table at the bottom of this readme.

**The fix for each bug will always include its bug ID right next to it in the source code, so that you can easily search for it.** Source code comments will also include a more in-depth explanation of the bug, its cause, and how the fix works.

### IDs

Each bug is assigned a 6-character ID (these are permanent and won't be changed over time). The IDs use the following format:

* The first character is what part of the game the bug is in. This is loosely based on the game's folder structure:

    Char | Meaning | Char | Meaning | Char | Meaning
    ---- | ------- | ---- | ------- | ---- | -------
    **C** | Code (DOL and/or RELs) | **N**| Env | **S** | Stage
    **E** | Effect | **O**| Object | **T** | Tilesets
    **L** | Layout | **P**| The region-specific folders (EU, US, JP, KR, TW, CN) | **U** | Sound
    **M** | MovieDemo | **R**| Replay | **W** | WorldMap

* The next 3 digits represent a bug class (for example, "actors that Yoshi shouldn't be able to eat, but can"). These are just assigned sequentially.
* The last two digits indicate a specific instance of the bug class (for example, "Yoshi shouldn't be able to eat Falling Icicles, but can"), and are also assigned sequentially.

The digits are split into two groups like that so that related bugs will sort together numerically even if they're discovered at different times.

### Tags

Each bug may also be marked with any combination of the following modifier tags, letting you enable or disable them as a group with configure.py `--tag-default` arguments:

Tag | Meaning
--- | ------
**B** | The fix for the bug may not be backward-compatible with existing levels.
**E** | This isn't a real bug; the fix is actually more of an *enhancement*. (These will be added somewhat sparingly.)
**F** | The bug is actually harmless and kind of fun to play around with.
**M** | The bug can only be encountered through further game modifications (for example, buggy interactions between a pair of actors that never appear together in any retail level). Note that emulator enhancements such as HD internal resolution don't count as "game modifications."
**S** | It's subjective / debatable whether this is actually a bug.

### Versions

Some bugs were fixed (or very rarely, introduced) by Nintendo in later versions of the game. When applicable, this is documented as well. Bugfixes won't be applied to versions where they're known to already be fixed.

### Options

The fixes for most bugs can just be turned on or off, but a few have more options to pick from. The exact set of available options can vary depending on the bug. The first listed option is the default.

## Full list of fixed bugs

ID | Description | Tags | Versions | Options
-- | ----------- | ---- | -------- | -------
**C00000** | Unused Rotation-Controlled Solid Platforms (sprite 107) ignore the starting-rotation setting | B, M | | on, off
**C00001** | Rotation-Controlled Event Deactivation Blocks (sprite 252) double the starting-rotation setting | B, M | | on, off
**C00002** | Rotation-Controlled Coins (sprite 253) double the starting-rotation setting | B, M | | on, off
**C00003** | Rotation-Controlled ? Blocks (sprite 255) and Rotation-Controlled Brick Blocks (sprite 256) double the starting-rotation setting | B, M | | on, off
**C00100** | The game crashes if Yoshi takes damage while eating a fruit | | Fixed in: K, W, C | on, off
**C00200** | Upside-down switches are moved 1/16 of a tile to the left | | | on, off
**C00300** | The first event-activated camera profile in a level cannot be the first one activated | M | | on, off
**C00400** | Dynamically generated lightmaps use "nearest neighbor" filtering, causing pixelation when internal resolution is increased | E, S | | on, off
**C00500** | Yoshi is able to swallow Special Exit Controllers (sprite 179) | M | | on, off
**C00501** | Yoshi is able to swallow Giant Falling Icicles (sprite 311) | M | | on, off
**C00502** | Yoshi is able to swallow Giant Floating Logs (sprite 173) | M | | on, off
**C00503** | Yoshi is able to swallow Toad House Chests (sprite 203) | M | | on, off
**C00504** | Yoshi is able to swallow Falling Icicles (sprite 265) | M | | passthrough, iceball, off
**C00600** | Save files are region-locked | E | | on, off
**C00700** | The Bowser Boss Door (sprite 452) is not considered a "wide" door for the purposes of fine-tuning player walking animations | | | on, off
**C00800** | Falling Icicles (sprite 265) can be killed with a Propeller Suit spin-drill | F | | on, off
**C00801** | Boo Circle Boos (spawned by sprite 323) can be killed with a Propeller Suit spin-drill | F | | on, off
**C00900** | Bushes (sprite 387) with sizes larger than the default ("small") spawn too late when approached from above | M | | on, off
**C01000** | The unused Jumbo Ray Respawner 2 deletes all of its existing Jumbo Rays whenever it spawns new ones | M | | on, off
**P00000** | Voice actress Caety Sagoian's name is misspelled as "Catey Sagoian" in the credits | | Fixed in: K, W | on, off
**P00100** | The "SOUND EFFECTS" section is mis-titled as "SOUND EFFECT" in the credits | | Only in: C | on, off
