// MIT License

// Copyright (c) 2024 RoadrunnerWMC

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


// Patches related to the unused Water Current Controller: sprite 243,
// profile 123 (TAG_WATER).


#ifndef NSMBWUP_C01500_OFF

// dAcPy_c::setPenWaterMoveSpeed() is the function that calculates the
// player's movement speed while swimming underwater with the Penguin
// Suit. Although it does make use of the water-current speed value from
// the global dWaterEntryMng_c instance, it does so using the following
// logic (local variable and struct-member names below are unofficial):

//     float some_local_variable = 0.0f;
//
//     if (dWaterEntryMng_c::m_instance->current != 0.0f) {
//         some_local_variable = 8.25f;
//     }

// This causes a rightward water-current force on Penguin Mario, with
// hardcoded strength, whenever there's any current at all, regardless
// of its actual speed or direction as configured in the
// water-current-controller settings.

// It's not clear *why* the function is written this way. It may just be
// a debugging/testing leftover that was never fixed up, since the
// water-current feature itself was ultimately abandoned.

// The corrected code is simply:

//     float some_local_variable = dWaterEntryMng_c::m_instance->current;

// which is done by the patch below.

// Implementation note: I chose to only patch the second byte of the
// instruction at 80131400, in order to change only the destination
// register without overwriting the immediate offset (0x8c0) with
// itself. This is slightly more future-proof in case there are ever any
// future game releases that change the offset.

// Another valid way to implement this patch would be to leave 80131400
// unchanged and replace 80131404 with fmr f0, f5 instead of with nop.

kmWrite8(0x80131401, 0xa3);         // "lfs f0, OFFSET(r3)" -> "lfs f5, OFFSET(r3)"
kmWrite32(0x80131404, 0x60000000);  // nop
kmWrite32(0x80131408, 0x60000000);  // nop
kmWrite32(0x8013140c, 0x60000000);  // nop

#endif  // !NSMBWUP_C01500_OFF
