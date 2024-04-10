#!/usr/bin/env python3

# MIT License
#
# Copyright (c) 2022, 2024 RoadrunnerWMC
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
from typing import Any, Dict, List, Optional, Tuple, Union

import db as db_lib
from nsmbw_constants import LANG_FOLDER_NAMES


PROJECT_SAFE_NAME = 'nsmbw_updated'
PROJECT_DISPLAY_NAME = 'NSMBW Updated'

BUILD_DIR = Path('_build')
OUTPUT_DIR = Path('bin')

RIIVO_DISC_ROOT = OUTPUT_DIR / PROJECT_SAFE_NAME
RIIVO_CONFIG_DIR = OUTPUT_DIR / 'riivolution'

RIIVO_DISC_CODE = RIIVO_DISC_ROOT / 'Code'
RIIVO_DISC_CODE_LOADER = RIIVO_DISC_CODE / 'loader.bin'
RIIVO_DISC_OBJECT = RIIVO_DISC_ROOT / 'Object'
RIIVO_DISC_STAGE = RIIVO_DISC_ROOT / 'Stage'

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


class Config:
    """
    Class that represents configuration options provided by the user
    """
    db: db_lib.Database
    kamek_exe: Path
    kstdlib_dir: Path
    cw_exe: Path
    loader_dir: Path
    loader_base_addr: int
    game_roots: Dict[str, Path]

    bugfixes_default: bool
    bugfixes_default_by_tag: List[Tuple[db_lib.DatabaseEntryTag, bool]]
    bugfixes_individual: Dict[str, Union[bool, str]]

    @staticmethod
    def set_up_arg_parser(db: db_lib.Database, parser: argparse.ArgumentParser) -> None:
        """
        Add arguments to the ArgumentParser
        """
        env_group = parser.add_argument_group('Specifying the environment')
        env_group.add_argument('--game-root', metavar='DIR', type=Path, action='append',
            help='the path to an extracted game root (with "Effect", "Env", "HomeButton2", etc. subdirectories).'
            ' You can specify this multiple times for different game versions, and the resulting Riivolution patch will include assets for all of them.')
        env_group.add_argument('--kamek', type=Path, required=True,
            help='the Kamek binary ("Kamek.exe" on Windows, "Kamek" on other platforms)')
        env_group.add_argument('--kstdlib', type=Path, required=True,
            help='Kamek\'s "k_stdlib" directory')
        env_group.add_argument('--cw', type=Path, metavar='MWCCEPPC', required=True,
            help='CodeWarrior\'s "mwcceppc.exe"')
        env_group.add_argument('--loader-dir', metavar='DIR', type=Path, required=True,
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

        self = cls()
        self.db = db
        self.kamek_exe = args.kamek.resolve()
        self.kstdlib_dir = args.kstdlib.resolve()
        self.cw_exe = args.cw.resolve()
        self.loader_dir = args.loader_dir.resolve()
        self.loader_base_addr = int(args.loader_base_addr, 16)
        self.game_roots = {k: v.resolve() for k, v in game_roots.items()}
        self.bugfixes_default = bugfixes_default
        self.bugfixes_default_by_tag = bugfixes_default_by_tag
        self.bugfixes_individual = bugfixes_individual
        return self

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

    def get_xml_template_path(self) -> Path:
        # This is an intermediate output path, so the exact choice here
        # doesn't matter as long as we're consistent.
        return BUILD_DIR / 'riivo_template.xml'


########################################################################
################################# Code #################################
########################################################################


CODE_ROOT_DIR = Path('code')
CODE_NINJA_FILE = CODE_ROOT_DIR / 'build.ninja'
CODE_TEMPLATE_REPO_DIR = CODE_ROOT_DIR / 'Kamek-Ninja-Template'
CODE_CONFIGURE_SCRIPT = CODE_TEMPLATE_REPO_DIR / 'configure.py'
CODE_CW_WINE_WRAPPER = CODE_TEMPLATE_REPO_DIR / 'mwcceppc_wine_wrapper.py'


def make_code_rules(config: Config) -> str:
    """
    Create Ninja rules to build the .bin files in the Code directory
    """

    lines = []

    # ------------------------------------------------------------------
    # Project code files: built using the project template; we don't
    # actually touch CodeWarrior or Kamek ourselves

    # The goal here is to detect the following cases:
    # - Bug is unselected: add "BUGNAME_OFF" flag
    # - Bug is selected (True): the default behavior, no flag needed
    # - Bug is selected (some other str): add "BUGNAME_CHOICENAME" flag
    selected_bugfixes = config.get_selected_bugfixes()
    bug_flags = set()
    for id in config.db.entries.keys():
        choice = selected_bugfixes.get(id, False)
        if isinstance(choice, bool):
            if not choice:
                bug_flags.add(f'{id}_OFF')
        else:
            bug_flags.add(f'{id}_{choice.upper()}')

    # Run the code template's separate configure.py script
    proc = subprocess.run([sys.executable, str(CODE_CONFIGURE_SCRIPT),
        '--kamek', str(config.kamek_exe),
        '--kstdlib', str(config.kstdlib_dir),
        '--cw', str(config.cw_exe),
        '--project-dir', str(CODE_ROOT_DIR),
        '--output-dir', str(RIIVO_DISC_CODE),
        *[f'-DNSMBWUP_{bf}' for bf in bug_flags],
    ])
    if proc.returncode != 0:
        exit(proc.returncode)

    # Now we can just include the build.ninja it just created, as a
    # "subninja".

    lines.append(f'subninja {CODE_NINJA_FILE}')
    lines.append('')

    # Note that this only works correctly because the configure.py for
    # the Kamek project obsessively uses absolute paths for everything.
    # Ideally Ninja would support setting the current working directory
    # within a subninja, but unfortunately it doesn't:
    # https://github.com/ninja-build/ninja/pull/456

    # ------------------------------------------------------------------
    # Loader: we do handle this one manually

    use_wine = (sys.platform != 'win32')

    quote = "'" if use_wine else '"'

    if use_wine:
        cc = CODE_CW_WINE_WRAPPER
        cc = f"{ninja_escape(sys.executable)} {quote}{ninja_escape(cc)}{quote} {quote}$mwcceppc{quote}"
    else:
        cc = '$mwcceppc'

    lines.append(f"""
mwcceppc = {ninja_escape(config.cw_exe)}
cc = {cc}
kamek = {ninja_escape(config.kamek_exe)}
kstdlib = {ninja_escape(config.kstdlib_dir)}
loaderdir = {ninja_escape(config.loader_dir)}
loaderaddr = {config.loader_base_addr:#08x}

cflags = $
  -I- $
  -i {quote}$kstdlib{quote} $
  -i {quote}$loaderdir{quote} $
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
  command = $cc $cflags -c -o $out -MDfile $out.d $in
  depfile = $out.d
  description = {ninja_escape(config.cw_exe.name)} -o $out_shortname $in_shortname
""".strip('\n'))

    cpp_files = set(config.loader_dir.glob('*.cpp'))

    # Add "cw" edges for all .cpp -> .o files
    for cpp_file in cpp_files:
        # sanity check
        if cpp_file.name not in {'kamekLoader.cpp', 'nsmbw.cpp'}:
            print(f'WARNING: Unexpected loader cpp file "{cpp_file}"')

        o_file = cpp_file.with_suffix('.o')
        lines.append(f'build {ninja_escape(o_file)}: cw {ninja_escape(cpp_file)}')
        lines.append(f'  out_shortname = {ninja_escape(o_file.name)}')
        lines.append(f'  in_shortname = {ninja_escape(cpp_file.name)}')

    lines.append(f"""
rule kmstatic
  command = {quote}$kamek{quote} $in -static=$baseaddr -input-riiv=$inxml -output-riiv=$outxml -output-code=$outbin -valuefile=$outbin_disc -quiet
  description = {ninja_escape(config.kamek_exe.name)} -> $outxml_filename + $outbin_filename
""".strip('\n'))

    # Add a "kmstatic" edge for loader.bin
    lines.append(f'build $outdir/{ninja_escape(RIIVO_XML.relative_to(OUTPUT_DIR))} {ninja_escape(RIIVO_DISC_CODE_LOADER)}: kmstatic')
    for cpp_file in cpp_files:
        o_file = cpp_file.with_suffix('.o')
        lines[-1] += f' {ninja_escape(o_file)}'
    lines.append(f'  inxml = {ninja_escape(config.get_xml_template_path())}')
    lines.append(f'  outxml = $outdir/{ninja_escape(RIIVO_XML.relative_to(OUTPUT_DIR))}')
    lines.append(f'  outxml_filename = {ninja_escape(RIIVO_XML.name)}')
    lines.append(f'  outbin = {ninja_escape(RIIVO_DISC_CODE_LOADER)}')
    lines.append(f'  outbin_filename = {ninja_escape(RIIVO_DISC_CODE_LOADER.name)}')
    lines.append(f'  outbin_disc = {ninja_escape(RIIVO_DISC_CODE_LOADER.relative_to(RIIVO_DISC_ROOT).as_posix())}')
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

    use_wine = (sys.platform != 'win32')
    quote = "'" if use_wine else '"'

    lines = [f"""
rule credits
  command = {quote}$py{quote} {CREDITS_PY} $in $out $bugs
  description = Editing credits...
""".strip('\n')]

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

            target_dir = f'$outdir/{ninja_escape((RIIVO_DISC_ROOT / staffroll_relative).relative_to(OUTPUT_DIR))}'
            lines.append(f'build {target_dir}: credits {ninja_escape(staffroll)}')

            already_covered_staffrolls.add(staffroll_relative)

    return '\n'.join(lines)


########################################################################
####################### Riivolution XML template #######################
########################################################################


CREATE_RIIVOLUTION_XML_TEMPLATE_PY = Path('create_riivolution_xml_template.py')

def make_riivolution_xml_template_rules(config: Config) -> str:
    """
    Create Ninja rules to build the Riivolution XML template
    """

    use_wine = (sys.platform != 'win32')
    quote = "'" if use_wine else '"'

    return f"""
rule riixml
  command = {quote}$py{quote} {ninja_escape(CREATE_RIIVOLUTION_XML_TEMPLATE_PY)} $out /{ninja_escape(PROJECT_SAFE_NAME)} {quote}--title={PROJECT_DISPLAY_NAME}{quote}
  description = Generating Riivolution XML template...

build {ninja_escape(config.get_xml_template_path())}: riixml
""".strip('\n')


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

    txt = f"""
# NOTE: This file is generated by {Path(__file__).name}.

py = {sys.executable}

# NOTE: "builddir" has special significance to Ninja (see the manual)
builddir = {BUILD_DIR}
outdir = {OUTPUT_DIR}

bugs = {' '.join(sorted(bug_items))}

{make_riivolution_xml_template_rules(config)}
{make_code_rules(config)}
{make_credits_rules(config)}
""".strip('\n')

    while '\n\n\n' in txt:
        txt = txt.replace('\n\n\n', '\n\n')

    return txt + '\n'


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


if __name__ == '__main__':
    main()
