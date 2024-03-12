# Kamek Ninja Template Tutorial

This page will go over two examples: one to show basic / common usage, and one to demonstrate how to compile separately for different game versions. I'll use *New Super Mario Bros. Wii* because that's what I'm familiar with, but nothing about this build system is specific to NSMBW beyond some design limitations of Kamek itself (e.g. no support for patching truly relocatable game code).

Since the focus here is on how to use the build system, not C++ or Kamek itself, I won't go into detail on how the patches work or how to create your own. Look up Kamek documentation or tutorials for that.


## Example 1: multiple C++ files

Let's start by demonstrating a basic project with multiple C++ files.

### Setup

First, download this repo and install the dependencies listed in [the readme](readme.md).

Next, add a couple source files to the `src` directory. `goomba.cpp`:

```cpp
#include <kamek.h>

// Faster Goomba movement speed
kmWrite32(0x80ad2870, 0x40000000);  // 0.5 -> 2.0
kmWrite32(0x80ad2874, 0xc0000000);  // -0.5 -> -2.0

// Faster Goomba bahp jump speed, animation speed, and other uses
kmWrite32(0x8042b7c8, 0x41000000);  // 2.0 -> 8.0
```

`timer.cpp`:

```cpp
#include <kamek.h>

// Faster timer countdown speed
kmWrite32(0x800e3ab8, 0x3403fe90);  // 92 -> 368
```

These could, of course, easily be combined into one file, but they're separate here to demonstrate multiple C++ files being linked together correctly.

### Configuration

Now let's try to build it. First, run `configure.py -h` to see what configuration options are available:

```
$ python3 configure.py -h
usage: configure.py [-h] --kamek KAMEK --kstdlib KSTDLIB --cw MWCCEPPC [--project-dir PROJECT] [--select-version VERSION] [--build-dir BUILD]
                    [--output-dir OUT]

Creates a Ninja file matching the configuration options you specify.

options:
  -h, --help            show this help message and exit

Kamek location:
  --kamek KAMEK         the Kamek binary ("Kamek.exe" on Windows, "Kamek" on other platforms)
  --kstdlib KSTDLIB     Kamek's "k_stdlib" directory

CodeWarrior location:
  --cw MWCCEPPC         CodeWarrior's "mwcceppc.exe"

Project location:
  --project-dir PROJECT
                        project directory (with src/, include/, etc.) (default: current directory)

Output options:
  --select-version VERSION
                        build only for the indicated game version, instead of all of them (can be specified multiple times)
  --build-dir BUILD     directory to put object files and Ninja's bookkeeping files ("$builddir") (default: <project dir>/_build)
  --output-dir OUT      output directory to put Kamekfiles in (default: <project dir>/bin)

Any additional arguments will be passed directly to CodeWarrior.
$
```

There are three required arguments: `--kamek`, `--kstdlib`, and `--cw`. Run `configure.py` with those set to correct values:

```
$ # This command is only correct for *my* system -- change the paths below as necessary!
$ python3 configure.py --kamek ../Kamek/Kamek/bin/Debug/net6.0/Kamek --kstdlib ../Kamek/k_stdlib/ --cw ../Kamek/cw/mwcceppc.exe
$
```

If nothing's printed and a `build.ninja` file appears, configuration was successful.

### Running Ninja

Now run `ninja` to perform the build. The output should look something like this:

```
$ ninja
[1/10] mwcceppc.exe -o timer.o timer.cpp
[2/10] mwcceppc.exe -o goomba.o goomba.cpp
[3/10] Kamek -> P2.bin
Kamek 2.0 by Ninji/Ash Wolf - https://github.com/Treeki/Kamek

adding /[snip]/_build/goomba.o as object..
adding /[snip]/_build/timer.o as object..
(skipping version P1 as it's not selected)
(skipping version E1 as it's not selected)
(skipping version J1 as it's not selected)
linking version P2...
(skipping version E2 as it's not selected)
(skipping version J2 as it's not selected)
(skipping version K as it's not selected)
(skipping version W as it's not selected)
[4/10] Kamek -> E2.bin
Kamek 2.0 by Ninji/Ash Wolf - https://github.com/Treeki/Kamek

adding /[snip]/_build/goomba.o as object..
adding /[snip]/_build/timer.o as object..
(skipping version P1 as it's not selected)
(skipping version E1 as it's not selected)
(skipping version J1 as it's not selected)
(skipping version P2 as it's not selected)
linking version E2...
(skipping version J2 as it's not selected)
(skipping version K as it's not selected)
(skipping version W as it's not selected)

[snip...]

[10/10] Kamek -> W.bin
Kamek 2.0 by Ninji/Ash Wolf - https://github.com/Treeki/Kamek

adding /[snip]/_build/goomba.o as object..
adding /[snip]/_build/timer.o as object..
(skipping version P1 as it's not selected)
(skipping version E1 as it's not selected)
(skipping version J1 as it's not selected)
(skipping version P2 as it's not selected)
(skipping version E2 as it's not selected)
(skipping version J2 as it's not selected)
(skipping version K as it's not selected)
linking version W...
$
```

Ninja shows 10 steps for this build (`[1/10]` through `[10/10]`). That's:

* 2 steps to compile the two C++ files with CodeWarrior, and
* 8 steps to link it for each version of the game supported by this address map, known as P1, E1, J1, P2, E2, J2, K, and W.

> **Note**: Kamek supports linking for all versions simultaneously, so strictly speaking, linking eight separate times like this is redundant. However, the second example will show how doing this gives us extra flexibility -- and Kamek is fast enough (and Ninja parallel enough) that it doesn't significantly affect build times.

Intermediate object files are placed in the `_build` directory, and the output Kamekfiles can be found in `bin`. You should see one for each region.

If you try these Kamekfiles in *New Super Mario Bros. Wii* with the Kamek NSMBW loader, you'll get to enjoy hyper-fast Goombas and a 4x-speed timer.

### Incremental builds

Since Ninja knows the dependency relationships between all of the files in the project, it's able to perform "incremental builds", meaning that when you change one file, it can re-compile only the files that actually changed, reusing the other outputs from the previous build.

To demonstrate this, let's say we want the timer to run at 10x speed instead of 4x. Change `timer.cpp` like so:

```cpp
#include <kamek.h>

// Faster timer countdown speed
kmWrite32(0x800e3ab8, 0x3403fc68);  // 92 -> 920
```

Run `ninja` again to rebuild. Notice that only 9 total steps are shown this time instead of 10: Ninja can see that we didn't modify `goomba.cpp`, so it can reuse the `goomba.o` it compiled last time.

If you run `ninja` a third time, without making any changes at all, it knows it doesn't need to do anything:

```
$ ninja
ninja: no work to do.
$
```

## Example 2: compilation per game version

Kamek can smooth over most differences between game versions by automatically translating addresses, but sometimes there are semantic differences that need to be handled manually. This build system provides a simple way to do that, to the extent that it's possible with Kamek.

### Demonstrating the problem

Let's say you want to try out the unused `TIME_UP` layout actor left over from the prerelease demos. This can be done with two patches, shown below for PAL v1 (also known as "P1"):

```cpp
#include <kamek.h>

// During level load, create a TIME_UP instead of a COURSE_TIME_UP
kmWrite32(0x80926010, 0x386002be);  // profile 0x2bf -> 0x2be

// Have it display upon initialization instead of waiting for something to trigger it
kmWrite32(0x807b35f4, 0x989f0259);
```

This will cause the layout to display over every level at all times.

However, this patch doesn't work for every game version: the global table of profiles was modified in the Korean and Taiwanese versions, and this actor's profile ID was increased to 0x2c0. So for those versions, the first patch would need to be changed to this:

```cpp
kmWrite32(0x80926010, 0x386002c0);  // profile 0x2c1 -> 0x2c0
```

But of course, that would also break the earlier versions.

### The solution

So we need one patch for the six game versions released before the Korean version, and another for the remaining two *(yes, I'm ignoring the Chinese release for this tutorial)*. We can accomplish that in a single C++ file by guarding the two patches with preprocessor directives, and telling the build system which one we want to use for each game version.

Here's the most basic way to do that. Save these as `demo_time_up.cpp` and `demo_time_up.json`, respectively, in the `src` directory:

```cpp
#include <kamek.h>

// Spawn a TIME_UP instead of a COURSE_TIME_UP
#ifdef IS_GAME_VERSION_DYNAMIC
#error Dynamic compilation is unsupported for this patch.
#elif defined(IS_GAME_VERSION_P1_COMPATIBLE)
kmWrite32(0x80926010, 0x386002be);  // profile 0x2bf -> 0x2be
#elif defined(IS_GAME_VERSION_K_COMPATIBLE)
kmWrite32(0x80926010, 0x386002c0);  // profile 0x2c1 -> 0x2c0
#else
#error Unknown game version.
#endif

// Have it display upon initialization instead of waiting for something to trigger it
kmWrite32(0x807b35f4, 0x989f0259);
```

```json
{
    "json_version": 1,
    "builds": {
        "P1": ["P1", "E1", "J1", "P2", "E2", "J2"],
        "K": ["K", "W"]
    }
}
```

### Explanation

Let's break down what that all means. First, in the JSON:

* `"json_version": 1` is just for backwards/forwards compatibility.
* The `"builds"` key has two entries, which means that `demo_time_up.cpp` should be compiled twice. Specifically:
    * It should be built once for the "P1" version, and auto-ported to the "P1", "E1", ..., and "J2" versions, and
    * It should be built again (in parallel, of course) for the "K" version, and auto-ported to the "K" and "W" versions.

Any C++ files that don't have a corresponding JSON will be built with the preprocessor flag `IS_GAME_VERSION_DYNAMIC`, which your code should interpret as "compile in a way compatible with all game versions, even if that requires dynamic version detection at runtime." **It's best practice to always check for this flag**, even if it's just to `#error`, so that if your JSON is ever accidentally deleted or renamed, you'll spot the mistake at build time instead of introducing a subtle bug that only affects a subset of game versions.

If you add a JSON to indicate that you want split compilation, each copy will be compiled with an `IS_GAME_VERSION_{name}_COMPATIBLE` flag that indicates the name of the version (in this example: `IS_GAME_VERSION_P1_COMPATIBLE` and `IS_GAME_VERSION_K_COMPATIBLE`). You should interpret those as "compile in a way that works with the `{name}` version **and any others that are equivalent to it in context**, up to differences in addresses."

### Further improvements

In addition to explicitly checking for `IS_GAME_VERSION_DYNAMIC`, it's also good practice for your preprocessor directives to account for all game versions instead of just those which you'll be *explicitly* building for, in case you ever decide to version-partition the translation unit differently in the future. While you *could* do that like this...

```cpp
// Spawn a TIME_UP instead of a COURSE_TIME_UP
#ifdef IS_GAME_VERSION_DYNAMIC
#error Dynamic compilation is unsupported for this patch.
#elif defined(IS_GAME_VERSION_P1_COMPATIBLE) \
   || defined(IS_GAME_VERSION_E1_COMPATIBLE) \
   || defined(IS_GAME_VERSION_J1_COMPATIBLE) \
   || defined(IS_GAME_VERSION_P2_COMPATIBLE) \
   || defined(IS_GAME_VERSION_E2_COMPATIBLE) \
   || defined(IS_GAME_VERSION_J2_COMPATIBLE)
kmWrite32(0x80926010, 0x386002be);  // profile 0x2bf -> 0x2be
#elif defined(IS_GAME_VERSION_K_COMPATIBLE) || defined(IS_GAME_VERSION_W_COMPATIBLE)
kmWrite32(0x80926010, 0x386002c0);  // profile 0x2c1 -> 0x2c0
#else
#error Unknown game version.
#endif
```

...that's pretty ugly and verbose. Instead, you can use a header to add some more semantically-rich preprocessor symbols, such as the provided example `game_versions_nsmbw.h`:

```cpp
#include "game_versions_nsmbw.h"

// Spawn a TIME_UP instead of a COURSE_TIME_UP
#ifdef IS_GAME_VERSION_DYNAMIC
#error Dynamic compilation is unsupported for this patch.
#elif GAME_REVISION < GAME_REVISION_K
kmWrite32(0x80926010, 0x386002be);  // profile 0x2bf -> 0x2be
#else
kmWrite32(0x80926010, 0x386002c0);  // profile 0x2c1 -> 0x2c0
#endif
```

`game_versions_nsmbw.h` adds preprocessor symbols to make it easier to compare versions by revision (older vs newer) and region. If you're modding a different game, you can look at how that header is implemented and create an equivalent one for your game!

### Even further improvements

Just for completeness, the *truly* best solution for this particular example would be to define the profile table in a separate header file, and then refer to the profile by name in the C++ code rather than hardcoding its ID. `include/profiles.h`:

```cpp
#include "game_versions_nsmbw.h"

enum Profiles {
    BOOT,
    AUTO_SELECT,
    SELECT,
    WORLD_MAP,
    WORLD_9_DEMO,
// [snip]
    EVENT_OPENING_TITLE,
    SELECT_PLAYER,
    MULTI_COURSE_SELECT,
#if GAME_REVISION >= GAME_REVISION_K
    MULTI_COURSE_SELECT_TOURNAMENT,
    MULTI_COURSE_SELECT_TOURNAMENT_BUTTON,
#endif
    TIME_UP,  // (this is 0x2be in revisions before K, and 0x2c0 in and after K)
    COURSE_TIME_UP,
    YES_NO_WINDOW,
// [snip]
    DUMMY_ACTOR,
    LASTACTOR_STAGE,
    LASTACTOR
};
```

`src/demo_time_up.cpp`:

```cpp
#include <kamek.h>
#include "profiles.h"

// Spawn a TIME_UP instead of a COURSE_TIME_UP
kmWrite32(0x80926010, 0x38600000 | TIME_UP);

// Have it display upon initialization instead of waiting for something to trigger it
kmWrite32(0x807b35f4, 0x989f0259);
```


## Other tips

* Whenever you add, remove, or rename a source code file, be sure to re-run `configure.py`, because it needs to regenerate the Ninja file describing the project. You might find it helpful to create a personal shell script for this, with your own paths hardcoded.
* To delete all intermediate and output files in order to rebuild everything from scratch (like `make clean` in Make-based projects), you can use `ninja -t clean`.
