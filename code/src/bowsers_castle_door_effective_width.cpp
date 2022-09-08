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


// Thanks to Grop for this patch.

// dAcPy_c::exeDemoOutDoor_OpenDoor() is responsible for deciding if
// Mario is horizontally "close enough" to the center of a door to be
// able to enter it without having to walk towards the center first.
// dAcPy_c::setDoorDemo() selects the maximum allowed distance, and the
// value it picks depends on the type of door, determined by checking
// the door's profile ID: tower (EN_TORIDEDOOR) and castle
// (EN_CASTLEDOOR) doors allow 10 units (1 unit = 1/16 tile), and all
// others use 8.

// This makes sense because those doors are wider than others... but it
// seems the developers forgot about the Bowser's Castle boss room door
// (EN_KOOPADOOR), which is the widest door in the game.

// This patch adjusts the condition to include EN_KOOPADOOR in the "wide
// door types" case. Note that both the code being patched and the patch
// itself rely on the fact that these three profile IDs are sequential.

kmWrite32(0x8013f41c, 0x28000002);  // cmplwi r0, 2
