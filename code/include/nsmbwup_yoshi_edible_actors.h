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


// Many actors that should really be inedible but aren't used in the
// game's six Yoshi levels end up being edible anyway.

// Making an actor edible requires two conditions, and thus, there are
// generally two ways it can be fixed, though they're not exactly
// equivalent:

// First, it needs a dCc_c collision controller in order to detect the
// collision with Yoshi's tongue. The collision is only detected if the
// "YoshiEat" bit ((bitfield >> 15) & 1) in the dCc_c's initialization
// struct's "attack bitfield" is set. Otherwise, Yoshi's tongue will
// pass right through the collision box. This is the right place to
// patch if that's the behavior you want.

// Second, assuming the bit is set and a collision occurs, a u8 at 0x36d
// in dActor_c determines what happens next:

// 0 = Yoshi's tongue bounces off the collision box as though it were solid
// 1 = actor is held in mouth
// 2 = actor is swallowed (this is the default value (yes, really))
// 3 = (seems identical to 2?)
// 4 = actor is held in mouth, and becomes a fireball when spat out
// 5 = actor is held in mouth, and becomes an iceball when spat out
// (higher values seem identical to 1)

// This is the right place to patch if you want Yoshi's tongue to treat
// the collision as solid, or one of those other behaviors.
