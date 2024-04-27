// MIT License

// Copyright (c) 2022 RoadrunnerWMC

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <kamek.h>

#include "nsmbwup_user_config.h"


// Patches for Jumbo Ray Respawner 2 (daMantaMgr2_c): profile 526
// (MANTA_MGR2).


#ifndef NSMBWUP_C01000_OFF

// The used Jumbo Ray Respawner actor (MANTA_MGR) has two methods for
// deleting its child Rays:

// * 8085f570 (unofficially, "daMantaMgr_c::deleteAllChildren()")
//   deletes all of them. This is only called when the manager itself is
//   being deleted.
// * 8085f5c0 (unofficially,
//   "daMantaMgr_c::deleteAllChildrenInWaitState()") deletes only those
//   which are in the "Wait" state (not currently flying across the
//   screen). This is called whenever the manager spawns a new set of
//   Rays.

// The unused version, MANTA_MGR2, only has a copy (8085fbe0,
// unofficially "daMantaMgr2_c::deleteAllChildren()") of the first
// method, and calls it in both situations. This means that existing
// Rays disappear whenever the next ones are spawned.

// This is likely a bug, because this actor also has a setting (unique
// to it) to use a different spawn rate when at least one Ray is being
// ridden by a player. That feature wouldn't make sense if the delays
// were intended to be long enough for all Rays to go entirely
// off-screen before the next set could spawn, since that would force
// all players to dismount.

// This can be fixed by redirecting two of the three calls to 8085fbe0
// to daMantaMgr_c's second child-deletion method. Replacing a class
// method with one from a different, non-parent class isn't safe in
// general, but it works in this case because 8085f5c0 only accesses
// fields from the fBase_c class that all actors inherit. This seems
// like a better fix than copying the entire function into this file
// just for the conceptual purity of avoiding code-sharing between the
// two classes.

kmWrite32(0x80860070, 0x4bfff551);  // bl 0x8085f5c0
kmWrite32(0x808600d4, 0x4bfff4ed);  // bl 0x8085f5c0

#endif  // !NSMBWUP_C01000_OFF
