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
import dataclasses
from pathlib import Path
import sys
from typing import Any, Dict, Optional


PROJECT_SAFE_NAME = 'nsmbw_updated'
PROJECT_DISPLAY_NAME = 'NSMBW Updated'
REGIONS = ['P', 'E', 'J']
VERSIONS = ['P1', 'E1', 'J1', 'P2', 'E2', 'J2']
LANG_FOLDER_NAMES = {
    'P1': 'EU',
    'E1': 'US',
    'J1': 'JP',
    'P2': 'EU',
    'E2': 'US',
    'J2': 'JP',
    'P3': 'EU',
    'J3': 'JP',
    'K': 'KR',
    'W': 'TW',
    'C': 'CN',
}


BUILD_DIR = Path('build')

RIIVO_DISC_ROOT = BUILD_DIR / PROJECT_SAFE_NAME
RIIVO_CONFIG_DIR = BUILD_DIR / 'riivolution'

RIIVO_DISC_CODE = RIIVO_DISC_ROOT / 'Code'
RIIVO_DISC_OBJECT = RIIVO_DISC_ROOT / 'Object'
RIIVO_DISC_STAGE = RIIVO_DISC_ROOT / 'Stage'
RIIVO_DISC_FOLDER_NAMES = ['Code', 'Object', 'Stage', *sorted(set(LANG_FOLDER_NAMES.values()))]

RIIVO_XML = RIIVO_CONFIG_DIR / f'{PROJECT_SAFE_NAME}.xml'


########################################################################
########################### Utility functions ##########################
########################################################################


def detect_game_version(root: Path) -> Optional[str]:
    """
    Detect the game version at the specified path.
    """
    # TODO: P3 and J3 can't be detected from just the disc root...

    if (root / 'COPYDATE_CODE_2009-10-03_232911').is_file():
        return 'P1'
    elif (root / 'COPYDATE_CODE_2009-10-03_232303').is_file():
        return 'E1'
    elif (root / 'COPYDATE_CODE_2009-10-03_231655').is_file():
        return 'J1'
    elif (root / 'COPYDATE_CODE_2010-01-05_152101').is_file():
        return 'P2'
    elif (root / 'COPYDATE_CODE_2010-01-05_143554').is_file():
        return 'E2'
    elif (root / 'COPYDATE_CODE_2010-01-05_160530').is_file():
        return 'J2'
    elif (root / 'COPYDATE_CODE_2010-03-12_153510').is_file():
        return 'K'
    elif (root / 'COPYDATE_CODE_2010-03-15_160349').is_file():
        return 'W'
    elif (root / 'COPYDATE_CODE_2016-05-03_111248').is_file():
        return 'C'


def ninja_escape_spaces(thing: Any) -> str:
    """
    Call str() on `thing` (probably a str or Path), and apply
    Ninja-style space escaping
    """
    return str(thing).replace(' ', '$ ')


########################################################################
############################# Config class #############################
########################################################################


@dataclasses.dataclass
class Config:
    """
    Class that represents configuration options provided by the user
    """
    game_roots: Dict[str, Path] = dataclasses.field(default_factory=dict)

    def set_up_arg_parser(self, parser: argparse.ArgumentParser) -> None:
        """
        Add arguments to the ArgumentParser
        """
        parser.add_argument('--game-root', type=Path, action='append',
            help='the path to an extracted game root (i.e. with the Effect, Env, HomeButton2, etc. subdirectories).'
            ' You can specify this multiple times for different game versions, and the resulting Riivolution patch will include assets for all of them.')

        # TODO: let the user configure
        # - path to CodeWarrior
        # - path to Kamek

    def read_args(self, args: argparse.Namespace) -> None:
        """
        Figure out the configuration data from the CLI arguments
        """
        if args.game_root:
            for game_root in args.game_root:
                version = detect_game_version(game_root)
                if version is None:
                    raise ValueError(f"Couldn't identify the game version at {game_root}. Are you sure that's a path to NSMBW?")
                else:
                    print(f'Found {version} NSMBW at {game_root}')
                    self.game_roots[version] = game_root
        else:
            print('WARNING: No game roots specified -- patched assets will not be built!')


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


def make_code_rules(config: Config) -> str:
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
"""]

    # Add "cw" edges for all .cpp -> .o files.
    for fp in src_files:
        lines.append('build'
            f' {ninja_escape_spaces(fp.with_suffix(".o"))}:'
            f' cw {ninja_escape_spaces(fp)}')
    lines.append('')

    lines.append(f"""
rule km
  command = $kamek $in -dynamic -versions=$addrmap '-output-kamek=$builddir/{RIIVO_DISC_CODE.relative_to(BUILD_DIR)}/$$KV$$.bin'
  description = Linking all outputs with $kamek ...
""")

    # Add a "km" edge for the final linking step.
    # This line is a bit of a cheat. Ninja needs to know that the
    # linking outputs are P1.bin, E1.bin, etc, but Kamek derives those
    # filenames from the address map rather than the command line. So we
    # write this build edge without using "$out" anywhere at all.
    lines.append('build')
    for version in VERSIONS:
        out_fp = RIIVO_DISC_CODE / f'{version}.bin'
        lines[-1] += f' $builddir/{ninja_escape_spaces(out_fp.relative_to(BUILD_DIR))}'

    lines[-1] += ' km'
    for fp in src_files:
        lines[-1] += f' {ninja_escape_spaces(fp.with_suffix(".o"))}'

    lines.append('')
    
    return '\n'.join(lines)


########################################################################
################################ Credits ###############################
########################################################################


CREDITS_PY = Path('credits/credits.py')


def make_credits_rules(config: Config) -> str:
    """
    Create Ninja rules to update the credits
    """
    if not config.game_roots:
        return ''

    lines = [f"""
rule credits
  command = $py {CREDITS_PY} $in $out
  description = Editing credits...
"""]

    already_covered_staffrolls = set()

    for version, root in config.game_roots.items():
        lang_folder = root / LANG_FOLDER_NAMES[version]

        # Find all staffroll.bin's
        staffrolls = []
        for possible_parent in [lang_folder, *lang_folder.iterdir()]:
            staffroll_fp = possible_parent / 'staffroll' / 'staffroll.bin'
            if staffroll_fp.is_file():
                staffrolls.append(staffroll_fp)

        if not staffrolls:
            raise ValueError(f"Couldn't find a single staffroll.bin in {lang_folder}...")

        # Add build edges
        for staffroll in staffrolls:
            staffroll_relative = staffroll.relative_to(root)

            if staffroll_relative in already_covered_staffrolls:
                continue

            target_dir = f'$builddir/{(RIIVO_DISC_ROOT / staffroll_relative).relative_to(BUILD_DIR)}'
            lines.append(f'build {ninja_escape_spaces(target_dir)}: credits {ninja_escape_spaces(staffroll)}')

            already_covered_staffrolls.add(staffroll_relative)

    return '\n'.join(lines)


########################################################################
############################ Riivolution XML ###########################
########################################################################


CREATE_RIIVOLUTION_XML_PY = Path('create_riivolution_xml.py')

def make_riivolution_xml_rules(config: Config) -> str:
    """
    Create Ninja rules to build the Riivolution XML
    """
    lines = [f"""
rule riixml
  command = $py {CREATE_RIIVOLUTION_XML_PY} $out /{PROJECT_SAFE_NAME} {','.join(REGIONS)} '{PROJECT_DISPLAY_NAME}'"""]

    for folder in RIIVO_DISC_FOLDER_NAMES:
        lines[-1] += f' --folder={folder},/{folder}'

    lines.append('  description = Generating Riivolution XML...')
    lines.append('')

    lines.append(f'build $builddir/{RIIVO_XML.relative_to(BUILD_DIR)}: riixml')
    
    return '\n'.join(lines)


########################################################################
################################ General ###############################
########################################################################

NINJA_FILE = Path('build.ninja')


def make_ninja_file(config: Config) -> str:
    """
    Make the overall Ninja file
    """
    txt = f"""# NOTE: This file is generated by {Path(__file__).name}.

py = {sys.executable}

# NOTE: "builddir" has special significance to Ninja (see the manual)
builddir = {BUILD_DIR}

{make_code_rules(config)}
{make_credits_rules(config)}
{make_riivolution_xml_rules(config)}
"""

    while '\n\n\n' in txt:
        txt = txt.replace('\n\n\n', '\n\n')

    return txt


def main(argv=None) -> None:
    """
    Main function
    """

    config = Config()

    parser = argparse.ArgumentParser(
        description=f'Configure {PROJECT_DISPLAY_NAME} to prepare for building.')
    config.set_up_arg_parser(parser)

    args = parser.parse_args(argv)
    config.read_args(args)

    NINJA_FILE.write_text(make_ninja_file(config), encoding='utf-8')
    print(f'Wrote {NINJA_FILE}')


if __name__ == '__main__':
    main()
