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


// Patches for Bowser's Castle Boss Room Door (daEnKoopaDoor_c): sprite
// 452, profile 233 (EN_KOOPADOOR).


#ifndef NSMBWUP_C00700_OFF

// Thanks to Grop for this patch.

// When Mario enters a door, he decides if the door is "wide" or not,
// which affects some details of his animation.

// If Mario isn't "close enough" to the center of a normal door, he'll
// automatically walk to the center before entering it. If he is, the
// walking animation will be skipped, and he'll just slide toward the
// center while walking away from the camera instead.

// Doors considered wide have a slightly larger radius for the "close
// enough to the center" range (5/8 of a tile instead of 1/2), and they
// omit the sliding behavior. (These changes take place in
// dAcPy_c::exeDemoOutDoor_OpenDoor() and
// dAcPy_c::exeDemoOutDoor_MoveInter() respectively.)

// dAcPy_c::setDoorDemo() classifies a door into one of the
// two categories. It considers tower (EN_TORIDEDOOR) and castle
// (EN_CASTLEDOOR) doors wide, and all others normal. It seems, however,
// that the developers forgot to consider the Bowser's Castle boss room
// door, which is the widest door in the game.

// This patch adjusts the condition to include this door in that
// category. Note that both the code being patched and the patch itself
// rely on the fact that these three profile IDs are sequential.

kmWrite32(0x8013f41c, 0x28000002);  // cmplwi r0, 2

#endif  // !NSMBWUP_C00700_OFF
