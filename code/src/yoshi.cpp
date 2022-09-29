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


// Patches for Yoshi (daYoshi_c): profile 14 (YOSHI).


#ifdef C00100

// Thanks to Ninji (Treeki) for this patch.

// If Yoshi takes damage while eating a fruit, the game tries to play
// animation "Run". The animation is actually called "Rrun" (two "r"s),
// so the game crashes.

// Nintendo fixed this in the Korean version of the game, and all later
// versions.

#ifdef IS_PRE_K
kmWritePointer(0x802f2a4c, "Rrun");
#endif  // IS_PRE_K

#endif  // C00100