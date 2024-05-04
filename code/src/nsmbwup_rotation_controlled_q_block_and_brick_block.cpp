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


// Patches for Rotation-Controlled Blocks (daEnBlockAngle_c):
// - Rotation-Controlled ? Block: sprite 255, profile 532
//   (EN_BLOCK_HATENA_ANGLE)
// - Rotation-Controlled Brick Block: sprite 256, profile 533
//   (EN_BLOCK_RENGA_ANGLE)


#ifndef NSMBWUP_C00003_OFF

// The actor misuses the rotation controller's "starting rotation"
// spritedata field.

// More information on this type of bug can be found in
// nsmbwup_rotation_controlled_actors_starting_rotation.h.

// TODO: This patch breaks the following retail stages:
// - Coin-2: this stage uses lots of intentionally confusing
//   rotation-controlled blocks and coins.
// - 8-3: there's a pair of location-triggered brick blocks at the end
//   of the stage.
// The stages should be modified (under this same bug ID) to compensate.

kmWrite32(0x809c15c4, 0x3860c000);  // li r3, -0x4000
kmWrite32(0x809c15dc, 0x3860c000);  // li r3, -0x4000

#endif  // !NSMBWUP_C00003_OFF
