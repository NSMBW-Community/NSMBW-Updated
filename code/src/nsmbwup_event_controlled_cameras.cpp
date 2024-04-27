// MIT License

// Copyright (c) 2021 RoadrunnerWMC

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


// Patches for Event-Controlled Cameras, aka Camera Profiles.


#ifndef NSMBWUP_C00300_OFF

// When using camera mode 5 (event-controlled), the game only switches
// camera modes if the new camera-profile struct would have a different
// index from the current one. Otherwise it refuses to switch to the new
// profile, even if its event is activated.

// Unfortunately, it initializes the attribute that keeps track of the
// current camera profile index to 0 instead of -1. So if you try to
// activate the first profile first, it won't work because the game
// incorrectly thinks it's already using that one. You need to switch to
// the second or higher profile first, and only then can the first
// profile be used.

// Reggie works around this issue by inserting a dummy camera profile
// into the first slot, with the event ID set to 0 so it can never be
// activated. But it's still nice to have the bug, and fix, documented.

kmBranchDefAsm(0x80077290, 0x80077294) {
    nofralloc
    li r31, 0xff
    stb r31, 0x20(r6)
    li r31, 0
    blr
};

#endif  // !NSMBWUP_C00300_OFF
