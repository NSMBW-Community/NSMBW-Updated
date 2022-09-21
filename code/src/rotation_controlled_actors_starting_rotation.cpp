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


// Fix actors that don't use the rotation controller's Starting Rotation
// value correctly.

// Some context: most rotation-controlled things inherit from
// daRotObjsBase_c and use its correctly-written functions for reading
// the rotation controller's info. But some handle it all manually
// themselves instead, and several don't read the starting-rotation
// value properly.

// All these actors have an "initial angle" field, representing their
// placement angle from the controller, calculated with atan2 upon load.
// The mistake they make is trying to also incorporate the controller's
// starting-rotation value into that, which is wrong because that's
// already included in the controller's rotation values themselves.

// daEnLiftRotHalf_c subtracts the value away, which results in that
// actor effectively ignoring starting-rotation. All the others add it
// instead, causing the starting-rotation to double.

// -0x4000 is used below as a replacement for instructions that read the
// rot. controller's starting-rotation value, because it's the
// difference between "up" (the default angle) and "right" (0 radians).
// This constant shows up frequently in Nintendo's own rotation-control
// code, too.

// ----

// Unused Rotation-Controlled Solid Platform, daEnLiftRotHalf_c
// (sprite 107, actor 481, EN_LIFT_ROTATION_HALF)
kmWrite32(0x80a5d980, 0x38a0c000);  // li r5, -0x4000

// ----

// Rotation-Controlled Event Deactivation Block, daEnObjRotationBlock_c
// (sprite 252, actor 529, EN_ROTATION_BLOCK)
kmWrite32(0x80a7b558, 0x3860c000);  // li r3, -0x4000
kmWrite32(0x80a7b570, 0x3860c000);  // li r3, -0x4000

// ----

// Rotation-Controlled Coin, daEnCoinAngle_c (sprite 253, actor 530, EN_COIN_ANGLE)
kmWrite32(0x809e55f4, 0x3860c000);  // li r3, -0x4000
kmWrite32(0x809e560c, 0x3860c000);  // li r3, -0x4000

// ----

// Rotation-Controlled ? and Brick Blocks, daEnBlockAngle_c
// (sprites 255 & 256, actors 532 & 533, EN_BLOCK_HATENA_ANGLE & EN_BLOCK_RENGA_ANGLE)
kmWrite32(0x809c15c4, 0x3860c000);  // li r3, -0x4000
kmWrite32(0x809c15dc, 0x3860c000);  // li r3, -0x4000
