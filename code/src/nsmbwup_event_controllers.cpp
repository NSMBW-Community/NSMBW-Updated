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


// Patches for event controllers implemented by the daFlagObj_c class:
// - Zone Enter: sprite 33, profile 64 (AC_FLAGON)
// - "And": sprite 34, profile 65 (AC_4SWICHAND [sic])
// - "Or": sprite 35, profile 66 (AC_4SWICHOR [sic])
// - Random: sprite 36, profile 67 (AC_RANDSWICH [sic])
// - "Chainer": sprite 37, profile 68 (AC_CHNGESWICH [sic])
// - Location Check: sprite 38, profile 69 (AC_IFSWICH [sic])
// - "Multi-Chainer": sprite 39, profile 70 (AC_RNSWICH [sic])


// If a Zone Enter or Chainer is configured to activate a timed event,
// it accidentally cancels P-Switch music if any is playing when the
// time expires.

// More information on this type of bug can be found in
// nsmbwup_timed_events_canceling_p_switch_music.h.

#ifndef NSMBWUP_C01401_OFF
// Zone Enter:
kmWrite32(0x807eb210, 0x39200001);  // li r9, 1
kmWrite32(0x807eb248, 0x39200001);  // li r9, 1
#endif  // !NSMBWUP_C01401_OFF

#ifndef NSMBWUP_C01402_OFF
// Chainer:
kmWrite32(0x807ebaa0, 0x39200001);  // li r9, 1
kmWrite32(0x807ebac8, 0x39200001);  // li r9, 1
kmWrite32(0x807ebb28, 0x39200001);  // li r9, 1
kmWrite32(0x807ebb50, 0x39200001);  // li r9, 1
#endif  // !NSMBWUP_C01402_OFF
