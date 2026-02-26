#!/usr/bin/env python3

# MIT License
#
# Copyright (c) 2022 RoadrunnerWMC
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

from pathlib import Path
import sys
import subprocess
from typing import Any


# Note 1: "-I" is special-cased.
# Note 2: I'm ignoring "-instmgr" and "-instance_manager" because it's
# an obscure feature with weird CLI syntax, and probably nobody uses it.
ARGS_INDICATING_MAKEFILES = {'-Mfile', '-MMfile', '-MDfile', '-MMDfile'}
ARGS_INDICATING_PATHS = {'-i', '-include', '-ir', *ARGS_INDICATING_MAKEFILES, '-o', '-precompile', '-prefix'}


def is_windows():
    return sys.platform == 'win32'


def makefile_escape(thing: Any) -> str:
    """
    Call str() on `thing` (probably a str or Path), and apply
    Makefile-style space escaping
    """
    return str(thing).replace('\\', '\\\\').replace(' ', '\\ ')


def makefile_unescape(thing: str) -> str:
    """
    Remove Makefile-style escaping from a string
    """
    return str(thing).replace('\\ ', ' ').replace('\\\\', '\\')


def host_to_guest(path: Path) -> str:
    """
    Convert a host path to a string CodeWarrior will be happy with
    """
    if is_windows():
        return str(path.resolve())
    else:
        return subprocess.check_output(
            ['winepath', '-w', str(path)],
            encoding='utf-8',
            stderr=subprocess.DEVNULL,
        ).rstrip('\n')


def guest_to_host(path_str: str) -> Path:
    """
    Convert a path string from CodeWarrior to a path for the host system
    """
    if is_windows():
        return Path(path_str)
    else:
        return Path(subprocess.check_output(
            ['winepath', '-u', path_str],
            encoding='utf-8',
            stderr=subprocess.DEVNULL,
        ).rstrip('\n'))


def fix_makefile(text: str, filename: str) -> str:
    """
    Convert all Windows paths to Unix paths within a
    CodeWarrior-generated Makefile
    """
    new_lines = []

    for i, line in enumerate(text.splitlines()):
        if not line:
            # Blank line
            new_lines.append('')

        elif line.count(': ') == 1:
            split_point = line.index(': ')

            ends_with_slash = line.endswith(' \\')
            if ends_with_slash:
                line = line[:-2]
            line = line.rstrip()

            o_file_guest = makefile_unescape(line[:split_point])
            cpp_file_guest = makefile_unescape(line[split_point + 2 :])

            o_file_host = guest_to_host(o_file_guest).resolve()
            cpp_file_host = guest_to_host(cpp_file_guest).resolve()

            SPACE_BACKSLASH = ' \\'
            new_lines.append(f'{makefile_escape(o_file_host)}: {makefile_escape(cpp_file_host)}{SPACE_BACKSLASH if ends_with_slash else ""}')

        elif line.startswith('\t'):
            # Continuation of a rule ("\tsome_header.h \\")
            line = line[1:]

            ends_with_slash = line.endswith(' \\')
            if ends_with_slash:
                line = line[:-2]
            line = line.rstrip()

            h_file_guest = makefile_unescape(line)
            h_file_host = guest_to_host(h_file_guest).resolve()
            SPACE_BACKSLASH = ' \\'
            new_lines.append(f'\t{makefile_escape(h_file_host)}{SPACE_BACKSLASH if ends_with_slash else ""}')

        else:
            raise ValueError(f"Couldn't parse line {i+1} of {filename}: {line!r}")

    return '\n'.join(new_lines)


def main(argv=None):
    if argv is None:
        argv = sys.argv

    if len(argv) < 2:
        print(f'usage: {argv[0]} /path/to/(mwcceppc.exe or mwasmeppc.exe) [arguments to CodeWarrior, using host filepaths]...')
        return

    # Ignore this Python script's own filename
    argv.pop(0)

    # First argument is the path to CodeWarrior
    cw_exe = Path(argv.pop(0))

    # Next, we scan for any arguments we recognize, and translate any
    # paths
    makefile_paths = []
    for i, arg in enumerate(argv):
        prev_arg = argv[i - 1] if i > 0 else None

        # Arguments that precede paths, e.g. "-i", "-MDfile", ...
        if prev_arg in ARGS_INDICATING_PATHS:
            if prev_arg in ARGS_INDICATING_MAKEFILES:
                makefile_paths.append(Path(arg))

            argv[i] = host_to_guest(Path(arg))

        # I hate this, but I don't know how else to detect the .cpp, .c,
        # .S, etc. input file argument(s)
        elif Path(arg).is_file():
            argv[i] = host_to_guest(Path(arg))

        # And a special case for "-I/path/to/include/dir"
        elif arg.startswith('-I') and arg != '-I-':
            argv[i] = '-I' + host_to_guest(Path(arg[2:]))

    # Time to invoke CodeWarrior!
    cmd = [cw_exe, *argv]
    if not is_windows():
        cmd.insert(0, 'wine')
    proc = subprocess.run(cmd)
    if proc.returncode != 0:
        exit(proc.returncode)

    # And now fix up any Makefiles it may have generated
    for path in makefile_paths:
        makefile_txt = path.read_text(encoding='utf-8')
        makefile_txt = fix_makefile(makefile_txt, path.name)
        path.write_text(makefile_txt, encoding='utf-8')


if __name__ == '__main__':
    main()
