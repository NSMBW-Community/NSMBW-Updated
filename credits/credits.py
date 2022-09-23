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


import argparse
from pathlib import Path
import subprocess
import sys
import tempfile
from typing import Set


def fix_caety_sagoian(txt: str, bugs: Set[str]) -> str:
    """
    Voice actress Caety Sagoian's name is misspelled as "Catey Sagoian"
    in P1, E1, J1, P2, E2, J2, P3, J3, and C. This is fixed in K and W.
    """
    if 'P00000' in bugs:
        return txt.replace('Catey Sagoian', 'Caety Sagoian')
    else:
        return txt


def fix_sound_effects(txt: str, bugs: Set[str]) -> str:
    """
    The "SOUND EFFECTS" section is mis-titled as "SOUND EFFECT" in the
    C release. The C staffroll is otherwise identical to the J version,
    indicating that the devs grabbed a very slightly outdated version of
    the v1 credits file for this release.
    """
    if 'P00100' in bugs:
        return txt.replace('<bold>SOUND EFFECT</bold>', '<bold>SOUND EFFECTS</bold>')
    else:
        return txt


def fix_staffroll_txt(txt: str, bugs: Set[str]) -> str:
    """
    Apply fixes to the text-file representation of staffroll.bin.
    """
    txt = fix_caety_sagoian(txt, bugs)
    txt = fix_sound_effects(txt, bugs)
    return txt


def main(argv=None) -> None:
    """
    Main function
    """
    parser = argparse.ArgumentParser(
        description='Apply bugfixes to the credits file (staffroll.bin).')

    parser.add_argument('input_file', type=Path,
        help='input staffroll.bin')
    parser.add_argument('output_file', type=Path,
        help='output staffroll.bin (will be *deleted* instead of written if no modifications are made)')
    parser.add_argument('bugs', nargs='*',
        help='set of bugs to fix')

    args = parser.parse_args(argv)

    here = Path(__file__).parent
    staffroll_tool = here / 'nsmbw-staffroll-tool' / 'staffroll.py'

    with tempfile.TemporaryDirectory() as tempdir:
        temp_dir = Path(tempdir)
        temp_file = temp_dir / 'staffroll.bin'

        # Convert .bin -> .txt
        subprocess.run([
            sys.executable,
            str(staffroll_tool),
            str(args.input_file),
            str(temp_file),
            '--type=bin'])

        # Edit the .txt
        staffroll_txt = temp_file.read_text(encoding='utf-8')
        staffroll_txt_2 = fix_staffroll_txt(staffroll_txt, set(args.bugs))

        if staffroll_txt == staffroll_txt_2:
            args.output_file.unlink(missing_ok=True)

        else:
            temp_file.write_text(staffroll_txt_2, encoding='utf-8')

            # Convert .txt -> .bin
            subprocess.run([
                sys.executable,
                str(staffroll_tool),
                str(temp_file),
                str(args.output_file),
                '--type=txt'])


if __name__ == '__main__':
    main()
