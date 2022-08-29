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
from typing import List


PROJECT_SAFE_NAME = 'nsmbw_updated'
PROJECT_DISPLAY_NAME = 'NSMBW Updated'
REGIONS = ['P', 'E', 'J']
VERSIONS = ['P1', 'E1', 'J1', 'P2', 'E2', 'J2']


BUILD_DIR = Path('build')

RIIVO_DISC_ROOT = BUILD_DIR / PROJECT_SAFE_NAME
RIIVO_CONFIG_DIR = BUILD_DIR / 'riivolution'

RIIVO_DISC_CODE = RIIVO_DISC_ROOT / 'Code'
RIIVO_DISC_OBJECT = RIIVO_DISC_ROOT / 'Object'
RIIVO_DISC_STAGE = RIIVO_DISC_ROOT / 'Stage'
RIIVO_DISC_FOLDER_NAMES = ['Code', 'Object', 'Stage']

RIIVO_XML = RIIVO_CONFIG_DIR / f'{PROJECT_SAFE_NAME}.xml'


########################################################################
########################### Utility functions ##########################
########################################################################


def remove_chars_from_last_line_and_add(lines: List[str], num_chars: int, add: str = '') -> None:
    """
    Remove `num_chars` characters from the last string in the list, and
    append another string to it.
    """
    lines[-1] = lines[-1][:-num_chars] + add


def remove_chars_from_last_line(lines: List[str], num_chars: int) -> None:
    """
    Alias for remove_chars_from_last_line_and_add() without the "add"
    functionality, and with a simpler name as a result.
    """
    remove_chars_from_last_line_and_add(lines, num_chars)


########################################################################
################################# Code #################################
########################################################################


CODE_ROOT_DIR = Path('code')
KAMEK_ROOT_DIR = Path('Kamek')

CODEWARRIOR_EXE = KAMEK_ROOT_DIR / 'cw' / 'mwcceppc.exe'
KAMEK_EXE = KAMEK_ROOT_DIR / 'Kamek' / 'bin' / 'Debug' / 'net6.0' / 'Kamek'
KSTDLIB_DIR = KAMEK_ROOT_DIR / 'k_stdlib'

ADDRESS_MAP_TXT = CODE_ROOT_DIR / 'address-map.txt'

CODE_SRC_DIR = CODE_ROOT_DIR / 'src'
CODE_INCLUDE_DIR = CODE_ROOT_DIR / 'include'


def make_code_rules() -> str:
    """
    Create Ninja rules to build the .bin files in the Code directory
    """
    src_files = sorted(CODE_SRC_DIR.glob('**/*.cpp'))
    use_wine = (sys.platform == 'win32')

    lines = [f"""
mwcceppc = {CODEWARRIOR_EXE}
cc = {'' if use_wine else 'wine '}$mwcceppc
kamek = {KAMEK_EXE}
kstdlib = {KSTDLIB_DIR}
addrmap = {ADDRESS_MAP_TXT}
includedir = {CODE_INCLUDE_DIR}

cflags = $
  -I- $
  -i $kstdlib $
  -i $includedir $
  -Cpp_exceptions off $
  -enum int $
  -O4,s $
  -use_lmw_stmw on $
  -fp hard $
  -rostr $
  -sdata 0 $
  -sdata2 0 $
  -RTTI off

rule cw
  command = $cc $cflags -c -o $out $in
  description = $cc $in

rule km
  command = $kamek $in -dynamic -versions=$addrmap $
    '-output-kamek=$builddir/{RIIVO_DISC_CODE.relative_to(BUILD_DIR)}/$$KV$$.bin'
  description = Linking all outputs with $kamek ...
"""]

    # Add "cw" edges for all .cpp -> .o files.
    for fp in src_files:
        lines.append('build'
            f' {fp.with_suffix(".o")}: $\n'
            f'  cw {fp}')
    lines.append('')

    # Add a "km" edge for the final linking step.
    # This line is a bit of a cheat. Ninja needs to know that the
    # linking outputs are P1.bin, E1.bin, etc, but Kamek derives those
    # filenames from the address map rather than the command line. So we
    # write this build edge without using "$out" anywhere at all.
    lines.append('build $')
    for version in VERSIONS:
        out_fp = RIIVO_DISC_CODE / f'{version}.bin'
        lines.append(f'    $builddir/{out_fp.relative_to(BUILD_DIR)} $')
    remove_chars_from_last_line_and_add(lines, 2, ': $')

    lines.append('  km $')
    for fp in src_files:
        lines.append(f'    {fp.with_suffix(".o")} $')
    remove_chars_from_last_line(lines, 2)

    lines.append('')
    
    return '\n'.join(lines)


########################################################################
############################ Riivolution XML ###########################
########################################################################


CREATE_RIIVOLUTION_XML_PY = Path('create_riivolution_xml.py')

def make_riivolution_xml_rules() -> str:
    """
    Create Ninja rules to build the Riivolution XML
    """
    lines = [f"""
rule riixml
  command = $py {CREATE_RIIVOLUTION_XML_PY} $
    $out $
    /{PROJECT_SAFE_NAME} $
    {','.join(REGIONS)} $
    '{PROJECT_DISPLAY_NAME}' $"""]

    for folder in RIIVO_DISC_FOLDER_NAMES:
        lines.append(f'    --folder={folder},/{folder} $')
    remove_chars_from_last_line(lines, 2)

    lines.append('  description = Generating Riivolution XML...')
    lines.append('')

    lines.append(f'build $builddir/{RIIVO_XML.relative_to(BUILD_DIR)}: riixml')
    
    return '\n'.join(lines)


########################################################################
################################ General ###############################
########################################################################

NINJA_FILE = Path('build.ninja')


def make_ninja_file() -> str:
    """
    Make the overall Ninja file
    """
    return f"""# NOTE: This file is generated by {Path(__file__).name}.

py = {sys.executable}

# NOTE: "builddir" has special significance to Ninja (see the manual)
builddir = {BUILD_DIR}

{make_code_rules()}
{make_riivolution_xml_rules()}
"""


def main() -> None:
    NINJA_FILE.write_text(make_ninja_file(), encoding='utf-8')
    print(f'Wrote {NINJA_FILE}')


if __name__ == '__main__':
    main()
