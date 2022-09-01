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


// A 1-byte value at 0x36d in dActor_c determines whether Yoshi can eat
// the actor or not:

// 0 = no
// 1 = can be held in mouth
// 2 = can be swallowed (default)
// 3 = seems identical to 2
// 4 = Yoshi spits out a fireball
// 5 = Yoshi spits out an iceball
// (higher values seem identical to 1)

// Since "swallowable" is the default, many actors that should really be
// inedible but aren't used in the game's six Yoshi levels don't bother
// to change it. This isn't really a *bug* since it doesn't affect
// normal gameplay, but making actors behave correctly in more
// situations opens up more possibilities for custom level design.

// Sprites that edit the value do so in their create() method (see
// daEnPataMet_c, for example), so these patches work the same way.

// Special Exit Controller, daNextGotoBlock_c (sprite 179, actor 226, AC_NEXTGOTO_BLOCK)
kmBranchDefAsm(0x8086e794, 0x8086e798) {
    nofralloc
    stfs f31, 0x3dc(r3)
    li r7, 0
    stb r7, 0x36d(r3)
    blr
}

// Giant Icicle, daEnBigIcicle_c (sprite 311, actor 553, EN_BIG_ICICLE)
// (thanks to Meatball132 for this patch)
kmBranchDefAsm(0x809b4420, 0x809b4424) {
    nofralloc
    stfs f1, 0x318(r30)
    li r9, 0
    stb r9, 0x36d(r30)
    blr
};
