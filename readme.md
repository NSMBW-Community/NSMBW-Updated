# NSMBW-Updated

Just a collection of unofficial bugfixes for *New Super Mario Bros. Wii.* There isn't much here yet, but hopefully it'll gradually grow over time.

This project doesn't include anything from *Newer Super Mario Bros. Wii*, other than a small number of bugfixes it added. If you want to use Newer's extra features in NSMBW, go check out [NSMBWer](https://github.com/Danster64/NSMBWer).


## License

MIT license. See license.txt for details.


## Bugs Fixed

* Sprites 107, 252, 253, 255, and 256 now handle the rotation controller's "Starting Rotation" value properly.
* The game no longer crashes if Yoshi takes damage while eating a fruit, also known as the "Yoshi animation bug."
* Switches are no longer horizontally mispositioned when upside-down.
* The first camera profile in the level data can now be the first one to be activated. *(Note: Reggie secretly inserts a dummy camera profile into the level data in order to work around this bug for you, so you're unlikely to experience this bug even without the patch.)*


## Todo

* Port from Newer:
    * Connected pipes
    * Yoshi eating giant icicles (Newer 1.30)
* Backports from later game versions
    * Savegame stuff
    * Tileset collisions
    * Etc
* Deal with hardcodings in some way (make this an optional feature)
* Papyrus patches
    * Tiling errors
    * Places where you can get below the level geometry, e.g. 9-1, possibly the start of 4-Ship
    * Check if any of the rotation-controller fixes would affect any retail levels, and patch their spritedata to compensate if so
* If Bonk ever releases, hus can fix the W4 starting path node
* Propeller suit spin drill can kill...
    * Falling icicles
    * Boo circle boos
* Yoshi eating...
    * SECs
    * Giant icicles (port from Newer 1.30)
* Hammer Bros (base class) spawn in the "Attack" state, but don't actually throw anything the first time
    * Maybe making them spawn in a different state is sufficient?
* `daIce_c::create` bug if an actor is frozen while in poison water (investigated by Grop on Horizon)
