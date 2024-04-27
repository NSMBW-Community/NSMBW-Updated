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


// Patches for Switch (daEnHnswich_c):
// - ? Switch: sprite 40, profile 72 (EN_HNSWICH [sic])
// - P Switch: sprite 41, profile 73 (EN_PSWICH [sic])
// - ! Switch: sprite 42, profile 74 (EN_QSWICH [sic])
// - Unused ? Switch: sprite 153, profile 498 (EN_BIG_HNSWICH [sic])
// - Small Bowser Switch: sprite 478, profile 75 (EN_BOSS_KOOPA_SWITCH)
// - Large Bowser Switch: sprite 479, profile 76 (EN_BOSS_KOOPA_BIG_SWITCH)


#ifndef NSMBWUP_C00200_OFF

// The game actively moves upside-down switches left by one unit (1/16
// of a tile) upon spawn. This doesn't appear to serve any useful
// purpose, and just makes them look slightly wrong.

// This behavior can also be found in NSMBDS. If it had some reason to
// exist in that game, it's probably unlikely (but admittedly not
// impossible) that it still does here.

kmWrite32(0x80a19a7c, 0x60000000);  // nop

#endif  // !NSMBWUP_C00200_OFF
