# Bug organization system

This page explains NSMBW-Updated's bug organization system.


## Sources of truth

Information about fixed bugs is kept in `db.txt` in the root folder. The table in the readme is automatically generated from it using `update_readme_table.py`, which should be run every time `db.txt` is modified.

**The fix for each bug will always include its bug ID right next to it in the source code, so that you can easily search for it.** Source code comments will also include a more in-depth explanation of the bug, its cause, and how the fix works.


## IDs

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


## Tags

Each bug may also be marked with any combination of the following modifier tags, letting you enable or disable them as a group with configure.py `--tag-default` arguments:

Tag | Meaning
--- | ------
**B** | The fix for the bug may not be backward-compatible with existing levels. It should be fine if you're creating new levels with the fix in mind, but otherwise, be careful.
**E** | This isn't a real bug; the fix is actually more of an *enhancement*. (These will be added somewhat sparingly.)
**F** | The bug is actually harmless and kind of fun to play around with.
**M** | The bug can only be encountered through further game modifications (for example, buggy interactions between a pair of actors that never appear together in any retail level). Note that emulator enhancements such as HD internal resolution don't count as "game modifications."
**S** | It's subjective / debatable whether this is actually a bug.


## Versions

Some bugs were fixed (or very rarely, introduced) by Nintendo in later versions of the game. When applicable, this is documented as well. Bugfixes won't be applied to versions where they're known to already be fixed.


## Options

The fixes for most bugs can just be turned on or off, but a few have more options to pick from. The exact set of available options can vary depending on the bug. The first listed option is always the default.
