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


// Patches for Rotation-Controlled Coin (daEnCoinAngle_c): sprite 253,
// profile 530 (EN_COIN_ANGLE).


#ifndef NSMBWUP_C00002_OFF

// The actor misuses the rotation controller's "starting rotation"
// spritedata field.

// More information on this type of bug can be found in
// nsmbwup_rotation_controlled_actors_starting_rotation.h.

// TODO: This patch breaks the following retail stages:
// - 1-castle: the rotation controllers attached to the gears and coins
//   use a starting angle value of "1".
// - Coin-2: this stage uses lots of intentionally confusing
//   rotation-controlled blocks and coins.
// - 7-2, 8-1, 8-3: the location-triggered bonus coins that rotate in
//   from below the stage use a starting angle value of "4" to mean 180
//   degrees. One structure in 8-3 nonsensically uses starting_angle = 8
//   to mean 0 degrees. (That one should really be redone, since it's
//   the only one where the blocks and coins end up upside-down.)
// The stages should be modified (under this same bug ID) to compensate.

kmWrite32(0x809e55f4, 0x3860c000);  // li r3, -0x4000
kmWrite32(0x809e560c, 0x3860c000);  // li r3, -0x4000

#endif  // !NSMBWUP_C00002_OFF
