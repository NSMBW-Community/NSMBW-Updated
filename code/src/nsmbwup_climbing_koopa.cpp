// MIT License

// Copyright (c) 2026 Meatball132

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


// Patches for:
// - Climbing Koopa - Horizontal (daEnNetNokoLR_c): sprite 125, profile 170 (EN_NET_NOKONOKO_LR)
// - Climbing Koopa - Vertical (daEnNetNokoUD_c): sprite 126, profile 171 (EN_NET_NOKONOKO_UD)


#ifndef NSMBWUP_C01700_OFF

// Climbing Koopas, when a flip panel is turning, change to the
// state daNetEnemy_c::StateID_NetWait, or if "attached" to the panel,
// to daNetEnemy_c::StateID_NetMove. If, however, a Climbing Koopa is
// frozen when this happens (i.e. if a player freezes it and flips the
// panel immediately), it's forced out of the state dEn_c::StateID_Ice,
// the state which handles what an enemy actor should do while encased
// in ice, inherited by most enemies.

// Once frozen, the Climbing Koopa is no longer climbing its fence, and
// falls to the ground along with the ice (daIce_c), with which it
// shares an "ice manager" (dIceMng_c). The ice, while it's falling,
// checks once per frame whether it's hit the ground fast enough to
// shatter. If it has, it spawns a coin and sets the ice manager's
// destroy mode to "break".

// The enemy is supposed to check the ice manager's destroy mode each
// frame. If it indicates the ice should be broken, the enemy should
// kill both the ice and enemy actors. However, this check happens in
// the code for the Ice state. Therefore, the Climbing Koopa,
// blissfully unaware that it should be frozen (you can spot it
// climbing in place behind the ice, at this point), doesn't get the
// message that it should die. The ice actor, though, continues to
// recognize that it has fallen to the ground each frame, so it spawns
// another coin and marks the destroy mode again, ad infinitum.

// This quickly spawns enough coins to bring the Wii to its knees,
// slowing down the game dramatically. If the situation is left alone,
// enough coins can spawn to crash the game!

// The fix is simple: add an extra condition to the Climbing Koopa flip
// panel check. If its ice manager is active, the Climbing Koopa is in
// the Ice state, so the check should fail.

extern "C" void execute__12daNetEnemy_cFv__shouldFlip_checkFail();

kmBranchDefAsm(0x80044994, 0x80044998) {
    nofralloc
    lwz r0, 0x490(r3)      // r0 = this->mIceMng.mActive
    cmpwi r0, 0
    beq checkPass
    b execute__12daNetEnemy_cFv__shouldFlip_checkFail    
checkPass:
    lwzu r12, 0x0394 (r3)  // Restore original instruction
    blr
};

#endif  // !NSMBWUP_C01700_OFF
