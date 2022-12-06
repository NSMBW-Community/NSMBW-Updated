# Embedding NSMBW-Updated

This page explains how to include NSMBW-Updated's bugfixes in your own NSMBW mod projects.


## The problem with asset bugfixes

Every bugfix in NSMBW-Updated can be categorized as one of the following:

* Bugfixes that patch code
* Bugfixes that patch assets

The code patches are produced as a Kamek 2 patch file. These are easy to apply with Riivolution, and don't contain any copyrighted Nintendo code.

On the other hand, no systems currently exist for patching (as opposed to wholly replacing) asset files at runtime on a Wii, so NSMBW-Updated's asset patching needs to be done at build time. This is why you have to build its standalone Riivolution patch yourself. This is pretty unusual for NSMBW mods, though -- at time of writing, no other mod requires every player to perform this extra step.

Additionally, while code patches are useful broadly, asset files tend to be replaced *anyway* by mod projects (think of levels, tilesets, the credits staffroll, and so on). So bugfixes that apply to those are, generally speaking, less valuable in that context than the code bugfixes are.

For both of these reasons, NSMBW-Updated is designed to make it relatively easy to include its *code* bugfixes in other mods, but its *asset* bugfixes aren't optimized for that. The rest of this page will only discuss code bugfixes.


## Kamek 1 vs Kamek 2

NSMBW-Updated's code patches are a [Kamek 2 (also known as "C# Kamek")](https://github.com/Treeki/Kamek) project. If you're using [Kamek 1 (also known as "Python Kamek")](https://github.com/Newer-Team/NewerSMBW), you'll have to switch to Kamek 2 before you can use these patches.


## How to add NSMBW-Updated's code to your Kamek 2 project

NSMBW-Updated's code lives in the `code` directory, with C++ files in `src` and headers in `include`. If your Kamek project is organized in the same way, you can just add NSMBW-Updated's files to those directories and ensure that your build system is aware of them. All of the filenames are prefixed with "`nsmbwup_`" to help keep them separate from your mod's other code files.

By default, all bugfixes are enabled, and are configured to be built in a game-version-agnostic way. To adjust that configuration, read on.


## How to set preprocessor variables

Before explaining what preprocessor variables are available, let's quickly talk about the two places where you can set them.

Like most other C++ compilers, CodeWarrior supports defining preprocessor variables on the command line with `-DVARNAME` (that is, `-D` followed by the name of the variable, with nothing in between). You can adjust your build system to add NSMBW-Updated configuration variables here.

Alternatively, you can `#define` your variables in `nsmbwup_user_config.h`, in the `include` directory. This is an empty header file that's `#include`d in every NSMBW-Updated C++ file, specifically for this purpose. Depending on your personal preference, this may be more convenient for you than providing more command-line arguments to CodeWarrior. You can even add more complex logic here, such as conditionally enabling or disabling a certain bugfix depending on the target game version (though I don't know why you'd want to do that).


## Configuration preprocessor variables

### Target game version

The `NSMBWUP_VERSION` variable indicates the game version to target statically at compile-time. The default value is `NSMBWUP_VERSION_DYNAMIC`, which is a special "version" that ensures that each active bugfix is built in a generic way compatible with all game versions. The other valid values represent specific game versions: `NSMBWUP_VERSION_P1`, `NSMBWUP_VERSION_E2`, etc. Compiling separately for different game versions increases build time, but produces more optimized Kamekfiles. **(TODO: explain how the json files help with this)**

The specific numeric values of the `NSMBWUP_VERSION_{name}` constants are subject to change. Since that prevents you from safely specifying `NSMBWUP_VERSION`'s value on the command line, you can instead set a `NSMBWUP_SET_VERSION_{name}` flag, which'll cause `NSMBWUP_VERSION` to be set to the version you named.

### Bug options

As mentioned earlier, by default, every bugfix is built, with its default option if applicable.

To disable the fix for a particular bug, define a `NSMBWUP_{id}_OFF` flag, where "`{id}`" is the bug ID. For example, `NSMBWUP_C00900_OFF`.

To select a different option (for bugs that support it), define a `NSMBWUP_{id}_{option}` flag. For example, `NSMBWUP_C00504_ICEBALL`.
