// MIT License

// Copyright (c) 2024 RoadrunnerWMC

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

#ifndef _NSMBWUP_COMMON_H
#define _NSMBWUP_COMMON_H

#include <kamek.h>

#include "game_versions_nsmbw.h"


// Support code used in many patches.


#define _NSMBWUP_ASSEMBLE_BRANCH(src,dst) (0x48000000|(((dst)-(src))&0x3FFFFFC))


// Kamek-Ninja-Template includes its own game region/version constants,
// but I'm avoiding using them here to make it a little easier to embed
// portions of NSMBW-Updated into projects that don't use that template.
// (The IS_GAME_VERSION_DYNAMIC guards can be removed pretty easily.)

#define _NSMBWUP_GAME_REGION_P 0x10
#define _NSMBWUP_GAME_REGION_E 0x20
#define _NSMBWUP_GAME_REGION_J 0x30
#define _NSMBWUP_GAME_REGION_K 0x40
#define _NSMBWUP_GAME_REGION_W 0x50
#define _NSMBWUP_GAME_REGION_C 0x60
#define _NSMBWUP_GAME_REVISION_V1 0x01
#define _NSMBWUP_GAME_REVISION_V2 0x02
#define _NSMBWUP_GAME_REVISION_V3 0x03
#define _NSMBWUP_GAME_REVISION_K 0x04
#define _NSMBWUP_GAME_REVISION_W 0x05
#define _NSMBWUP_GAME_REVISION_C 0x06

#define _NSMBWUP_GET_GAME_REGION(x) ((x)&0xf0)
#define _NSMBWUP_GET_GAME_REVISION(x) ((x)&0x0f)


#ifdef IS_GAME_VERSION_DYNAMIC

/// Function to check the running game version at runtime.
/// Returns 0 if it's not recognized.
/// Doesn't check for P3/J3 (the Wii U eShop versions) -- these are
/// returned as P2/J2, respectively.
u32 _nsmbwup_check_game_version();

#endif  // IS_GAME_VERSION_DYNAMIC


#endif  // _NSMBWUP_COMMON_H
