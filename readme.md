# NSMBW-Updated

A collection of unofficial bugfixes for *New Super Mario Bros. Wii.*

The goal is for this project to be all of the following:

* A standalone Riivolution patch that fixes as many mistakes in the base game as possible
* Detailed, high-quality documentation on those mistakes and how the fixes work
* A useful base code package that can be plugged into other mods of the game, replacing the previous status quo of perpetually copying the same few common patches from mod to mod


## Where to go next

[Click here for more information about how to get NSMBW-Updated as a standalone Riivolution patch.](readme_building.md)

[Click here for more information about how to include NSMBW-Updated's code in your own NSMBW mod project.](readme_embedding.md)

[Click here for more information about how bugs are organized in this project.](readme_bug_organization.md)

Keep scrolling down for the full list of fixed bugs.


## License

MIT license. See [license.txt](license.txt) for details.


## Todo

See [todo.md](todo.md).


## Full list of fixed bugs

[Explanations of IDs, tags, versions, and options](readme_bug_organization.md)

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
**C00505** | Yoshi is able to swallow Icicles (sprite 201) | M | | passthrough, iceball, off
**C00600** | Save files are region-locked | E | | on, off
**C00700** | The Bowser Boss Door (sprite 452) is not considered a "wide" door for the purposes of fine-tuning player walking animations | | | on, off
**C00800** | Falling Icicles (sprite 265) can be killed with a Propeller Suit spin-drill | F | | on, off
**C00801** | Boo Circle Boos (spawned by sprite 323) can be killed with a Propeller Suit spin-drill | F | | on, off
**C00802** | Icicles (sprite 201) can be killed with a Propeller Suit spin-drill | F | | on, off
**C00900** | Bushes (sprite 387) with sizes larger than the default ("small") spawn too late when approached from above | M | | on, off
**C01000** | The unused Jumbo Ray Respawner 2 deletes all of its existing Jumbo Rays whenever it spawns new ones | M | | on, off
**C01100** | "Direct Pipe" entrances use the wrong path nodes | M | | on, off
**C01200** | "Direct Pipe" entrances are exited in the direction of the entered pipe, rather than the exited pipe | B, M | | on, off
**C01300** | "Direct Pipe" entrances that should exit upward actually exit downward | B, M | | on, off
**C01400** | Star Coins (sprite 32) that activate timed events cancel P-Switch music when the time expires | M | | on, off
**C01401** | "Event Controller - Zone Enter"s (sprite 33) that activate timed events cancel P-Switch music when the time expires | M | | on, off
**C01402** | "Event Controller - Chainer"s (sprite 37) that activate timed events cancel P-Switch music when the time expires | M | | on, off
**C01403** | Red Rings (sprite 156) that activate events cancel P-Switch music when the event expires | M | | on, off
**C01500** | The unused Water Current Controller (sprite 243) applies a fixed rightward force to players with Penguin Suits, regardless of sprite settings | M | | on, off
**C01600** | The world map HUD animation for entering "View Map" mode is bugged | | Fixed in: K, W, C | on, off
**C01700** | Climbing Koopas (sprites 125 and 126) don't shatter and start spawning infinite coins if they're frozen when the player punches a flip panel. | F | | on, off
**P00000** | Voice actress Caety Sagoian's name is misspelled as "Catey Sagoian" in the credits | | Fixed in: K, W | on, off
**P00100** | The "SOUND EFFECTS" section is mis-titled as "SOUND EFFECT" in the credits | | Only in: C | on, off
