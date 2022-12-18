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


// This is a configuration file where you can adjust preprocessor
// variables change compilation behavior. It may be more convenient for
// you than adding more CLI flags to your CodeWarrior invocations.

// Here are some examples:

// To deactivate the fix for a particular bug:

// #define NSMBWUP_C00900_OFF

// To deactivate the fix for a particular bug, but only for a certain
// game version:

// #if !defined(IS_GAME_VERSION_E2_COMPATIBLE)
// #define NSMBWUP_C00900_OFF
// #endif

// In this case, you'd also need to add a JSON config file for the cpp
// file containing that particular bugfix, to indicate that it should be
// compiled separately for different versions (once for E2, and once for
// everything else).
