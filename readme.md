# NSMBW-Updated

A collection of unofficial bugfixes for *New Super Mario Bros. Wii.*

*Note: This project doesn't include anything from [Newer Super Mario Bros. Wii](https://newerteam.com/wii/), other than a small number of bugfixes it added. If you want to use Newer's extra features in NSMBW, go check out [NSMBWer](https://github.com/Danster64/NSMBWer).*


## License

MIT license. See license.txt for details.


## Build Instructions

None yet. This is still in early development, please be patient!


## Todo

See [todo.md](todo.md).


## Bug Organization

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

Each bug can also be marked with any combination of the following modifier tags:

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

## Full List of Fixed Bugs

ID | Description | Tags | Versions | Options
-- | ----------- | ---- | -------- | -------
**C00000** | Unused Rotation-Controlled Solid Platforms (sprite 107) ignore the starting-rotation setting | B, M | | on, off
**C00001** | Rotation-Controlled Event Deactivation Blocks (sprite 252) double the starting-rotation setting | B, M | | on, off
**C00002** | Rotation-Controlled Coins (sprite 253) double the starting-rotation setting | B, M | | on, off
**C00003** | Rotation-Controlled ? Blocks (sprite 255) and Rotation-Controlled Brick Blocks (sprite 256) double the starting-rotation setting | B, M | | on, off
**C00100** | The game crashes if Yoshi takes damage while eating a fruit | | Fixed in: C, K, W | on, off
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
**P00000** | Voice actress Caety Sagoian's name is misspelled as "Catey Sagoian" in the credits | | Fixed in: K, W | on, off
**P00100** | The "SOUND EFFECTS" section is mis-titled as "SOUND EFFECT" in the credits | | Only in: C | on, off
