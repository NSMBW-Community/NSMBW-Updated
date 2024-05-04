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
#include "nsmbwup_common.h"
#include "nsmbwup_user_config.h"


// Patches for the world map HUD (dCourseSelectGuide_c).


#ifndef NSMBWUP_C01600_OFF

// This is a backport of a change Nintendo made in the Korean version,
// and all later versions.

// When entering View Map mode, most of the button prompts in the
// corners of the screen begin to slide away for one frame, but then
// immediately disappear. The one in the bottom left changes its text
// from "View Map" to "Back to Mario" (while still visible), finishes
// sliding away, and then slides back into view.

// The intended animation, though, is for all of the button prompts to
// finish sliding away before the text changes and the bottom-left one
// reappears.

// This happens due to an issue with the logic in
// dCourseSelectGuide_c::executeState_ScrollGuideOnStageWait(), the
// "execute" method for StateID_ScrollGuideOnStageWait in
// dCourseSelectGuide_c's button-prompts state machine (sStateMgr_c).
// This state represents the time period when the button prompts are
// sliding away, and its only purpose is to wait for the animation to
// end before transitioning to the next state
// (StateID_ScrollGuideOnStageAnimeEndCheck), at which point the layout
// will be updated to change the text in the bottom-left and to hide the
// right-side button prompts.

// The function checks a few flags, but fails to actually check whether
// the animation has ended before triggering the transition. This causes
// it to transition after just one frame, while the animation is still
// ongoing.

// The fix is just to add a check that prevents the transition from
// occurring while the animation is still active.

// Implementation note: this patch only replaces the end of the buggy
// function (the tail call to the changeState() method), not the entire
// function.

#if IS_GAME_VERSION_DYNAMIC || (GAME_REVISION < GAME_REVISION_K)

class sStateIDIf_c {};

class sStateMgr_c {
public:
    virtual ~sStateMgr_c();
    virtual void initializeState();
    virtual void executeState();
    virtual void finalizeState();
    virtual void changeState(const sStateIDIf_c&);
    virtual void refreshState();
    virtual void getState();
    virtual void getNewStateID();
    virtual void getStateID();
    virtual void getOldStateID();
};

class LytBase_c {
public:
    /* 0x000 */ u8 pad[0x198];

    bool isAnime(int id);
};

class dCourseSelectGuide_c {
public:
    /* 0x000 */ u8 pad_1[0x8];
    /* 0x008 */ LytBase_c layout;
    /* 0x1a0 */ u8 pad_2[0x1a4];
    /* 0x344 */ sStateMgr_c stateManager;

    static const sStateIDIf_c StateID_ScrollGuideOnStageAnimeEndCheck;
};

// In DYNAMIC mode, we declare this as a function with a name we can
// reference, so we can write some code to perform a runtime patch
// depending on the results of the version check.
// Otherwise, we just use Kamek's standard patching machinery.

#ifdef IS_GAME_VERSION_DYNAMIC
static void executeState_ScrollGuideOnStageWait_new_end(dCourseSelectGuide_c *this_) {
#else
kmBranchDefCpp(0x800122fc, NULL, void, dCourseSelectGuide_c *this_) {
#endif  // IS_GAME_VERSION_DYNAMIC

    // New code
    if (this_->layout.isAnime(6))
        return;

    // Original code
    this_->stateManager.changeState(
        dCourseSelectGuide_c::StateID_ScrollGuideOnStageAnimeEndCheck);
}


#ifdef IS_GAME_VERSION_DYNAMIC

// In DYNAMIC mode, we add some extra code to the end of
// dCourseSelectGuide_c::__ct() to check the game version at runtime
// and manually apply the patch if appropriate.

kmBranchDefCpp(0x8000fe48, NULL, dCourseSelectGuide_c*, dCourseSelectGuide_c *this_) {
    u32 version = _nsmbwup_check_game_version();

    if (version != 0 && _NSMBWUP_GET_GAME_REVISION(version) < _NSMBWUP_GAME_REVISION_K) {
        u32 b_src = 0x800122fc;  // Correct for P1, P2, E1, E2, J1, J2
        u32 inst = _NSMBWUP_ASSEMBLE_BRANCH(
            b_src,
            (u32)&executeState_ScrollGuideOnStageWait_new_end
        );
        *((u32*)b_src) = inst;
    }

    return this_;
}
#endif  // IS_GAME_VERSION_DYNAMIC

#endif  // IS_GAME_VERSION_DYNAMIC || (GAME_REVISION < GAME_REVISION_K)

#endif  // !NSMBWUP_C01600_OFF
