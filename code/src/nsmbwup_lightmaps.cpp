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


// Patches for EGG's custom system for dynamically generated lightmaps.


#ifndef NSMBWUP_C00400_OFF

// Thanks to Skawo and Ninji for this patch.

// Dynamically generated lightmap materials are set to nearest-neighbor
// filtering rather than linear, resulting in pixelated-looking
// reflections on models when played in Dolphin with increased internal
// resolution.

// Here, we patch EGG::LightTexture::draw() to force the
// EGG::CpuTexture's minFilter and magFilter fields to 5
// (Linear_Mipmap_Linear) and 1 (Linear) respectively, just before
// they're drawn.

// A better solution would be to patch these values when they're
// initialized in EGG::LightTexture::initialize() (see P1: 802cbe5c).
// Unfortunately, that function runs before the Kamek loader, so that
// doesn't work with the current loader setup.

kmBranchDefAsm(0x802cc5f4, 0x802cc5f8) {
    nofralloc
    li r4, 5
    stb r4, 0xc(r3)
    li r4, 1
    stb r4, 0xd(r3)
    li r4, 0
    blr
};

#endif  // !NSMBWUP_C00400_OFF
