# Embedding NSMBW-Updated

This page explains how to include NSMBW-Updated's bugfixes in your own NSMBW mod projects.


## The problem with asset bugfixes

Every bugfix in NSMBW-Updated either:

* Patches code, or
* Patches assets.

The code patches are produced as a Kamek 2 patch file. These are easy to apply with Riivolution, and don't contain any copyrighted Nintendo code.

On the other hand, no systems currently exist for patching (as opposed to wholly replacing) asset files at runtime on a Wii, so NSMBW-Updated's asset patching needs to be done at build time. This is why you have to build its standalone Riivolution patch yourself. This is pretty unusual for NSMBW mods, though -- at time of writing, no other mod requires every player to perform this extra step.

Additionally, while code patches are useful broadly, asset files tend to be replaced *anyway* by mod projects (think of levels, tilesets, the credits staffroll, and so on). So bugfixes that apply to those are, generally speaking, less valuable in that context than the code bugfixes are.

For both of these reasons, NSMBW-Updated is designed to make it relatively easy to include its *code* bugfixes in other mods, but its *asset* bugfixes aren't optimized for that. The rest of this page will only discuss code bugfixes.


## Kamek 1 vs Kamek 2

NSMBW-Updated's code patches are a [Kamek 2 (also known as "C# Kamek")](https://github.com/Treeki/Kamek) project. If you're using [Kamek 1 (also known as "Python Kamek")](https://github.com/Newer-Team/NewerSMBW), you'll have to switch to Kamek 2 before you can use these patches.


## Setup

NSMBW-Updated uses the [Kamek Ninja Template](https://github.com/NSMBW-Community/Kamek-Ninja-Template) for its code, albeit in a slightly unusual way since it's integrated into a larger overall build system (which *also* uses Python and Ninja).

### If you're starting a new Kamek project

If you're starting from scratch, the easiest approach is to start with a clean copy of [the template](https://github.com/NSMBW-Community/Kamek-Ninja-Template) and copy NSMBW-Updated's `src` folder, `include` folder, and `externals.txt` into it. It should work perfectly out-of-the-box.

### If you want to incorporate it into an existing Kamek project

If your project is using the same [template](https://github.com/NSMBW-Community/Kamek-Ninja-Template), you can just add NSMBW-Updated's files to the `src` and `include` directories (and merge its `externals.txt` with yours). All of the filenames are prefixed with "`nsmbwup_`" to help keep them separate from your mod's other code files.

Otherwise, just make sure that whatever build system you're using can see the cpp files, and that the folder containing the headers from `include` is passed to CodeWarrior with `-i` (not `-I`).

If you only want a few specific source code files instead of all of them, it's possible to extract them individually, as they're designed to be mostly independent of each other. Just be sure to also take `src/nsmbwup_common.cpp`, `include/nsmbwup_common.h`, and `include/nsmbwup_user_config.h` (and also `include/game_versions_nsmbw.h` if you don't already have it).

## Configuration

By default, all bugfixes are enabled, and are configured to be built in a game-version-agnostic way. To adjust that configuration, read on.

### How to set preprocessor symbols

Before explaining what preprocessor symbols are available, let's quickly talk about the two places where you can set them.

Like most other C++ compilers, CodeWarrior supports defining preprocessor symbols on the command line with `-DVARNAME` (that is, `-D` followed by the name of the symbol, with nothing in between). If you're using your own build system, you can adjust it to add NSMBW-Updated configuration symbols here.

Alternatively, you can `#define` your symbols in `nsmbwup_user_config.h`, in the `include` directory. This is an empty header file that's `#include`d in every NSMBW-Updated C++ file, specifically for this purpose. Depending on your personal preference, this may be more convenient for you than providing more command-line arguments to CodeWarrior. You can even add more complex logic here, such as conditionally enabling or disabling a certain bugfix depending on the target game version (though I don't know why you'd want to do that).

### Configuration preprocessor symbols

#### Target game version

NSMBW-Updated uses the `IS_GAME_VERSION_{version}_COMPATIBLE` symbols from the [Kamek Ninja Template](https://github.com/NSMBW-Community/Kamek-Ninja-Template) to adjust compilation for specific game versions. If you're using that build system, and you keep the JSON files from the `src` folder, it'll compile specialized patches for each game version when appropriate. Otherwise, it'll assume `IS_GAME_VERSION_DYNAMIC` by default, and compile in a generic way compatible with all game versions. See the Kamek Ninja Template documentation for more information about how this system works.

#### Bug options

As mentioned earlier, by default, every bugfix is built, with its default option if applicable.

To disable the fix for a particular bug, define a `NSMBWUP_{id}_OFF` flag, where "`{id}`" is the bug ID. For example, `NSMBWUP_C00900_OFF`.

To select a different option (for bugs that support it), define a `NSMBWUP_{id}_{option}` flag. For example, `NSMBWUP_C00504_ICEBALL`.
