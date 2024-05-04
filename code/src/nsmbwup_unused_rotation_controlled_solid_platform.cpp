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


// Patches for Unused Rotation-Controlled Solid Platform
// (daEnLiftRotHalf_c): sprite 107, profile 481 (EN_LIFT_ROTATION_HALF).


#ifndef NSMBWUP_C00000_OFF

// The actor misuses the rotation controller's "starting rotation"
// spritedata field.

// More information on this type of bug can be found in
// nsmbwup_rotation_controlled_actors_starting_rotation.h.

kmWrite32(0x80a5d980, 0x38a0c000);  // li r5, -0x4000

#endif  // !NSMBWUP_C00000_OFF
