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

#include "nsmbwup_user_config.h"


// Patches for Red Ring (daEnRedRing_c): sprite 156, profile 495
// (EN_REDRING).


#ifndef NSMBWUP_C01403_OFF

// If the actor is configured to activate an event, it accidentally
// cancels P-Switch music if any is playing when the event expires after
// a short time.

// More information on this type of bug can be found here:
#include "nsmbwup_timed_events_canceling_p_switch_music.h"

kmWrite32(0x80a93034, 0x39200001);  // li r9, 1

#endif  // NSMBWUP_C01403_OFF
