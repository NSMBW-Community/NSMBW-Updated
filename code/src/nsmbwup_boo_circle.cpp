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


// Patches for:
// - Boo Circle (daRotarionGhostParent_c [sic]): sprite 323, profile 368 (AC_ROTATION_GHOST_PARENT)
// - Boo Circle Boo (daEnRotarionGhost_c [sic]): profile 369 (EN_ROTATION_GHOST)


#ifndef NSMBWUP_C00801_OFF

// daEnRotarionGhost_c can be killed by a Propeller Suit spin-drill.

// More information on this type of bug can be found in
// nsmbwup_propeller_suit_drillable_actors.h.

kmWrite8(0x80ad415a, 0x88);

#endif  // !NSMBWUP_C00801_OFF
