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
#include <k_sdk/types.h>

#include "game_versions_nsmbw.h"
#include "nsmbwup_common.h"
#include "nsmbwup_user_config.h"


// Patches for Tilt Lift (daLiftRemoconSeesaw_c): sprite 51, profile 460
// (AC_LIFT_REMOCON_SEESAW)


#ifndef NSMBWUP_C01900_OFF

// The Chinese release of NSMBW for the NVIDIA SHIELD TV runs under a
// Wii emulator developed by NVIDIA. One challenge they had for this
// version is the controls; the SHIELD TV controller doesn't support
// motion controls of any kind. Unlike Super Mario Galaxy, though,
// which bound the motion controls to buttons, NSMBW was modified to
// support the Classic Controller, so the emulator had a more
// conventional control scheme to configure the bindings for.

// The Tilt Lift is one of the motion-controlled mechanics that needed
// to be adapted to the Classic Controller. For some reason, NVIDIA
// added a check at runtime for whether to use the original motion
// controls or their new button-based controls, based on whether the
// Classic Controller is in use by the player currently using the Tilt
// Lift. This is despite each emulated controller always being
// configured to use the Classic Controller.

// However, if no players have activated the Tilt Lift, the value
// that keeps track of which player has claimed the Tilt Lift is set
// to -1. This value is what was used by NVIDIA to index into the array
// of controller data pointers, and an index of -1 causes an out-of-
// bounds access into the last value of a 3x4 identity matrix, which is
// always 0. This causes an invalid pointer dereference when NVIDIA's
// check attempts to read the controller's device type, and no players
// are standing on a Tilt Lift (which will happen the moment a Tilt
// Lift spawns).

// NVIDIA's emulator ignores this, so this issue cannot be observed
// normally. However, if you convert the ROM from the SHIELD version
// and play it in Dolphin Emulator, this bug causes an invalid read
// warning or a crash, depending on the CPU emulation settings. On real
// Wii hardware, it crashes.

// To fix this, we just fail NVIDIA's Classic Controller condition
// early if the active Tilt Lift player is -1.

#if IS_GAME_VERSION_DYNAMIC || (GAME_REVISION >= GAME_REVISION_C)

extern "C" void executeState_Move__21daLiftRemoconSeesaw_cFv__isValidPlayer_checkFail();
#ifdef IS_GAME_VERSION_DYNAMIC
static asm void executeState_Move_activePlayerCheck() {
#else
// Note that the addresses are mapped +0x10000000 to work around
// the inability to tell Kamek that a particular address is for a
// different base address than the rest of the addresses
kmBranchDefAsm(0x9084417c, 0x90844180) {
#endif  // IS_GAME_VERSION_DYNAMIC
    nofralloc
    extsb. r0, r0  // Restore original instruction, but with dot to update CR0
    bge checkPass
    // Check fails if r0 (this->mActivePlayer) is negative
    b executeState_Move__21daLiftRemoconSeesaw_cFv__isValidPlayer_checkFail
checkPass:
    blr
};

#ifdef IS_GAME_VERSION_DYNAMIC
class daLiftRemoconSeesaw_c;

kmBranchDefCpp(0x8083ee78, NULL, daLiftRemoconSeesaw_c*, daLiftRemoconSeesaw_c *this_) {
    u32 version = _nsmbwup_check_game_version();

    // Since the condition is >=, no need to check that it's also != 0
    if (_NSMBWUP_GET_GAME_REVISION(version) >= _NSMBWUP_GAME_REVISION_C) {
        u32 b_src = 0x8084417c;  // Correct for C
        u32 inst = _NSMBWUP_ASSEMBLE_BRANCH(
            b_src,
            (u32)&executeState_Move_activePlayerCheck
        );
        *((u32*)b_src) = inst;
    }

    return this_;
}
#endif  // IS_GAME_VERSION_DYNAMIC

#endif  // IS_GAME_VERSION_DYNAMIC || (GAME_REVISION >= GAME_REVISION_C)


#endif  // !NSMBWUP_C01900_OFF
