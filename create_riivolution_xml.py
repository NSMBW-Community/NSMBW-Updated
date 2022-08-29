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


PROJECT_SAFE_NAME = 'nsmbw_updated'
PROJECT_DISPLAY_NAME = 'NSMBW Updated'


def make_xml(args) -> str:
    xml = [f"""
<wiidisc version="1" shiftfiles="true" root="{args.root_dir}" log="true">
    <id game="SMN">
""".strip('\n')]

    for region in args.regions.split(','):
        xml.append(f'        <region type="{region}"/>')

    xml.append(f"""
    </id>
    <options>
        <section name="{args.title}">
            <option name="{args.title}" id="{PROJECT_SAFE_NAME}" default="1">
                <choice name="Enabled"><patch id="{PROJECT_SAFE_NAME}"/></choice>
            </option>
        </section>
    </options>
    <patch id="{PROJECT_SAFE_NAME}">
""".strip('\n'))

    if args.folder is not None:
        for f in args.folder:
            external, disc = f.split(',')
            xml.append(f'        <folder external="{external}" disc="{disc}" create="true" />')

    xml.append('TODO: <memory> stuff')

    xml.append("""
    </patch>
</wiidisc>
""".strip('\n'))

    return '\n'.join(xml)


def main(argv=None) -> None:

    parser = argparse.ArgumentParser(
        description=f'Create a Riivolution XML for {PROJECT_DISPLAY_NAME}.')

    parser.add_argument('output_file', type=Path,
        help='XML file to write output to')
    parser.add_argument('root_dir',
        help='"root" directory (you should probably start it with "/")')
    parser.add_argument('regions',
        help='comma-separated list of regions (e.g. "P,E,J")')
    parser.add_argument('title',
        help='name to use in the Riivolution menu')
    parser.add_argument('--folder', action='append',
        help='"external,disc"')

    args = parser.parse_args(argv)

    args.output_file.write_text(make_xml(args), encoding='utf-8')


if __name__ == '__main__':
    main()
