# Copyright 2019 RoadrunnerWMC
#
# This file is part of Staffroll Tool.
#
# Staffroll Tool is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Staffroll Tool is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Staffroll Tool.  If not, see <https://www.gnu.org/licenses/>.

import argparse
import pathlib

import staffroll_lib


def test():
    """
    Simple test function
    """
    for fn in pathlib.Path('.').glob('test/*/staffroll.bin'):
        with open(fn, 'rb') as f:
            fd = f.read()

        lines = staffroll_lib.readStaffrollBin(fd)

        fd2 = staffroll_lib.saveStaffrollBin(lines)

        assert fd2 == fd

        txt = staffroll_lib.saveStaffrollTxt(lines)
        lines2 = staffroll_lib.readStaffrollTxt(txt)

        assert staffroll_lib.saveStaffrollTxt(lines2) == txt
        assert staffroll_lib.saveStaffrollBin(lines2) == fd2


def main():
    """
    Main function for CLI execution
    """
    # Define the CLI
    parser = argparse.ArgumentParser()
    parser.add_argument('in_file', help='the input file path')
    parser.add_argument('out_file', help='the output file path (defaults to [in_file].[ext])', nargs='?')
    parser.add_argument('--type',
                        help='the input file type (will be guessed if not specified)',
                        choices=['bin', 'txt'])
    parser.add_argument('--dont-abbr-indents',
                        help='do not abbreviate indentation values when converting to text',
                        action='store_true')
    args = parser.parse_args()
    
    # Open the file data (as binary, since we may have to guess the file
    # type soon)
    with open(args.in_file, 'rb') as f:
        data = f.read()

    # Is this a binary file?
    if args.type is None:
        # Simple heuristic that'll work 95% of the time is to check for
        # the existence of null bytes.
        is_bin = (b'\0' in data)
    else:
        # We're told explicitly.
        is_bin = (args.type == 'bin')


    # Determine the output filename
    out_fn = args.out_file
    if out_fn is None:
        out_fn = args.in_file + ('.txt' if is_bin else '.bin')
    

    if is_bin:
        # Convert to text
        lines = staffroll_lib.readStaffrollBin(data)
        txt = staffroll_lib.saveStaffrollTxt(lines, not args.dont_abbr_indents)
        with open(out_fn, 'w', encoding='utf-8') as f:
            f.write(txt)

    else:
        # Convert to binary
        if args.dont_abbr_indents:
            print('Warning: converting text to binary, but --dont-abbr-indents is specified (which has no effect there)')

        data = data.decode('utf-8')
        lines = staffroll_lib.readStaffrollTxt(data)
        bin = staffroll_lib.saveStaffrollBin(lines)
        with open(out_fn, 'wb') as f:
            f.write(bin)


if __name__ == '__main__':
    main()
