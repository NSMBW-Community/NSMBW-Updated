# Kamek Ninja Template

This repo provides a modern build system / project template for large-scale [Kamek](https://github.com/Treeki/Kamek) projects, powered by [Python](https://www.python.org) and [Ninja](https://ninja-build.org).

Kamek is a great tool for injecting custom code into Wii games, but it's not a full build system, just a linker and a small runtime loader. Shell scripts (like the examples included with Kamek) work well enough for small projects, but larger projects can benefit greatly from features like parallel and incremental builds.

This repo was spun off from the [NSMBW-Updated](https://github.com/NSMBW-Community/NSMBW-Updated) project, after it became clear how useful its build system was and how much other projects could benefit from it.


## Overview

Prerequisites:

* A recent version of Python 3
* [Kamek](https://github.com/Treeki/Kamek) and the appropriate version of CodeWarrior for it (follow the download/setup instructions included with Kamek)
* [Ninja](https://ninja-build.org)
* If you're on a non-Windows system, [Wine](https://www.winehq.org/) is needed to run CodeWarrior.

The high-level workflow looks like this:

0. Build Kamek and get a copy of CodeWarrior, if you haven't already. (See Kamek's documentation for details.)
1. Run configure.py to generate build.ninja.
2. Run `ninja` to build your Kamek files.


## Documentation

[Click here for a step-by-step usage tutorial](readme_tutorial.md), or [click here for more detailed documentation.](readme_docs.md)


## License

MIT license. See [license.txt](license.txt) for details.
