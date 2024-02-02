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


// dSwitchFlagMng_c is the class used to manage timed events for actors
// like switches and Red Rings. The function at 800e42b0 (unofficially
// "dSwitchFlagMng_c::set()", true name unknown at time of writing) is
// called by such actors to enable an event and optionally set a timeout
// for it.

// The last parameter to that function (r9) controls whether P-Switch
// music (specifically the background music, not the click-clack sounds
// that play on top of it and are shared with other sprites like Red
// Rings) will be canceled when the timeout expires. Since actors that
// aren't P-Switches don't play P-Switch music, they shouldn't be
// canceling it, either, so they universally set that argument to false.

// Unfortunately, the parameter's meaning is the opposite of what you'd
// probably expect: setting it to *true*, not false, disables the
// music-canceling behavior. As a result, several actors that activate
// timed events also cancel P-Switch music when their timeouts end, if
// any happens to be playing at that time.

// The fix is to simply change the argument from false to true for each
// affected call to the function.

// Note: the music-disabling behavior only happens if the timeout value
// (the second non-"this" parameter, r5) is nonzero. Since the majority
// of calls to the function have a timeout of 0, most of them cannot
// trigger this bug, regardless of what the other parameter is set to.
