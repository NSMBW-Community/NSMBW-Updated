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
import json
from pathlib import Path
import sys
from typing import Any, Dict, List, Optional, Set, Tuple, Union

import db as db_lib
from nsmbw_constants import REGIONS, VERSIONS, LANG_FOLDER_NAMES


PROJECT_SAFE_NAME = 'nsmbw_updated'
PROJECT_DISPLAY_NAME = 'NSMBW Updated'

DEFAULT_VERSION = 'P1'

BUILD_DIR = Path('build')

RIIVO_DISC_ROOT = BUILD_DIR / PROJECT_SAFE_NAME
RIIVO_CONFIG_DIR = BUILD_DIR / 'riivolution'

RIIVO_DISC_CODE = RIIVO_DISC_ROOT / 'Code'
RIIVO_DISC_CODE_LOADER = RIIVO_DISC_CODE / 'loader.bin'
RIIVO_DISC_OBJECT = RIIVO_DISC_ROOT / 'Object'
RIIVO_DISC_STAGE = RIIVO_DISC_ROOT / 'Stage'
RIIVO_DISC_FOLDER_NAMES = ['Code', 'Object', 'Stage', *sorted(set(LANG_FOLDER_NAMES.values()))]

RIIVO_XML = RIIVO_CONFIG_DIR / f'{PROJECT_SAFE_NAME}.xml'

DEFAULT_LOADER_BASE_ADDR = 0x80001900


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


def ninja_escape(thing: Any) -> str:
    """
    Call str() on `thing` (probably a str or Path), and apply
    Ninja-style space escaping
    """
    return str(thing).replace('$', '$$').replace(':', '$:').replace(' ', '$ ')


########################################################################
############################# Config class #############################
########################################################################


@dataclasses.dataclass
class Config:
    """
    Class that represents configuration options provided by the user
    """
    db: db_lib.Database
    kamek_root: Path
    cw_exe: Path
    loader_root: Path
    loader_base_addr: int
    game_roots: Dict[str, Path] = dataclasses.field(default_factory=dict)

    bugfixes_default: bool = True
    bugfixes_default_by_tag: List[Tuple[db_lib.DatabaseEntryTag, bool]] = dataclasses.field(default_factory=list)
    bugfixes_individual: Dict[str, Union[bool, str]] = dataclasses.field(default_factory=dict)

    @staticmethod
    def set_up_arg_parser(db: db_lib.Database, parser: argparse.ArgumentParser) -> None:
        """
        Add arguments to the ArgumentParser
        """
        env_group = parser.add_argument_group('Environment setup')
        env_group.add_argument('--game-root', metavar='DIR', type=Path, action='append',
            help='the path to an extracted game root (with "Effect", "Env", "HomeButton2", etc. subdirectories).'
            ' You can specify this multiple times for different game versions, and the resulting Riivolution patch will include assets for all of them.')
        env_group.add_argument('--kamek-root', type=Path, required=True,
            help='the Kamek directory (with "k_stdlib", "Kamek", etc. subdirectories)')
        env_group.add_argument('--cw', type=Path, metavar='MWCCEPPC', required=True,
            help='CodeWarrior\'s "mwcceppc.exe"')
        env_group.add_argument('--loader-root', metavar='DIR', type=Path, required=True,
            help='the path to the Kamek loader folder (should contain "kamekLoader.cpp" and "nsmbw.cpp")')

        config_group = parser.add_argument_group('Build configuration')
        config_group.add_argument('--loader-base-addr', metavar='ADDR', default=hex(DEFAULT_LOADER_BASE_ADDR),
            help=f'loader base address (default: {DEFAULT_LOADER_BASE_ADDR:#08x})')

        bugs_group = parser.add_argument_group('Bugfix selection')
        bugs_group.add_argument('--default', choices=('on', 'off'), default='on',
            help='whether all bugfixes should be enabled or disabled by default')

        tag_names = (f.name() for f in db_lib.DatabaseEntryTag if f)
        tags_metavar = f'{{{",".join(tag_names)}}}'
        bugs_group.add_argument('--tag-default', nargs=2, action='append',
            metavar=(tags_metavar, '{on,off}'),
            help='whether all bugfixes with a particular tag should be enabled or disabled by default')

        bug_names = sorted(db.entries)
        bug_names = bug_names[:3] + ['...'] + bug_names[-3:]
        bugs_metavar = f'{{{",".join(bug_names)}}}'
        bugs_group.add_argument('--bug', nargs=2, action='append',
            metavar=(bugs_metavar, '{(on|some_bug_specific_option),off}'),
            help='choose an option for a specific bugfix, or disable it')

    @classmethod
    def from_args(cls, db: db_lib.Database, args: argparse.Namespace) -> None:
        """
        Construct an instance from the CLI arguments
        """
        game_roots = {}
        if args.game_root:
            for game_root in args.game_root:
                version = detect_game_version(game_root)
                if version is None:
                    raise ValueError(f"Couldn't identify the game version at {game_root}. Are you sure that's a path to NSMBW?")
                else:
                    print(f'Found {version} NSMBW at {game_root}')
                    game_roots[version] = game_root
        else:
            print('WARNING: No game roots specified -- patched assets will not be built!')

        bugfixes_default = (args.default.lower() == 'on')

        # We intentionally preserve the order of the tag defaults
        bugfixes_default_by_tag = []
        if args.tag_default:
            for instance in args.tag_default:
                tag_str, choice_str = instance

                tag = db_lib.DatabaseEntryTag.from_name(tag_str.upper())
                if tag is None:
                    raise ValueError(f'Unknown tag: {tag_str!r}')
                if choice_str.lower() not in {'on', 'off'}:
                    raise ValueError(f'Tag defaults can only be "on" or "off", not {choice_str!r}')

                choice = (choice_str.lower() == 'on')

                bugfixes_default_by_tag.append((tag, choice))

        bugfixes_individual = {}
        if args.bug:
            for instance in args.bug:
                bug_id, choice_str = instance

                entry = db.entries.get(bug_id.upper())
                if entry is None:
                    raise ValueError(f'Unknown bug ID: {bug_id!r}')

                choice = None
                if choice_str.lower() == 'off':
                    choice = False
                else:
                    if entry.options is None:
                        if choice_str.lower() == 'on':
                            choice = True
                    else:
                        if choice_str.lower() in entry.options:
                            choice = choice_str.lower()

                if choice is None:
                    raise ValueError(f'Illegal option for bug {bug_id}: {choice_str!r}')

                bugfixes_individual[bug_id] = choice

        return cls(
            db,
            args.kamek_root,
            args.cw,
            args.loader_root,
            int(args.loader_base_addr, 16),
            game_roots,
            bugfixes_default,
            bugfixes_default_by_tag,
            bugfixes_individual)

    def get_selected_bugfixes(self) -> Dict[str, Union[bool, str]]:
        """
        Flatten the info from the CLI args into a single dict that lists
        all bugfixes to be applied.
        """
        state = {}

        if self.bugfixes_default:
            for id, entry in self.db.entries.items():
                state[id] = entry.get_default_active_option()

        for tag, choice in self.bugfixes_default_by_tag:
            for id, entry in self.db.entries.items():
                if choice:
                    state[id] = entry.get_default_active_option()
                else:
                    state.pop(id, None)

        for id, choice in self.bugfixes_individual.items():
            if choice:
                state[id] = choice
            else:
                state.pop(id, None)

        return state

    def get_kamek_executable(self) -> Path:
        """
        Return the path to the Kamek executable
        """
        # TODO: make this better somehow...?
        bin_folder = self.kamek_root / 'Kamek' / 'bin'
        if not bin_folder.is_dir():
            raise RuntimeError(f'{bin_folder} not found. Please build Kamek itself first!')

        if sys.platform == 'win32':
            return bin_folder / 'Debug' / 'net6.0' / 'Kamek.exe'
        else:
            return bin_folder / 'Debug' / 'net6.0' / 'Kamek'

    def get_kstdlib_dir(self) -> Path:
        """
        Return the path to the k_stdlib dir
        """
        return self.kamek_root / 'k_stdlib'

    def get_loader_xml_path(self) -> Path:
        """
        Return the path to loader.xml. This is an intermediate output,
        so the exact choice here doesn't matter as long as we're
        consistent.
        """
        return self.loader_root / 'loader.xml'


########################################################################
################################# Code #################################
########################################################################


CODE_BUILD_VERSIONS = ['P1', 'E1', 'J1', 'P2', 'E2', 'J2', 'K', 'W']

CODE_ROOT_DIR = Path('code')

ADDRESS_MAP_TXT = CODE_ROOT_DIR / 'address-map.txt'

CODE_SRC_DIR = CODE_ROOT_DIR / 'src'
CODE_INCLUDE_DIR = CODE_ROOT_DIR / 'include'


@dataclasses.dataclass
class TranslationUnit:
    """
    Represents one translation unit (.cpp file)
    """
    cpp_file: Path

    # Sometimes we need different builds for different game versions
    # (such as when actor IDs are referenced in the code), but often we
    # can just build for P1 and reuse it everywhere. This dict defines
    # that relationship.
    builds: Dict[str, str] = dataclasses.field(
        default_factory=lambda: {v: DEFAULT_VERSION for v in CODE_BUILD_VERSIONS})

    def read_config(self, path: Path) -> None:
        """
        Read additional config data from an optional .json file
        """
        if not path.is_file():
            return
        with path.open(encoding='utf-8') as f:
            j = json.load(f)

        if builds_list := j.get('builds'):
            # Invert it
            self.builds = {}
            for build, users in builds_list.items():
                for user in users:
                    self.builds[user] = build

            if set(self.builds) != set(CODE_BUILD_VERSIONS):
                raise ValueError(f"{path.name} doesn't specify the right set of build versions:"
                    f' {sorted(self.builds)} vs {sorted(CODE_BUILD_VERSIONS)}')

    def iter_builds(self):
        """
        Iterator over the .o files that need to be built for this TU.
        Yields (version string, .o file path) pairs.
        """
        for version in set(self.builds.values()):
            yield version, self.o_file_for_version(version)

    def o_file_for_version(self, version: str) -> Path:
        """
        Return the .o file that should be linked for the specified game
        version.
        """
        if len(set(self.builds.values())) == 1:
            # No need to mangle the version name into the .o filename
            return self.cpp_file.with_suffix('.o')
        else:
            return self.cpp_file.parent / (self.cpp_file.stem + f'_{self.builds[version]}.o')


def make_code_rules(config: Config) -> str:
    """
    Create Ninja rules to build the .bin files in the Code directory
    """
    # Find all TUs, and read any configs
    tus = []
    for fp in sorted(CODE_SRC_DIR.glob('**/*.cpp')):
        tu = TranslationUnit(fp)
        tu.read_config(fp.with_suffix('.json'))
        tus.append(tu)

    use_wine = (sys.platform == 'win32')

    bug_flags = set()
    for id, choice in config.get_selected_bugfixes().items():
        bug_flags.add(id)
        if isinstance(choice, str):
            bug_flags.add(f'{id}_{choice.upper()}')

    lines = [f"""
mwcceppc = {config.cw_exe}
cc = {'' if use_wine else 'wine '}$mwcceppc
kamek = {config.get_kamek_executable()}
kstdlib = {config.get_kstdlib_dir()}
addrmap = {ADDRESS_MAP_TXT}
includedir = {CODE_INCLUDE_DIR}
loaderdir = {config.loader_root}
loaderaddr = {config.loader_base_addr:#08x}

cflags = $
  -I- $
  -i $kstdlib $
  -Cpp_exceptions off $
  -enum int $
  -O4,s $
  -use_lmw_stmw on $
  -fp hard $
  -rostr $
  -sdata 0 $
  -sdata2 0 $
  -RTTI off

cflags_P1 = -DVERSION_P1 -DREGION_P -DIS_V1      -DIS_PRE_V2  -DIS_PRE_K  -DIS_PRE_W  -DIS_PRE_C
cflags_E1 = -DVERSION_E1 -DREGION_E -DIS_V1      -DIS_PRE_V2  -DIS_PRE_K  -DIS_PRE_W  -DIS_PRE_C
cflags_J1 = -DVERSION_J1 -DREGION_J -DIS_V1      -DIS_PRE_V2  -DIS_PRE_K  -DIS_PRE_W  -DIS_PRE_C
cflags_P2 = -DVERSION_P2 -DREGION_P -DIS_POST_V1 -DIS_V2      -DIS_PRE_K  -DIS_PRE_W  -DIS_PRE_C
cflags_E2 = -DVERSION_E2 -DREGION_E -DIS_POST_V1 -DIS_V2      -DIS_PRE_K  -DIS_PRE_W  -DIS_PRE_C
cflags_J2 = -DVERSION_J2 -DREGION_J -DIS_POST_V1 -DIS_V2      -DIS_PRE_K  -DIS_PRE_W  -DIS_PRE_C
cflags_K  = -DVERSION_K  -DREGION_K -DIS_POST_V1 -DIS_POST_V2             -DIS_PRE_W  -DIS_PRE_C
cflags_W  = -DVERSION_W  -DREGION_W -DIS_POST_V1 -DIS_POST_V2 -DIS_POST_K             -DIS_PRE_C
cflags_C  = -DVERSION_C  -DREGION_C -DIS_POST_V1 -DIS_POST_V2 -DIS_POST_K -DIS_POST_W

cflags_bugs = {' '.join(f'-D{v}' for v in sorted(bug_flags))}

cflags_proj_include = -i $includedir
cflags_loader_include = -i $loaderdir

rule cw
  command = $cc $cflags -c -o $out $in
  description = $cc $in
"""]

    # ------------------------------------------------------------------
    # Project code files

    # Add "cw" edges for all .cpp -> .o files
    for tu in tus:
        for version, o_file in tu.iter_builds():
            lines.append('build'
                f' {ninja_escape(o_file)}:'
                f' cw {ninja_escape(tu.cpp_file)}')
            lines.append(f'  cflags = $cflags $cflags_proj_include $cflags_{version} $cflags_bugs')
    lines.append('')

    lines.append(f"""
rule kmdynamic
  command = $kamek $in -dynamic -versions=$addrmap -output-kamek=$out -select-version=$selectversion
  description = $kamek -> $out
""")

    # Add "km" edges for all .o -> .bin files
    for version in CODE_BUILD_VERSIONS:
        lines.append('build')
        out_fp = RIIVO_DISC_CODE / f'{version}.bin'
        lines[-1] += f' $builddir/{ninja_escape(out_fp.relative_to(BUILD_DIR))}'

        lines[-1] += ': kmdynamic'
        for tu in tus:
            o_file = tu.o_file_for_version(version)
            lines[-1] += f' {ninja_escape(o_file)}'
        lines.append(f'  selectversion = {version}')
        lines.append('')

    # ------------------------------------------------------------------
    # Loader

    cpp_files = set(config.loader_root.glob('*.cpp'))

    # Add "cw" edges for all .cpp -> .o files
    for cpp_file in cpp_files:
        # sanity check
        if cpp_file.name not in {'kamekLoader.cpp', 'nsmbw.cpp'}:
            print(f'WARNING: Unexpected loader cpp file "{cpp_file}"')

        o_file = cpp_file.with_suffix('.o')
        lines.append('build'
            f' {ninja_escape(o_file)}:'
            f' cw {ninja_escape(cpp_file)}')
        lines.append('  cflags = $cflags $cflags_loader_include')

    lines.append(f"""
rule kmstatic
  command = $kamek $in -static=$baseaddr -output-riiv=$outxml -output-code=$outbin
  description = $kamek -> $outxml + $outbin
""")

    # Add a "km" edge for loader.bin
    lines.append(f'build {ninja_escape(config.get_loader_xml_path())} {ninja_escape(RIIVO_DISC_CODE_LOADER)}: kmstatic')
    for cpp_file in cpp_files:
        o_file = cpp_file.with_suffix('.o')
        lines[-1] += f' {ninja_escape(o_file)}'
    lines.append(f'  outxml = {ninja_escape(config.get_loader_xml_path())}')
    lines.append(f'  outbin = {ninja_escape(RIIVO_DISC_CODE_LOADER)}')
    lines.append(f'  baseaddr = $loaderaddr')
    
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
  command = $py {CREDITS_PY} $in $out $bugs
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
            lines.append(f'build {ninja_escape(target_dir)}: credits {ninja_escape(staffroll)}')

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
  command = $py {CREATE_RIIVOLUTION_XML_PY} $out /{PROJECT_SAFE_NAME}"""]

    lines[-1] += f' --regions={",".join(REGIONS)}'
    lines[-1] += f" --title='{PROJECT_DISPLAY_NAME}'"
    lines[-1] += f" --loader-xml='$in'"
    lines[-1] += f" --loader-bin='{RIIVO_DISC_CODE_LOADER.relative_to(RIIVO_DISC_ROOT)}'"

    for folder in RIIVO_DISC_FOLDER_NAMES:
        lines[-1] += f' --folder={folder},/{folder}'

    lines.append('  description = Generating Riivolution XML...')
    lines.append('')

    lines.append(f'build $builddir/{RIIVO_XML.relative_to(BUILD_DIR)}: riixml {ninja_escape(config.get_loader_xml_path())}')
    
    return '\n'.join(lines)


########################################################################
################################ General ###############################
########################################################################

NINJA_FILE = Path('build.ninja')


def make_ninja_file(config: Config) -> str:
    """
    Make the overall Ninja file
    """
    bug_items = []
    for k, v in config.get_selected_bugfixes().items():
        if isinstance(v, str):
            bug_items.append(f'{k}={v}')
        else:
            bug_items.append(k)

    txt = f"""# NOTE: This file is generated by {Path(__file__).name}.

py = {sys.executable}

# NOTE: "builddir" has special significance to Ninja (see the manual)
builddir = {BUILD_DIR}

bugs = {' '.join(sorted(bug_items))}

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

    db = db_lib.Database.load_from_default_file()

    parser = argparse.ArgumentParser(
        description=f'Configure {PROJECT_DISPLAY_NAME} to prepare for building.')
    Config.set_up_arg_parser(db, parser)

    args = parser.parse_args(argv)
    config = Config.from_args(db, args)

    NINJA_FILE.write_text(make_ninja_file(config), encoding='utf-8')
    print(f'Wrote {NINJA_FILE}')


if __name__ == '__main__':
    main()
