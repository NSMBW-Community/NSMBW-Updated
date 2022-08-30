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

import enum
import io
import struct


class Contents(enum.IntEnum):
    """
    An enum defining valid values for the "block contents" nybble.
    """
    RANDOM = 0
    FORCE_COIN = 1
    NO_COIN = 2
    UNBREAKABLE = 3  # and anything greater than 3 (tested: 3, 4, 8)


class Tag(enum.Enum):
    """
    An enum representing tags in the text file. 
    """
    COPYRIGHT = '<copyrights>'
    BEGIN_BOLD = '<bold>'
    END_BOLD = '</bold>'
    BEGIN_FORCE_COIN = '<coin>'
    END_FORCE_COIN = '</coin>'
    BEGIN_NO_COIN = '<no_coin>'
    END_NO_COIN = '</no_coin>'
    BEGIN_UNBREAKABLE = '<unbreakable>'
    END_UNBREAKABLE = '</unbreakable>'

    @property
    def is_end(self):
        return self in END_TAGS

    def __str__(self):
        return self.value
    __repr__ = __str__

END_TAGS = set([Tag.END_BOLD, Tag.END_FORCE_COIN, Tag.END_NO_COIN, Tag.END_UNBREAKABLE])



class StaffrollLine:
    """
    A single line in staffroll.bin. The file as a whole is represented
    as a list of these.
    """
    def __init__(self, indent, parts):
        self.indent = indent
        self.parts = parts


    @classmethod
    def fromBinFile(cls, f):
        """
        Read this line from the provided binary file object
        """
        hdr, = struct.unpack_from('>I', f.read(4))

        # FFFFFFFF indicates a blank line
        if hdr == 0xFFFFFFFF:
            return None

        # We're going to build up the parts list. We need to keep track
        # of the current text formatting state at all times, so we can
        # recognize changes and insert tags appropriately.
        parts = []
        bold = False
        contents = Contents.RANDOM

        # current_str is a list of character codepoints
        current_str = []
        def flush_current_str():
            """
            Convert current_str to an actual str, put it in the parts
            list, and clear it
            """
            nonlocal current_str
            if current_str:
                parts.append(''.join(chr(c) for c in current_str))
                current_str.clear()

        num_chars, = struct.unpack_from('>I', f.read(4))
        for j in range(num_chars):
            # This contains the codepoint as well as formatting metadata
            char_data, = struct.unpack_from('>I', f.read(4))

            if char_data & 0x20000000:
                parts.append(Tag.COPYRIGHT)

            else:
                new_bold     = char_data & 0x10000000
                new_contents = Contents(min(max(Contents), char_data & 0xF))

                # Insert a tag if anything changed
                if new_bold and not bold:
                    flush_current_str()
                    parts.append(Tag.BEGIN_BOLD)
                elif bold and not new_bold:
                    flush_current_str()
                    parts.append(Tag.END_BOLD)

                if new_contents != contents:
                    flush_current_str()

                    # End previous contents tag
                    if contents == Contents.FORCE_COIN:
                        parts.append(Tag.END_FORCE_COIN)
                    elif contents == Contents.NO_COIN:
                        parts.append(Tag.END_NO_COIN)
                    elif contents == Contents.UNBREAKABLE:
                        parts.append(Tag.END_UNBREAKABLE)

                    # Begin new contents tag
                    if new_contents == Contents.FORCE_COIN:
                        parts.append(Tag.BEGIN_FORCE_COIN)
                    elif new_contents == Contents.NO_COIN:
                        parts.append(Tag.BEGIN_NO_COIN)
                    elif new_contents == Contents.UNBREAKABLE:
                        parts.append(Tag.BEGIN_UNBREAKABLE)

                # Add the codepoint to the list
                current_str.append(((char_data >> 4) & 0xFF) + 32)

                # Keep track of the new character formatting options for
                # next time
                bold, contents = new_bold, new_contents

        # Flush any remaining text data
        flush_current_str()

        return cls(hdr, parts)


    def saveToBinFile(self, f):
        """
        Write this line to the provided binary file object
        """
        self.delete_unrepresentable_chars()

        if self.indent < 0:
            print(f'WARNING: Line "{str(self)}" has negative offset (auto-clamping to 0)')

        f.write(struct.pack('>II', max(0, self.indent), self.num_chars_and_copyrights))

        # Need to keep track of these so we know what metadata to set on
        # each character
        bold = False
        contents = Contents.RANDOM

        for part in self.parts:
            if part == Tag.COPYRIGHT:
                f.write(b'\x20\0\0\0')
            elif part == Tag.BEGIN_BOLD:
                bold = True
            elif part == Tag.END_BOLD:
                bold = False
            elif part == Tag.BEGIN_FORCE_COIN:
                contents = Contents.FORCE_COIN
            elif part == Tag.BEGIN_NO_COIN:
                contents = Contents.NO_COIN
            elif part == Tag.BEGIN_UNBREAKABLE:
                contents = Contents.UNBREAKABLE
            elif isinstance(part, Tag) and part.is_end:
                contents = Contents.RANDOM
            else: # string instance
                for c in part:
                    cval = (ord(c) - 32) << 4
                    if bold: cval |= 0x10000000
                    cval |= contents

                    f.write(struct.pack('>I', cval))


    @classmethod
    def fromText(cls, text):
        """
        Read this line from the provided string (from a text file)
        """

        # Get the indentation value, and the remaining part of the line
        # following that
        colon_offs = text.find(':')
        indent = text[:colon_offs]

        if not indent:
            indent = None
        else:
            indent = int(indent)

        raw_line = text[colon_offs + 1:]

        def iter_parts(raw_line):
            """
            Iterate over the line string and yield strings and Tags
            appropriately
            """
            def find_all(tag):
                """
                Return a set of all indices where tag appears in raw_line.
                Case-insensitive.
                """
                tag = tag.upper()

                items = set()
                idx = raw_line.upper().find(tag)
                while idx >= 0:
                    items.add(idx)
                    idx = raw_line.upper().find(tag, idx + 1)

                return items

            # Find every instance of every Tag in raw_line
            tag_idxs = {tag: find_all(tag.value) for tag in Tag}

            # List containing the in-progress string
            # (list of characters)
            current_str = []

            idx = 0
            while idx < len(raw_line):
                for tag in Tag:
                    if idx in tag_idxs[tag]:
                        # There's a tag that starts at this index!

                        # Yield (flush) the string up to this point
                        if current_str:
                            yield ''.join(current_str)
                            current_str.clear()

                        # Yield the appropriate command, and skip past
                        # the tag contents
                        yield tag
                        idx += len(tag.value)
                        break

                else:
                    # No tag at this location, so just add the character
                    # to the current string
                    current_str.append(raw_line[idx])
                    idx += 1

            # Yield (flush) any remaining string
            if current_str:
                yield ''.join(current_str)

        # Collect all parts into a single list
        parts = [*iter_parts(raw_line)]

        # Trim off unnecessary END_ commands from the end of parts
        # (mainly for consistency with fromBinFile(), which behaves
        # this way)
        while parts and parts[-1] in END_TAGS:
            parts.pop()

        # Put together the new object, automatically calculating the
        # indentation level if appropriate
        obj = cls(indent, parts)
        if indent is None: obj.indent = obj.auto_indent
        return obj


    def saveAsText(self, abbreviate_indent=True):
        """
        Generate a string representing this line (for use in a text
        file). If abbreviate_indent is True, the indentation level will
        be omitted if it's the same as the calculated indentation level
        for automatic centering.
        """
        # This list will contain strings, to be merged together at the
        # end with ''.join()
        t = []

        # Add the indentation amount (but omit it if appropriate)
        if not abbreviate_indent or self.indent != self.auto_indent:
            t.append(str(self.indent))
        t.append(':')

        # Keep track of these, so we can figure out when these states
        # actually change
        bold = False
        contents = Contents.RANDOM

        for part in self.parts:
            if isinstance(part, Tag):
                # Only append the tag if it changes one of the
                # text formatting state variables
                if part == Tag.COPYRIGHT:
                    t.append('<Copyrights>')

                elif part == Tag.BEGIN_BOLD and not bold:
                    t.append('<bold>')
                    bold = True
                elif part == Tag.END_BOLD and bold:
                    t.append('</bold>')
                    bold = False

                else:
                    # Contents tag.

                    # (avoiding using .get(part, Contents.RANDOM) here
                    # so that it'll fail loudly if new tags are added in
                    # the future and this code doesn't get updated)
                    new_contents = {
                        Tag.BEGIN_FORCE_COIN: Contents.FORCE_COIN,
                        Tag.END_FORCE_COIN: Contents.RANDOM,
                        Tag.BEGIN_NO_COIN: Contents.NO_COIN,
                        Tag.END_NO_COIN: Contents.RANDOM,
                        Tag.BEGIN_UNBREAKABLE: Contents.UNBREAKABLE,
                        Tag.END_UNBREAKABLE: Contents.RANDOM,
                    }[part]

                    if contents != new_contents:

                        # End previous contents tag
                        if contents == Contents.FORCE_COIN:
                            t.append('</coin>')
                        elif contents == Contents.NO_COIN:
                            t.append('</no_coin>')
                        elif contents == Contents.UNBREAKABLE:
                            t.append('</unbreakable>')

                        # Begin new contents tag
                        if new_contents == Contents.FORCE_COIN:
                            t.append('<coin>')
                        elif new_contents == Contents.NO_COIN:
                            t.append('<no_coin>')
                        elif new_contents == Contents.UNBREAKABLE:
                            t.append('<unbreakable>')

                        contents = new_contents

            else: # string
                t.append(part)

        # Add closing tags if required
        if bold:
            t.append('</bold>')

        if contents == Contents.FORCE_COIN:
            t.append('</coin>')
        elif contents == Contents.NO_COIN:
            t.append('</no_coin>')
        elif contents == Contents.UNBREAKABLE:
            t.append('</unbreakable>')

        return ''.join(t)


    def delete_unrepresentable_chars(self):
        """
        Delete unrepresentable characters in all strings (byte value < 32)
        """
        for i, p in enumerate(self.parts):
            if isinstance(p, str):
                self.parts[i] = ''.join(c for c in p if ord(c) >= 32)


    @property
    def num_chars(self):
        """
        The total number of characters in this line.
        """
        v = 0
        for p in self.parts:
            if isinstance(p, str):
                v += len(p)
        return v


    @property
    def num_chars_and_copyrights(self):
        """
        The total number of characters in this line, counting
        Tag.COPYRIGHT as a character.
        """
        v = self.num_chars
        for p in self.parts:
            if p == Tag.COPYRIGHT:
                v += 1
        return v


    @property
    def auto_indent(self):
        """
        The indentation level that would be required for properly
        center-aligning the line. This is calculated automatically based
        on the text contents of the line.
        """
        # TODO: should this use .num_chars or .num_chars_and_copyrights?
        return max(0, 15 - self.num_chars // 2)


    def __str__(self):
        return self.saveAsText()

    def __repr__(self):
        return f'{type(self).__name__}({self.indent}, {self.parts})'


def readStaffrollBin(data):
    """
    Convert a bytes object containing staffroll.bin to a list of
    StaffrollLine objects and None's
    """

    f = io.BytesIO(data)

    lines = []
    num_lines, = struct.unpack_from('>I', f.read(4))
    for i in range(num_lines):
        lines.append(StaffrollLine.fromBinFile(f))

    return lines


def saveStaffrollBin(lines):
    """
    Convert a list of StaffrollLine objects and None's to a bytes object
    containing staffroll.bin
    """

    f = io.BytesIO()

    f.write(struct.pack('>I', len(lines)))

    for line in lines:
        if line is None:
            f.write(b'\xFF\xFF\xFF\xFF')
        else:
            line.saveToBinFile(f)

    f.seek(0)
    return f.read()


def readStaffrollTxt(s):
    """
    Convert a string containing a text-format staffroll file to a list
    of StaffrollLine objects and None's
    """
    lines = []
    for line in s.split('\n'):
        if not line:
            lines.append(None)
        else:
            lines.append(StaffrollLine.fromText(line))
    return lines


def saveStaffrollTxt(lines, abbreviate_indent=True):
    """
    Convert a list of StaffrollLine objects and None's to a string
    containing a text-format staffroll file
    """
    txt_lines = []
    for line in lines:
        if line is None:
            txt_lines.append('')
        else:
            txt_lines.append(line.saveAsText(abbreviate_indent))

    return '\n'.join(txt_lines)
