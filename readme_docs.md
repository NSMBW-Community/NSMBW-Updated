# Kamek Ninja Template Documentation

This page provides additional details and clarifications.


## Kamek build mode

Kamek supports two "build modes", `-static` and `-dynamic`, **but this build system only supports "dynamic" mode.** "Static" mode is most often used to build the runtime loader for "dynamic" projects, and since the loader is typically small (only 2 C++ files for *New Super Mario Bros. Wii*) and built separately from the "dynamic" code, it's reasonable to just use simple shell scripts to build it.


## configure.py usage

See `configure.py -h`.


## Project directory

The "project directory", provided to `configure.py` with the `--project-dir` argument, must have all of the following:

* A `src` directory, which will be scanned recursively for C++ and JSON files *(required)*
* An `include` directory, which will be used for local headers (CodeWarrior's lowercase `-i` argument) *(required)*
* `address-map.txt` or `versions.txt`, which is Kamek's "versions file" (Kamek's `-versions` argument) *(optional)*
* `externals.txt`, which is Kamek's "externals file" (Kamek's `-externals` argument) *(optional)*


## JSON files

The `"json_version"` key is required, and currently must be `1`.

As explained in the tutorial, `"builds"` is an object that uses version names for keys and lists of version names for values. Version names are checked against the contents of the address map / versions file, and must satisfy all of the following:

* No invalid/unrecognized versions
* The version used as the key for each group must also be in the group itself
* No versions appear in multiple groups

Note that "every version must be included in some group" is intentionally *not* a rule. Omitting a version will result in the C++ file not being linked into that version's Kamek file at all.

### Searching

JSON metadata can be defined for individual C++ files, or for entire folders. For `/a/b/c/d.cpp`, JSON data will be read from `/a/b/c/d.json`, `/a/b/c.json`, `/a/b.json`, or `/a.json`, whichever one of those exists first (stopping at the source code root directory if no JSON files are found by then).

If no JSON file is found at all, the default is equivalent to `{"json_version": 1}`.


## Preprocessor game version flag semantics

As stated in the tutorial, `IS_GAME_VERSION_DYNAMIC` indicates that the current translation unit should be built in a way compatible with every known game version, even if that requires adding runtime version detection. You should always check for this case when writing game-version preprocessor logic, but you're free to just `#error` if you don't want to implement it. As a suggestion, you may want to follow a policy of implementing it correctly in header files when possible, and not implementing it in source files.

`IS_GAME_VERSION_{version}_COMPATIBLE`, on the other hand, means that you're allowed to assume that the game version at runtime will be `{version}` **or one that's equivalent to it the context relevant to your .cpp file** (the "context" being whatever version difference(s) is/are motivating your use of split compilation in the first place). As the admittedly verbose name tries to indicate, **it does NOT mean that the game version at runtime will necessarily ACTUALLY be `{version}`.** So for example, the following patch **could be incorrect** depending on the JSON file contents:

```cpp
#ifdef IS_GAME_VERSION_P1_COMPATIBLE
// Replace all Goombas with Koopas, at the profile level
kmWrite32(0x8076a814, 0x80afdcb0);
#elif [snip]
[snip]
#endif
```

The value being written (`0x80afdcb0`) is a pointer that's only correct for the "P1" version specifically, which would be incorrect if the JSON file specifies that the "P1" compilation output should be auto-ported to one or more other versions. (Of course, the *actual* right way to implement this particular patch would be to use `kmWritePointer()` instead of any version-specific stuff.)


## Wine

Wine is used to run CodeWarrior on non-Windows systems. This is implemented with the `mwcceppc_wine_wrapper.py` wrapper script, which translates host paths to guest paths in CLI arguments and guest paths to host paths in Makefile outputs ("`.d`" files that define header dependencies).
