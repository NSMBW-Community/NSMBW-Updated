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


// Patches for save-data-related code.


#ifndef NSMBWUP_C00600_OFF

// The first four bytes of the save data file, wiimj2d.sav, are the game
// ID -- for example, "SMNP" for PAL NSMBW, or "SMNE" for US NSMBW.
// This is checked when the file is loaded. Unfortunately, this ties
// savegames to specific regions, even though in practice there are no
// other differences between different regions' savegames.

// The bytes are checked one by one in dNandThread_c::load(). This patch
// nops out the check for the fourth byte, so that files from any
// region will pass the validation.

kmWrite32(0x800cf8a4, 0x60000000);  // nop
kmWrite32(0x800cf8a8, 0x60000000);  // nop
kmWrite32(0x800cf8ac, 0x60000000);  // nop
kmWrite32(0x800cf8b0, 0x60000000);  // nop
kmWrite32(0x800cf8b4, 0x60000000);  // nop

#endif  // !NSMBWUP_C00600_OFF
