# Todo

Basically a gigantic list of bugs I'm aware of in NSMBW, which aren't fixed yet by patches in this repo.

## Bugs

Defined as "mistakes Nintendo made, which they most likely would've fixed had they been pointed out during development."

### Code

Difficulty ratings are my subjective guess. I haven't looked at the code behind any of these (or else they'd already be fixed!).

Despite being in the todo list, there's a good chance the "Harder" and "Hardest" bugs won't be fixed because they seem obscure and/or complicated at first glance. But I'm still documenting them for completeness. Who knows, maybe some are easier to fix than they look! I haven't checked the code for any of them, so it's possible.

#### Seem reasonably debuggable and fixable

##### Sprites & actors

* Hammer Bros (base class) spawn in the "Attack" state, but don't actually throw anything the first time
    * It may suffice to make them spawn in a different state
* `daIce_c::create` bug if an actor is frozen while in poison water
    * Grop investigated this in NHD and figured out the root cause
* Yoshi flutter-jump speed glitch: you retain horizontal speed if the d-pad is neutral
    * This would be classified as a "fun, non-critical" bug, and the patch for it will be optional and probably disabled by default
* It'd be nice if we could move flowers in front of layer 2
* Move-When-On platforms support different movement directions, but the texture apparently doesn't update correctly with those settings
* Some things (icicles, held Mecha-koopas, ...) can kill King Bills
* Banzai Bills have messed-up collision shapes ([video](https://cdn.discordapp.com/attachments/617856709423136799/982280695680483348/SMNP01_2022-06-03_15-51-07_1.mp4))
* The China 1-2 null-pointer crash
* Checkpoint Flag multiple-checkpoints multiplayer bug [(explained on TCRF)](https://tcrf.net/New_Super_Mario_Bros._Wii/Unused_Objects#Checkpoint_Flag)
* Port [Meatball's GAKENOKO fix](https://github.com/Meatball132/NSMBW_GAKENOKO_fix)
* Non-falling icicles can be inappropriately eaten by Yoshi, just like the falling ones can
* There's some bug relating to Cooligan movement speed when they spawn in water vs enter it (see AnotherSMBW 3-3, when you exit the pipe after star coin 1)
* The unused ice blocks don't melt if you fire a fireball from right next to them
* The unused rolling barrel actor breaks into brick shards, and alsos use a weird model compared to the normal barrels

##### Other things

* Backports from later versions of NSMBW
    * Savegame code fixes from NSMBW v2
    * A variety of other, undocumented code changes from even later versions
* Meatball claims that flying with a propeller suit plays two sfx, one positional and one global -- and they should really both be positional
* Fix the RNG function (this may necessitate a new tag, like "B" but just for replays)

#### Harder

* "When you hit a two-way line controlled block, the items coming out move relative to the block's position horizontally but not vertically" ([video](https://cdn.discordapp.com/attachments/708423907731832882/939947635790479411/yo_wmc.mp4))
* Autoscrollers with the "automatically begin" setting jerk to the player's position when they change powerup state
* Event-controlled spinning rotation controllers don't save/reload their state correctly when the player switches between zones
    * This affects my September 2021 contest level; [at the time, I added a non-general workaround that only works in that specific level](https://github.com/RoadrunnerWMC/RoadrunnerWMC_Levels/blob/3e5369e24f7c48b77e0c7e4324c4c5bec30e5b12/2021_09_NSMBW_Level_Contest/code/src/sprite_edits/rotation_controller_spinning.cpp)
* Reports that spinies in 6-2 "don't inflict damage if you're close enough to the ceiling"
* [You can clip through the moving platform in the W3 Boo House](https://cdn.discordapp.com/attachments/708423907731832882/1034717098322178088/2022-10-25_18-58-31_Trim.mp4)

#### Hardest

* [Held Koopa shell angle glitch](https://twitter.com/Monster_TAS/status/1459527580499935237/video/1)
* [Stuff from this video (Japanese captions)](https://youtu.be/oH44Xs-PoEY)
* The bug that occurs when you bubble at the same moment you touch the flagpole
    * If all the other players are bubbled or dead when you do this, it's a softlock (game must be hard-reset to continue)
* The bug that occurs if you groundpound onto the flagpole at the end of Coin Battle 1
    * Grop notes that "Fixing this might be as easy as just setting the state of the player to Normal when the player touches the flagpole."
* [Mario running animation glitch](https://twitter.com/mariobrothblog/status/1496569843998507019)
* [Mushroom platform propeller flight collision issue](https://youtu.be/mgNl4yeJO1E)
* The animation bug that occurs if you just barely jump onto a platform which is moving downwards (easy to do in 5-1)
* Clipping through platforms in 5-5 (by the POW) and the 8-7 bonecoaster

### Assets

#### Levels

Level fixes will use Papyrus patches, when that tool is ready. Until then, please hold off on fixing these issues.

* Tiling errors
    * B1 has a pretty comprehensive listing of them
    * Some pretty shoddy slope tiling near the red pipe in 3-4 A1Z1
    * 6-Ship has some ceiling tiles used as the floor at the very beginning
    * W6 and W8 airship battle rooms have a shadowed board in the top-right corner, which is a leftover from the W4 room the level designers copypasted
* Places where you can get below the level geometry, such as 9-1, and possibly the start of 4-Ship
* Double-check if any of the rotation-controller fixes would affect any retail levels, and patch their spritedata to compensate if so
* Inconsistency in the directions of the bowser-shell block objects in castle boss zones
* Things to comprehensively audit game-wide:
    * Wall shadows in levels that use Pa1_gake -- there are at least a couple places where they're tiled incorrectly
    * How POW blocks affect coins and star coins (definitely broken in, for example, 5-2 and 8-1)
        * I actually did this and made a spreadsheet
    * Places where multiplayer groundpound shakes the screen and reveals untiled places just outside of the zone

#### Tilesets

Similar to levels, these will use a not-yet-developed custom tool similar to Papyrus, which will be able to make precision edits to the original files instead of putting full .arc files in the repo.

##### Graphics

* Pa3_hanatari_saka: shading is noticeably different compared to the regular platforms
* Pa1_gake: the wall shadows are seriously messed up
    * This is fixed in the 2022 update for Another SMBW. The code used to generate the new shadows is already in this repo
* Tilesets with stray pixels:
    * Pa1_shiro_boss1: near the solid-on-top platforms
    * Pa1_koopa_out (according to B1)
    * Pa1_sabaku (ditto)
    * Pa1_sabaku_chika (ditto)
    * Pa1_shiro_sora: the first and 15th tiles in the fourth row
* Tilesets with missing randomization:
    * Pa1_gake_yougan
    * Pa1_korichika
    * Pa1_shiro_sora
* Pa1_shiro_sora: the top-left diagonal blue background-rocks object has the darkened bricks on the lower row of each tile, instead of the upper row

##### Objects

* Pa1_hikousen2: layout mistake in Bowser figurehead (object is unused in this version of the tileset, but we can still fix it)
* Pa2_sora: Fix repetition for the rainbow solid-on-top object

##### Collisions

* Pa1_gake:
    * Slopes are grassy for some reason
    * Some of the light-colored solid-on-top rock platforms have the floor collision on the wrong tiles
* Pa1_gake_yougan: some tiles that should be solid, aren't
* Pa1_toride_boss3: a large portion of the ground isn't actually icy
* [Pa1_toride_boss7: backport the improvement from NSMBW v2](https://tcrf.net/New_Super_Mario_Bros._Wii/Version_Differences#World_7-Tower_Boss_Battle_Tileset)
* Pa1_nohara: the solid-on-top platforms don't use the grassy terrain type
    * World 1-3 needs to be edited to be consistent with that change, too

#### World maps

* The W4 starting path node is inconsistent with those of every other world
* The W6 pipe joints use an outdated tileset texture
* The W7 cliffs use an outdated tileset texture

#### Actors

* The unused actors with darkened models: Loose Arrow, One-Way Gate, many of the test platforms, unused swinging castle platform? I think these might just be missing the placeholder lightmap materials.

#### Music

* The "Fast" version of Snow has an extra "bah" at the very beginning

## Enhancements

Defined as "improvements beyond what Nintendo had intended to do." This list just contains ideas -- not all will necessarily be implemented, as I really don't want the project scope to balloon too much!

* Make hardcoded behaviors optional
* Apply Pogo to all tilesets
* Automatic BRSAR patching at runtime
* Incorporate Ninji's China DRM patch
* Full-save anywhere? (definitely disable by default, if this is added)
* Disable eager tile-block allocation
* Collision renderer from Newer?
* Disable death / powerup-change lag
* Additional translations? (The hardest part would be fonts -- there's no Cryllic MarioFont as far as I know, for example)
    * I should make a separate repo with a Riivo patch that replaces every level with a (shared) dummy level file, and a set of savefiles that can be used to quickly trigger as many messages as possible (plus a document listing, for every translatable message, the fastest way to trigger it)
    * Maybe also make a dummy "translation" that just replaces every message with a unique number, and then use that to try to classify where some of the less-obvious messages are used