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