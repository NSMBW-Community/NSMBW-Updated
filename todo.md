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
* There's some bug relating to Cooligan movement speed when they spawn in water vs enter it (see AnotherSMBW 3-3, when you exit the pipe after star coin 1)
* The unused ice blocks don't melt if you fire a fireball from right next to them
* The unused rolling barrel actor breaks into brick shards, and also uses a weird model compared to the normal barrels
* Bugged liquid movement options
* [Upside-down Spinies get very confused if you destroy the ceiling while they're frozen](https://www.suppermariobroth.com/post/707457546930290688)
* If you slide into a horizontal pipe, the particle effect isn't canceled ([video 1](https://cdn.discordapp.com/attachments/1072714148275822652/1076065027120975953/slide_effect_horiz_pipe.mp4), [video 2](https://cdn.discordapp.com/attachments/1072714148275822652/1076066171058655262/slide_effect_horiz_pipe_2.mp4))
* "If you groundpound a ? Block containing a star while other tiles are under it, the star comes out the top of the block but you can't collect it"
* Upside-down swinging platforms can be stood on. (Fix should be applied to the line-collider code.)
* Sprite ? blocks containing vines spawn mushrooms in multiplayer, because MP uses a different item-spawning function that doesn't include vines
* The famous chain-link fence + frozen Koopas = infinite coin glitch ([patch available here](https://github.com/Ryguy0777/nsmbw-utils/blob/main/sprites/Shyguys/heiho_net.S#L45))
* There's a gap between the flat and sloped collision parts of the purple screw mushroom platform, which can be clipped through
* Jellybeams in non-darkness zones make a giant white rectangle ([patch available here](https://github.com/MandyIGuess/NewerSMBW-Modding/tree/master/Bugfixes/Jellybeam-Lighting))
* [Sliding Penguin Mario can sometimes (?) collect coins behind a metal grate, while in front](https://cdn.discordapp.com/attachments/601539713169489932/1261394439369261260/coins.mp4?ex=66be4e1d&is=66bcfc9d&hm=d7d06a9da0f7c0fa00905240ea27ec84b419cfd948800fe9db7dc39efcb62e15&)
* A player being held by another player can collect coins behind a metal grate, while in front
* [Penguin Mario can slide through a Bramball's head without killing it or taking damage](https://cdn.discordapp.com/attachments/601539713169489932/1264082346102231150/nice.mp4?ex=66be322c&is=66bce0ac&hm=fc161f601a3820294695bd55c2b3f3779b87b47cbc4b2c56bd655e84b58dde5a&)
* [Entering a level while a map enemy is in the middle of an animation can have odd-looking effects](https://youtu.be/RVv5qj26dzs) ([ref. 2](https://cdn.discordapp.com/attachments/601539713169489932/1279606704803418204/2024-08-31_20-59-07.mp4?ex=66d50e1c&is=66d3bc9c&hm=d255ecd2456a83db431c49d6eac2f7639f2275a8a3b871f23a057a4a82dbffb3&))
* Lily Piranha Plants move the wrong direction when a water current controller is active
* Unlike other types of item blocks, a ? Block Containing Toad will respawn as a used block upon reloading the zone (even if it actually contained a coin instead of Toad)
* [Pipe Cannons use your last terminal velocity as your terminal velocity instead of the normal terminal velocity](https://cdn.discordapp.com/attachments/774479587701686283/1330696301192675339/2025-01-19_18-22-20.mp4?ex=678eeb01&is=678d9981&hm=8cab054c43ad9c3704af47d549187c5bb33b1d078a770f6390ad05d6f551b9e5&), so you fall slower if you propeller-flutter into the cannon, and faster if you propeller-drill into it
* Stretching mushrooms' spawn area rectangle is not tall enough, so they despawn if the camera is too high while they (should) be on-screen
* From RootCubed: "Nintendo forgot to hide the "world number" textbox [in the pause menu] when in a coin battle coin course. This makes it so that the default value (8) is still slightly visible behind the coin icon"

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
* [You can kill a brick-block-animation actor with a ground pound](https://cdn.discordapp.com/attachments/922243048501567488/1327499029231177788/Dolphin_5.0-21088___JIT64_SC___Vulkan___HLE___New_Super_Mario_Bros._Wii_SMNE01_2025-01-10_21-46-14.mp4?ex=67834950&is=6781f7d0&hm=11ec305d02b3d497cc6fe410cd541d650016f7102165fc1714dc35fec71d3992&) ([video 2](https://cdn.discordapp.com/attachments/1324890548724699138/1327535922765365248/glitch.mp4?ex=67836bac&is=67821a2c&hm=8ceea5a257bdbab4f7b3616b57e5d10a1dcee17b2787e1d0e1a01d18e2b6becc&))
* (Supper Mario Broth, 2025-03-03) "If Mario jumps onto a moving block under the influence of star power, he may become stuck in a somersaulting state. As long as no further inputs are made, Mario will keep somersaulting indefinitely, even after the star power wears off." ([video](https://bsky.app/profile/mariobrothblog.bsky.social/post/3ljigcc6ack2m))
* [Spin jump collision issue with stairs](https://cdn.discordapp.com/attachments/601539713169489932/1265515188254412842/2024-07-23_20-09-56-00.34.36.197-00.34.39.366.mp4?ex=66be229d&is=66bcd11d&hm=48e8e64630cd79c9cc051892f9d0d1e4bf96a2ddbc3dc912dbb27f72cf2cb504&)
* [It's possible to use Yoshi to clip through the goal flagpole](https://cdn.discordapp.com/attachments/601539713169489932/1273486269644669011/behindTheFlag.mp4?ex=66beca02&is=66bd7882&hm=73bd692d310ec02970ca61996f62566bed1abd9db306abec4b64de887c65d115&)
* Bug used in TASes: if a player grabs a powerup and another player **who comes later in the player order** bubbles on the same frame, they can skip the powerup time-stop
* (For the benefit of mods only) In multiplayer, if a player grabs the castle-boss key but dies before touching the ground, the game softlocks. (Unclear what the best fix for that would be...)

#### Hardest

* [Held Koopa shell angle glitch](https://twitter.com/Monster_TAS/status/1459527580499935237/video/1)
* [Koopa shell riding along the ceiling glitch](https://clips.twitch.tv/DaintyPunchyEndivePrimeMe-3m9vHEvJFrmKVQqa)
* [Stuff from this video (Japanese captions)](https://youtu.be/oH44Xs-PoEY)
* The bug that occurs when you bubble at the same moment you touch the flagpole
    * If all the other players are bubbled or dead when you do this, it's a softlock (game must be hard-reset to continue)
* The bug that occurs if you groundpound onto the flagpole at the end of Coin Battle 1
    * Grop notes that "Fixing this might be as easy as just setting the state of the player to Normal when the player touches the flagpole."
* [Mario running animation glitch](https://twitter.com/mariobrothblog/status/1496569843998507019)
* [Mushroom platform propeller flight collision issue](https://youtu.be/mgNl4yeJO1E)
* The animation bug that occurs if you just barely jump onto a platform which is moving downwards (easy to do in 5-1)
* Clipping through platforms in 5-5 (by the POW) and the 8-7 bonecoaster
    * The speedrunning Discord claims that bonecoaster clips always happen in the same spot
* A player in a "slow tumble" state can jump in mid-air ([video 1](https://cdn.discordapp.com/attachments/1072714148275822652/1078848331368898570/wall_throw_glitch.mp4), [video 2](https://cdn.discordapp.com/attachments/1072714148275822652/1078848510209830943/mid_air_jump_nsmbw.mp4))
* [Animation glitch when ground-pounded while on a Yoshi in mid-air](https://twitter.com/MarioBrothBlog/status/1612119090667077632)
* [If you get pushed towards the left edge of the screen, and then start sliding, you can die as if being crushed](https://cdn.discordapp.com/attachments/601539713169489932/1284065949044965478/nsmbw_5-5_death.mp4?ex=66e5471b&is=66e3f59b&hm=df4b199c6298de7d21d9339b683553d37e02a174b4ceae7f9e83b91062f50928&)
* [Clipping through tilt-controlled girders with a spin jump](https://cdn.discordapp.com/attachments/601539713169489932/1332406711302230067/2025-01-24_11-15-56-00.31.02.800-00.31.08.462.mp4?ex=679523f2&is=6793d272&hm=73560d28667ec185aa9467f6cd0b4dac5988bb991d9b88f2ae96af3b65aaf7c6&)

### Assets

#### Levels

Level fixes will use Papyrus patches, when that tool is ready. Until then, please hold off on fixing these issues.

* Tiling errors
    * B1 has a pretty comprehensive listing of them
    * Some pretty shoddy slope tiling near the red pipe in 3-4 A1Z1
    * 6-Ship has some ceiling tiles used as the floor at the very beginning
    * W6 and W8 airship battle rooms have a shadowed board in the top-right corner, which is a leftover from the W4 room the level designers copypasted
    * 1-6 underground room uses wrong Pa0
    * 3-4 A3 uses the wrong object for a right wall
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
* The smashed-pipe models have the opposite problem (too bright)

#### Music

* The "Fast" version of Snow has an extra "bah" at the very beginning

#### Backgrounds

* The sky in bgB_5302 cuts off in very wide zones because it's attached to a ScrollB bone that's the child of another ScrollB bone, making it move slower than it should. The fix is to rename the `ScrollB_sakyu` bone to `sakyu` in `bgB_5302_kumo_ue2_M`. (Credit to B1 Gaming)

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
