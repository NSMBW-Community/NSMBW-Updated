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


// Patches for Falling Icicle (daEnIcicle_c): sprite 265, profile 339
// (EN_ICICLE).


// This actor has two dCc_c initialization structs, one for each size
// (1x1, 1x2). Two bugfix patches both flip a bit in the third byte of
// the initialization structs' attack bitfields, so since "kmWrite1()"
// doesn't exist, we have to take handle these two patches together.

// C00800:
// The actor can be killed by a Propeller Suit spin-drill.
// More information on this type of bug can be found here:
#include "nsmbwup_propeller_suit_drillable_actors.h"

// C00504:
// The actor can be eaten by Yoshi.
// More information on this type of bug can be found here:
#include "nsmbwup_yoshi_edible_actors.h"

// Note that for C00504, we only patch the attack bitfield if the
// "passthrough" patch option is chosen -- for the "iceball" option, we
// keep that bit set and change the behavior through a completely
// different mechanism (see further below).

// Determine what byte value to patch into the bitfields, if any
#if defined(C00800) && defined(C00504_PASSTHROUGH)
#define FALLING_ICICLE_ATTACK_BITFIELD_BYTE2 0x5f
#elif defined(C00800)
#define FALLING_ICICLE_ATTACK_BITFIELD_BYTE2 0xdf
#elif defined(C00504_PASSTHROUGH)
#define FALLING_ICICLE_ATTACK_BITFIELD_BYTE2 0x7f
#endif

// Patch it in
#ifdef FALLING_ICICLE_ATTACK_BITFIELD_BYTE2
kmWrite8(0x80ad0eba, FALLING_ICICLE_ATTACK_BITFIELD_BYTE2);  // for 1x1 size
kmWrite8(0x80ad0ede, FALLING_ICICLE_ATTACK_BITFIELD_BYTE2);  // for 1x2 size
#endif  // FALLING_ICICLE_ATTACK_BITFIELD_BYTE2


#ifdef C00504_ICEBALL
// Patch field 0x36d to 5, causing it to become an iceball when eaten
kmBranchDefAsm(0x80a20b48, 0x80a20b4c) {
    nofralloc
    stfs f0, 0x31c(r28)
    li r4, 5
    stb r4, 0x36d(r28)
    blr
};
#endif  // C00504_ICEBALL
