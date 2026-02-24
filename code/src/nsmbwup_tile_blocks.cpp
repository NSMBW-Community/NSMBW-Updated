// MIT License

// Copyright (c) 2024-2026 RoadrunnerWMC, CLF78

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


// Patches related to tile-block management in dBg_c, dBgUnit_c, and
// other related classes.


#ifndef NSMBWUP_C01900_OFF

// dBg_c::__GetUnitPointer() (80077520) gets a pointer to the requested
// layer (dBgUnit_c), and then calls a method on it to retrieve the
// requested tile from that layer. However, it doesn't check that the
// dBgUnit_c pointer is non-null before using it. Since dBgUnit_c's
// aren't created for layers with no corresponding bgdat file in the
// level archive, it *is* possible for this pointer to be null in
// certain custom levels.

// This can be triggered by spawning any actor that checks tile
// collisions (such as the player, or most enemies) on layer 0 or 2 in a
// level without a bgdat file for that layer.

// We fix this by simply making the function return null if the layer
// pointer is null.

extern "C" void __GetUnitPointer__5dBg_cFUsUsUcPib__epilogue();

kmBranchDefAsm(0x80077560, 0x80077564) {
    nofralloc
    cmpwi r3, 0
    bne+ continue
    // Return 0 (already in r3, since otherwise the previous instruction
    // would've branched)
    b __GetUnitPointer__5dBg_cFUsUsUcPib__epilogue
continue:
    mr r4, r28  // Restore the original instruction
    blr
}

#endif  // !NSMBWUP_C01900_OFF


#if !defined(NSMBWUP_C01900_OFF) || !defined(NSMBWUP_C02000_OFF)

// dBg_c::GetBgCheckData() (80077650), which gets the tile-collision
// data for a given position, doesn't check whether the tile-block
// pointer it receives from dBg_c::__GetUnitPointer() (80077520) is
// null.

// This can cause a crash when a player or other tile-collision-aware
// actor is spawned on a nonexistent layer (C01900), or when one of them
// ventures into a region without a tile block (as can happen with
// the C02000 patch enabled). In either case, the function dereferences
// a null pointer, and the game crashes.

// This patch adds a null-pointer check. If the pointer is null, the
// function is made to skip the rest of its original logic and return a
// collision value of 0, which indicates empty space.

extern "C" void GetBgCheckData__5dBg_cFUsUsUc__epilogue();

kmBranchDefAsm(0x800776b8, 0x800776bc) {
    nofralloc
    cmpwi r3, 0
    bne+ continue
    // Return 0. 0 is already in r3 (since otherwise the previous
    // instruction would've branched), but the function returns a 64-bit
    // integer, so be sure to put 0 in r4 as well
    li r4, 0
    b GetBgCheckData__5dBg_cFUsUsUc__epilogue
continue:
    mr r4, r3  // Restore the original instruction
    blr
}

#endif  // !NSMBWUP_C01900_OFF || !NSMBWUP_C02000_OFF


#ifndef NSMBWUP_C02000_OFF

// dBgUnit_c is the class that stores tile data at runtime for a single
// layer. Since most parts of the area canvas are not within any zone
// and don't contain any tiles, it uses a sparse representation to keep
// memory usage reasonable: it divides the area into 64x32 chunks
// ("blocks" or "BgBuffers") of 16x16 tiles each, and only allocates
// memory for blocks that are needed. It does this using:
//
// - An array of 64 * 32 bytes, one for each possible block position in
//   the area. Nonzero values represent indices into:
// - An array of up to 256 pointers to allocated tile blocks, which are
//   stored in:
// - A private heap.

// Unlike its counterpart in the previous NSMB game, rather than only
// allocating blocks that contain actual tiles, dBgUnit_c actually
// allocates every block that touches any zone in the area. This design
// decision may be a sort of stability precaution, as the rest of the
// game code can then generally assume that any tile block it wants to
// access will exist, without checking for null pointers. Although
// somewhat wasteful, the game usually has plenty of RAM for this.

// However, it *can* be a problem for custom levels. Since only 256
// blocks can be allocated per layer in a given area, and every block
// that any zone touches is allocated, there's a hard upper limit on the
// maximum total area of all zones in an area: 65536 tiles.

// To make larger custom levels possible, this set of patches stops
// dBgUnit_c from allocating tile blocks that don't contain any tiles,
// and fixes various issues this change would otherwise cause.

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// 0 is used as the "no block allocated" sentinel value in dBgUnit_c's
// block-indices array, which means that index 0 in the block-pointers
// array can't actually be used for anything. The class accounts for
// this by allocating a dummy block for index 0 (at 800838c0) before
// allocating the actual blocks.

// This dummy block unfortunately wastes some space in the class's heap.
// A better solution that saves some memory is to initialize the "next
// block index" field (which can be equivalently described as "number of
// allocated blocks") to 1 instead of 0 when the class is created.

// This patch for dBgUnit_c::InitBgBuffer() (80083660) does that. (It
// doesn't prevent the dummy block from being allocated, though --
// that's done as part of the next patch.)

// Note: the pointer at index 0 being null doesn't cause problems when
// the class is later destroyed, because the tile-block heap is just
// destroyed wholesale without freeing its blocks individually (see
// dBgUnit_c::~dBgUnit_c(), 800834d0).

kmWrite32(0x800837c0, 0x38a00001);  // li r5, 1
kmWrite32(0x800837c4, 0xb0a30c08);  // sth r5, 0xc08(r3)
kmWrite32(0x800837c8, 0x4e800020);  // blr

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// This is the main patch to prevent over-eager tile block allocation.
// It inserts a jump into dBgUnit_c::initialize() (80083870) that skips
// over most of the function, including the initial dummy block
// allocation and the loop that allocates all blocks that touch a zone.

// This only affects tile blocks that don't contain tiles, because the
// game will still allocate any missing tile blocks when laying out
// tiles from object data (at 80083e80).

kmWrite32(0x800838b8, 0x480001bc);  // b 0x1bc  (dest. 80083a74)

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// dBg_c::BgUnitChange() (80077860), which places a tile in a layer,
// doesn't check for a null return value from dBg_c::__GetUnitPointer().
// This causes problems if a Tile God (daChengeBlock_c [sic] / sprite
// 191 / profile 510 / AC_CHENGE_BLOCK [sic]) is used within a tile
// block that doesn't initially contain any tiles, and thus doesn't get
// allocated.

// In this case, if the tile block doesn't exist, it should be created.
// Rather than patching the function in some complex / invasive way to
// accomplish that, we replace the call to dBg_c::__GetUnitPointer()
// with a call to a new function that wraps it and also allocates the
// new block if necessary.

extern "C" void OSPanic(const char *file, int line, const char *format, ...);

class dBg_c {
public:
    unsigned short* __GetUnitPointer(
        unsigned short x,
        unsigned short y,
        unsigned char layer,
        int *blockNum,
        bool placeTile
    );
    bool CheckExistLayer(unsigned char layer);
};

class mHeap {
public:
    static void saveCurrentHeap();
    static void restoreCurrentHeap();
};

namespace EGG {
    class Heap {
    public:
        void becomeCurrentHeap();
    };

    class FrmHeap : public Heap {};
};

class dBgUnit_c {
public:
    /* 0x000 */ unsigned char _pad[0xc24];
    /* 0xc24 */ EGG::FrmHeap *frmHeap;  // unofficial name

    void CreateBgBuffer(int slotNumber);
};

class dBgGlobal_c {
public:
    dBgUnit_c* GetBgUnitP(int area, int layer);

    static dBgGlobal_c *ms_pInstance;
};

class dScStage_c {
public:
    /* 0x0000 */ unsigned char _pad[0x120e];
    /* 0x120e */ unsigned char curArea;  // unofficial name

    static dScStage_c *m_instance;
};

/// A wrapper for dBg_c::__GetUnitPointer() with the same signature and
/// purpose, but which also allocates a new tile block if the block the
/// requested tile would be in hasn't previously been allocated. Should
/// never return NULL, except for invalid arguments or trying to
/// allocate more than 256 tile blocks on a single layer.
unsigned short* __GetUnitPointer_allocateIfNecessary(
    dBg_c *this_,
    unsigned int x,
    unsigned int y,
    unsigned char layer,
    unsigned int *blockNum,
    bool placeTile
) {
    // __GetUnitPointer() crashes if the layer doesn't exist, so we
    // should check that first. (The patch for C01900 replaces the crash
    // with a null return value, but we can't assume that that patch is
    // enabled.)

    // As far as I can tell, there's no way to make this particular
    // error condition happen without further code modifications, since
    // every caller of this function either hardcodes the layer to be 1
    // (which is guaranteed to exist) or can be sure that the layer
    // exists for some contextual reason (e.g. a ?-block tile being
    // replaced by a used-block tile on the same layer). With that said,
    // it's not uncommon for mods to patch the Tile God actor to allow
    // it to paint onto different layers, so this check *can* be useful
    // in conjunction with that.

    if (!this_->CheckExistLayer(layer)) {
        OSPanic(__FILE__, __LINE__,
            "Can't draw tiles to layer %d because it's not allocated.\n\n"
            "Ensure the layer is allocated by placing"
            " at least one object on it somewhere in the level file.",
            (layer + 1) % 3
        );
    }

    unsigned short *tile = this_->__GetUnitPointer(x, y, layer, blockNum, placeTile);

    if (!tile) {
        // Get a pointer to the layer (dBgUnit_c) (we can safely assume
        // this is non-null thanks to the CheckExistLayer() call above)
        dBgUnit_c *unit = dBgGlobal_c::ms_pInstance->GetBgUnitP(dScStage_c::m_instance->curArea, layer);

        // Temporarily switch the current global heap to its private
        // tile-blocks heap...
        mHeap::saveCurrentHeap();
        unit->frmHeap->becomeCurrentHeap();

        // ...allocate the new block...
        // (slot number calculation: see dBgUnit_c::GetBgBufIndex()
        // (80083f90))
        unsigned int slotNumber = ((x >> 8) & 0xff) + ((y >> 2) & 0x3fc0);
        unit->CreateBgBuffer(slotNumber);

        // ...and switch back to the original global heap.
        mHeap::restoreCurrentHeap();

        // Now that the block is allocated, call the function again
        // (should return a non-null pointer this time)
        tile = this_->__GetUnitPointer(x, y, layer, blockNum, placeTile);
    }

    return tile;
}

// Replace dBg_c::BgUnitChange()'s call to dBg_c::__GetUnitPointer()
// with a call to the new function
kmCall(0x800778c0, __GetUnitPointer_allocateIfNecessary);

#endif  // !NSMBWUP_C02000_OFF
