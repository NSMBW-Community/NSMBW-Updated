// MIT License

// Copyright (c) 2026 RoadrunnerWMC

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


// Patches for Airship Nut Platform (daNut_c): sprite 295, profile 542
// (NUT).


#ifndef NSMBWUP_C01800_OFF

// This actor has some code to allow its size to be configured through
// spritedata, with nybbles 11 (height + 1) and 12 (width + 4). In the
// final game, the actor's model has a fixed size of 6x2, and its
// spritedata is always set accordingly (**** **** **12). The code to
// handle other sizes was thus left a bit unfinished.

// In particular, the "distance from center" field for the actor's
// sensor for detecting wall collisions is set to a hardcoded value of
// 0x30000 (3 tiles), which is only correct for 6-tile-wide platforms.
// This patch replaces that constant with a value calculated from the
// spritedata instead.

// Note that there are four different codepaths for setting up the
// sensor, depending on whether the height is set to 1, 2, 3, or 4
// tiles. (The height value is masked with & 1, so not only are the
// codepaths for heights 3 and 4 unused, they're also completely
// unreachable.) We thus apply this patch in four different places.

// Also note that this patch doesn't (yet) make the model change in size
// to reflect the spritedata settings. This feature may be added in the
// future.

// This replaces "lis r0, 3".
asm void daNut_c_calculate_sensor_width() {
    nofralloc
    lwz r0, 4(r30)             // r0 = this->m_param;
    rlwinm r0, r0, 15, 14, 16  // r0 = (r0 & 7) << 15;
    addis r0, r0, 2            // r0 += 0x20000;
    blr
}

kmCall(0x80873438, daNut_c_calculate_sensor_width);  // height = 1 tile
kmCall(0x80873498, daNut_c_calculate_sensor_width);  // height = 2 tiles
kmCall(0x8087350c, daNut_c_calculate_sensor_width);  // height = 3 tiles
kmCall(0x80873580, daNut_c_calculate_sensor_width);  // height = 4 tiles

#endif  // !NSMBWUP_C01800_OFF
