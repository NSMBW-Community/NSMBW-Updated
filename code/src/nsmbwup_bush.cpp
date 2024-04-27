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


// Patches for Bush (daObjFruitTree_c): sprite 387, profile 561
// (OBJ_FRUITTREE).


#ifndef NSMBWUP_C00900_OFF

// The Bush sprite just uses the default single-tile spawn range instead
// of configuring it to fit the actor's actual shape. This works fine at
// the default "small" size, but with any of the larger size settings,
// this can cause severe pop-in when approached from above.

// We fix this by simply increasing the spawn range vertically. The
// largest bush size is about 4 tiles (aka 64 units) taller than the
// default, so we increase it by that much.

kmWrite32(0x8030dfc8, 64);  // Move spawn range 4 tiles up
kmWrite32(0x8030dfd4, 64);  // Increase spawn range height by 4 tiles

#endif  // !NSMBWUP_C00900_OFF
