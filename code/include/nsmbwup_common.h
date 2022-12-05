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


// First, define constants for every game version, region, and revision.

// The numeric values are not guaranteed to be stable over time, but the
// revisions are guaranteed to be comparable in chronological order, so
// that you can do e.g. "NSMBWUP_REVISION_OF(x) < NSMBWUP_REVISION_K".

#define NSMBWUP_VERSION_P1 1
#define NSMBWUP_VERSION_E1 2
#define NSMBWUP_VERSION_J1 3
#define NSMBWUP_VERSION_P2 4
#define NSMBWUP_VERSION_E2 5
#define NSMBWUP_VERSION_J2 6
#define NSMBWUP_VERSION_K 7
#define NSMBWUP_VERSION_W 8
#define NSMBWUP_VERSION_C 9

#define NSMBWUP_REGION_P 1
#define NSMBWUP_REGION_E 2
#define NSMBWUP_REGION_J 3
#define NSMBWUP_REGION_K 4
#define NSMBWUP_REGION_W 5
#define NSMBWUP_REGION_C 6

#define NSMBWUP_REVISION_V1 1
#define NSMBWUP_REVISION_V2 2
// (space reserved for v3, the Wii U eShop version)
#define NSMBWUP_REVISION_K 4
#define NSMBWUP_REVISION_W 5
#define NSMBWUP_REVISION_C 6


// NSMBWUP_VERSION is the variable that'll indicate the game version
// we're compiling for. But since there's no safe way to set its value
// from the command line (I don't think
// "-DNSMBWUP_VERSION=NSMBWUP_VERSION_P1" would work), here are some
// helper macros you can use for that ("-DNSMBWUP_SET_VERSION_P1"):

#if defined(NSMBWUP_SET_VERSION_P1)
#define NSMBWUP_VERSION NSMBWUP_VERSION_P1
#undef NSMBWUP_SET_VERSION_P1
#elif defined(NSMBWUP_SET_VERSION_E1)
#define NSMBWUP_VERSION NSMBWUP_VERSION_E1
#undef NSMBWUP_SET_VERSION_E1
#elif defined(NSMBWUP_SET_VERSION_J1)
#define NSMBWUP_VERSION NSMBWUP_VERSION_J1
#undef NSMBWUP_SET_VERSION_J1
#elif defined(NSMBWUP_SET_VERSION_P2)
#define NSMBWUP_VERSION NSMBWUP_VERSION_P2
#undef NSMBWUP_SET_VERSION_P2
#elif defined(NSMBWUP_SET_VERSION_E2)
#define NSMBWUP_VERSION NSMBWUP_VERSION_E2
#undef NSMBWUP_SET_VERSION_E2
#elif defined(NSMBWUP_SET_VERSION_J2)
#define NSMBWUP_VERSION NSMBWUP_VERSION_J2
#undef NSMBWUP_SET_VERSION_J2
#elif defined(NSMBWUP_SET_VERSION_K)
#define NSMBWUP_VERSION NSMBWUP_VERSION_K
#undef NSMBWUP_SET_VERSION_K
#elif defined(NSMBWUP_SET_VERSION_W)
#define NSMBWUP_VERSION NSMBWUP_VERSION_W
#undef NSMBWUP_SET_VERSION_W
#elif defined(NSMBWUP_SET_VERSION_C)
#define NSMBWUP_VERSION NSMBWUP_VERSION_C
#undef NSMBWUP_SET_VERSION_C
#endif


// If no version has been selected yet, choose a default one.

#ifndef NSMBWUP_VERSION
#define NSMBWUP_VERSION NSMBWUP_VERSION_P1
#endif  // NSMBWUP_VERSION


// Finally, define some macros to check the region and revision of a
// particular game version.

#define NSMBWUP_REGION_OF(version) ( \
    (version) == NSMBWUP_VERSION_P1 ? NSMBWUP_REGION_P : \
    (version) == NSMBWUP_VERSION_E1 ? NSMBWUP_REGION_E : \
    (version) == NSMBWUP_VERSION_J1 ? NSMBWUP_REGION_J : \
    (version) == NSMBWUP_VERSION_P2 ? NSMBWUP_REGION_P : \
    (version) == NSMBWUP_VERSION_E2 ? NSMBWUP_REGION_E : \
    (version) == NSMBWUP_VERSION_J2 ? NSMBWUP_REGION_J : \
    (version) == NSMBWUP_VERSION_K ? NSMBWUP_REGION_K : \
    (version) == NSMBWUP_VERSION_W ? NSMBWUP_REGION_W : \
    (version) == NSMBWUP_VERSION_C ? NSMBWUP_REGION_C : \
    NSMBWUP_REGION_P)

#define NSMBWUP_REVISION_OF(version) ( \
    (version) == NSMBWUP_VERSION_P1 ? NSMBWUP_REVISION_V1 : \
    (version) == NSMBWUP_VERSION_E1 ? NSMBWUP_REVISION_V1 : \
    (version) == NSMBWUP_VERSION_J1 ? NSMBWUP_REVISION_V1 : \
    (version) == NSMBWUP_VERSION_P2 ? NSMBWUP_REVISION_V2 : \
    (version) == NSMBWUP_VERSION_E2 ? NSMBWUP_REVISION_V2 : \
    (version) == NSMBWUP_VERSION_J2 ? NSMBWUP_REVISION_V2 : \
    (version) == NSMBWUP_VERSION_K ? NSMBWUP_REVISION_K : \
    (version) == NSMBWUP_VERSION_W ? NSMBWUP_REVISION_W : \
    (version) == NSMBWUP_VERSION_C ? NSMBWUP_REVISION_C : \
    NSMBWUP_REVISION_V1)
