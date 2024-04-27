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


// Patches for "direct pipes" (aka "connected pipes") entrance
// functionality, which is implemented in daPlBase_c.

// Quick aside: "entrance" is the community term for "NextGoto", and
// "path" is the community term for "rail".


typedef struct NextGoto {
    /* 0x00 */ unsigned short x;
    /* 0x02 */ unsigned short y;
    /* 0x04 */ unsigned short _4;
    /* 0x06 */ unsigned short _6;
    /* 0x08 */ unsigned char id;
    /* 0x09 */ unsigned char dest_area;
    /* 0x0a */ unsigned char dest_id;
    /* 0x0b */ unsigned char type;
    /* 0x0c */ unsigned char _c;
    /* 0x0d */ unsigned char zone_id;
    /* 0x0e */ unsigned char layer;
    /* 0x0f */ unsigned char direct_pipe_path_id;
    /* 0x10 */ unsigned short flags;
    /* 0x12 */ bool leave_stage;
    /* 0x13 */ unsigned char _13;
} NextGoto;


class dCdFile_c {
public:
    NextGoto *getNextGotoP(unsigned char id);
};


#ifndef NSMBWUP_C01100_OFF

// Thanks to Ninji (Treeki) for this patch.

// daPlBase_c::initializeState_DemoRailDokan() is responsible for
// starting a direct-pipes player movement animation. This involves
// initializing a daPlBase_c field unofficially known as
// "nextPathNodeIndex", to either "1" or "path->numNodes - 2" depending
// on the entrance's "direct pipe reversed" flag (which makes the player
// follow the path backward from the end). It also checks that first
// node's position, to set the first target position for the player to
// linearly slide to.

// Unfortunately, though, it performs those two operations in the wrong
// order: it uses a *stale* value of "nextPathNodeIndex" to get the
// pointer to the first path node, before initializing it right after.
// This means the first path node may be anything: usually it's the
// first one in the area (index 0) the first time a player uses a
// direct-pipe entrance, and then some other, often invalid value for
// subsequent direct-pipe traversals. This frequently results in the
// player traveling to their death somewhere outside of the zone
// boundaries.

// To summarize, the function does

//     lvlRailNode *firstNode = area->pathNodesBlock[
//         path->startNodeIndex + this->nextPathNodeIndex];
//     this->nextPathNodeIndex = (destEntrance->flags & 0x0001)
//         ? (path->numNodes - 2) : 1;

// and the order of those two lines needs to be swapped.

// To do this, we simply overwrite that block of code with essentially
// the same assembly instructions, shuffled into the correct order.

// First line (initializing the next-path-node index)
       /* 0x800508f8 */             // lhz r0, 0x10(r30)              // r0 = destEntrance->flags;
kmWrite32(0x800508fc, 0x540007ff);  // clrlwi. r0, r0, 0x1f           // r0 &= 0xff;
kmWrite32(0x80050900, 0x41820014);  // beq NOT_DIRECT_PIPE_END        // if (r0 & DIRECT_PIPE_END) {
kmWrite32(0x80050904, 0xa0e30004);  // lhz r7, 4(r3)                  //     this->nextPathNodeIndex = path->numNodes - 2;
kmWrite32(0x80050908, 0x3807fffe);  // subi r0, r7, 0x2               //
kmWrite32(0x8005090c, 0xb01f042c);  // sth r0, 0x42c(r31)             //
kmWrite32(0x80050910, 0x4800000c);  // b AFTER_DIRECT_PIPE_END_CHECK  // }
                                    // NOT_DIRECT_PIPE_END:           // else {
kmWrite32(0x80050914, 0x38000001);  // li r0, 1                       //     this->nextPathNodeIndex = 1;
kmWrite32(0x80050918, 0xb01f042c);  // sth r0, 0x42c(r31)             //
                                    // AFTER_DIRECT_PIPE_END_CHECK:   // }

// Second line (calculating the first-node pointer)
kmWrite32(0x8005091c, 0xa0a30002);  // lhz r5, 2(r3)                  // r5 = path->startNodeIndex;
kmWrite32(0x80050920, 0xa89f042c);  // lha r4, 0x42c(r31)             // r4 = this->nextPathNodeIndex;
kmWrite32(0x80050924, 0x80c6003c);  // lwz r6, 0x3c(r6)               // r6 = area->pathNodesBlock;
kmWrite32(0x80050928, 0x7c052214);  // add r0, r5, r4                 // r0 = path->startNodeIndex + this->nextPathNodeIndex;
kmWrite32(0x8005092c, 0x54002036);  // slwi r0, r0, 4                 // (left-shift to prepare for indexing)
kmWrite32(0x80050930, 0x7ca60214);  // add r5, r6, r0                 // r5 = area->pathNodesBlock[r0];

#endif  // !NSMBWUP_C01100_OFF


#ifndef NSMBWUP_C01200_OFF

// daPlBase_c::setExitRailDokan(), called at the end of direct-pipes
// animations, switches the player to the correct animation state for
// exiting from the end of the pipe. To choose the right state, it needs
// to know what direction the exit pipe faces.

// It does this by checking the entrance's type. Unfortunately, though,
// it uses the wrong entrance for this -- specifically, the one the
// player *entered*, rather than its *destination*.

// We fix this by hooking on the instruction that calls
// dCdFile_c::getNextGotoP() (which gets the pointer to an entrance,
// given its ID) and replacing it with a new function that recreates
// that call and also adds another to get the pointer to *that*
// entrance's destination. This second pointer is then used for the
// type/direction check.

// We ignore the first entrance's destination-area value. While it
// really ought to match the current area ID, there isn't any useful
// behavior we could do if it doesn't.

kmCallDefCpp(0x80050a50, NextGoto*, dCdFile_c *area, unsigned char src_entrance_id) {
    // Original code (gets the entrance the player entered):
    NextGoto *src_ent = area->getNextGotoP(src_entrance_id);
    // New code (gets that entrance's destination):
    return area->getNextGotoP(src_ent->dest_id);
}

#endif  // !NSMBWUP_C01200_OFF


#ifndef NSMBWUP_C01300_OFF

// daPlBase_c::setExitRailDokan(), called at the end of direct-pipes
// animations, switches the player to the correct animation state for
// exiting from the end of the pipe. Specifically, it checks the
// destination entrance's (or the *source* entrance's, if bug C01200
// isn't fixed) type value -- which indicates the pipe direction -- and
// sets the player's "demo" (cutscene) state to one of
// daPlBase_c::StateID_DemoInDokanU, D, L, or R.

// But due to what can only be a typo, it uses StateID_DemoInDokanU for
// *both* of the vertical pipe directions, and never uses
// StateID_DemoInDokanD. The state ID names are a bit unintuitive: this
// bug makes the player exit from upward-facing pipes in the downward
// direction.

// We fix this by replacing the instruction that loads the wrong StateID
// address with some code that loads the right one.

// Implementation note: for each game version in isolation, this can be
// done by replacing a single instruction, but since that instruction
// uses an offset from an arbitrary address (in r31) that was calculated
// earlier in the function, this method should be somewhat safer and
// more robust against different builds of the game with different
// memory layouts.

// Implementation note 2: Newer SMBW works around this bug by instead
// encoding "direction of other side" as a new setting in byte 0x13 of
// the NextGoto struct. That byte is never referenced at all in the
// original game, and that fix clearly doesn't match the intent of the
// original code, which checks entrance types. The fix presented here is
// more correct, but may not work with certain levels that expect
// Newer's version.

extern "C" void* StateID_DemoInDokanD__10daPlBase_c;

kmBranchDefAsm(0x80050a88, 0x80050a8c) {
    lis r4, StateID_DemoInDokanD__10daPlBase_c@h
    ori r4, r4, StateID_DemoInDokanD__10daPlBase_c@l
}

#endif  // !NSMBWUP_C01300_OFF
