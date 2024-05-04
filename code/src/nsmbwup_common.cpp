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

#include <kamek.h>

#include "game_versions_nsmbw.h"
#include "nsmbwup_user_config.h"
#include "nsmbwup_common.h"


// Support code used in many patches.


#ifdef IS_GAME_VERSION_DYNAMIC

u32 _nsmbwup_check_game_version() {
    switch (*((u32*)0x800cf6cc))
    {
        case 0x40820030: return _NSMBWUP_GAME_REGION_P | _NSMBWUP_GAME_REVISION_V1;
        case 0x40820038: return _NSMBWUP_GAME_REGION_P | _NSMBWUP_GAME_REVISION_V2;
        case 0x48000465: return _NSMBWUP_GAME_REGION_E | _NSMBWUP_GAME_REVISION_V1;
        case 0x2c030000: return _NSMBWUP_GAME_REGION_E | _NSMBWUP_GAME_REVISION_V2;
        case 0x480000b4: return _NSMBWUP_GAME_REGION_J | _NSMBWUP_GAME_REVISION_V1;
        case 0x4082000c: return _NSMBWUP_GAME_REGION_J | _NSMBWUP_GAME_REVISION_V2;
        case 0x38a00001:
            switch (*((u8*)0x8000423a))
            {
                case 0xc8: return _NSMBWUP_GAME_REGION_K | _NSMBWUP_GAME_REVISION_K;
                case 0xac: return _NSMBWUP_GAME_REGION_W | _NSMBWUP_GAME_REVISION_W;
                default: return 0;
            }
            break;
        case 0x4182000c: return _NSMBWUP_GAME_REGION_C | _NSMBWUP_GAME_REVISION_C;
        default: return 0;
    }
}

#endif  // IS_GAME_VERSION_DYNAMIC
