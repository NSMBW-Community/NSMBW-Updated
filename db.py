import dataclasses
import enum
from pathlib import Path
from typing import Dict, List, Optional, Set, Union

from nsmbw_constants import VERSIONS


DEFAULT_DB_PATH = Path(__file__).parent / 'db.txt'


class DatabaseEntryTag(enum.Flag):
    """
    Represents the optional tags that can be associated with a database
    entry
    """
    NONE = 0
    BACKWARD_INCOMPATIBLE = enum.auto()
    ENHANCEMENT = enum.auto()
    FUN = enum.auto()
    MODDING_ONLY = enum.auto()
    SUBJECTIVE = enum.auto()

    @classmethod
    def from_name(cls, name: str) -> 'Self':
        """
        Convert a tag name string to a DatabaseEntryTag instance
        """
        return {
            'B': cls.BACKWARD_INCOMPATIBLE,
            'E': cls.ENHANCEMENT,
            'F': cls.FUN,
            'M': cls.MODDING_ONLY,
            'S': cls.SUBJECTIVE,
        }.get(name.upper())

    def name(self) -> str:
        """
        Get the name string for this instance
        """
        return {
            self.BACKWARD_INCOMPATIBLE: 'B',
            self.ENHANCEMENT: 'E',
            self.FUN: 'F',
            self.MODDING_ONLY: 'M',
            self.SUBJECTIVE: 'S',
        }.get(self)



@dataclasses.dataclass
class DatabaseEntry:
    """
    Represents one database entry
    """
    id: str
    name: str
    tags: DatabaseEntryTag
    fixed_in: Optional[Set[str]]
    only_in: Optional[Set[str]]
    options: Optional[List[str]]

    @classmethod
    def from_str(cls, s: str) -> 'Self':
        """
        Read a database entry from a string
        """
        id = None
        name = None
        tags = DatabaseEntryTag.NONE
        fixed_in = None
        only_in = None
        options = None

        for line in s.splitlines():
            if not line: continue

            if id is None:
                id = line
                if len(id) != 6:
                    raise ValueError(f'ID {id!r} has wrong length')

            elif line.startswith('tags:'):
                for tag_name in line.split(':')[1].split(','):
                    tag = DatabaseEntryTag.from_name(tag_name.strip())
                    if tag is None:
                        raise ValueError(f'Unknown tag in {id}: {tag!r}')
                    tags |= tag

            elif line.startswith('fixed-in:'):
                fixed_in = {v.strip() for v in line.split(':')[1].split(',')}

            elif line.startswith('only-in:'):
                only_in = {v.strip() for v in line.split(':')[1].split(',')}

            elif line.startswith('options:'):
                options = [v.strip() for v in line.split(':')[1].split(',')]

            else:
                if name is None:
                    name = line.strip()
                else:
                    raise ValueError(f'Unexpected line: {line!r}')

        if id is None:
            raise ValueError('ID not found')
        elif name is None:
            raise ValueError(f'Name not found for {id}')

        if fixed_in is not None and only_in is not None:
            raise ValueError("Can't specify both fixed-in and only-in for the same entry")

        return cls(id=id, name=name, tags=tags, fixed_in=fixed_in, only_in=only_in, options=options)

    def comma_separated_tags_str(self) -> str:
        """
        Return the tags, as a nice comma-separated string
        """
        # ugh
        distinct_tags = []
        for f in DatabaseEntryTag:
            if self.tags & f:
                distinct_tags.append(f)
        return ', '.join(sorted(f.name() for f in distinct_tags))

    def comma_separated_fixed_in_str(self) -> str:
        """
        Return the "fixed-in" versions set, as a nice comma-separated
        string
        """
        return ', '.join(v for v in VERSIONS if v in self.fixed_in)

    def comma_separated_only_in_str(self) -> str:
        """
        Return the "only-in" versions set, as a nice comma-separated
        string
        """
        return ', '.join(v for v in VERSIONS if v in self.only_in)

    def comma_separated_options_str(self) -> str:
        """
        Return the "options" list, as a nice comma-separated string
        """
        return ', '.join(v for v in self.options)

    def __str__(self):
        lines = []
        lines.append(self.id)

        if self.tags:
            lines.append(f'tags: {self.comma_separated_tags_str()}')
        if self.fixed_in:
            lines.append(f'fixed-in: {self.comma_separated_fixed_in_str()}')
        if self.only_in:
            lines.append(f'only-in: {self.comma_separated_only_in_str()}')
        if self.options:
            lines.append(f'options: {self.comma_separated_options_str()}')

        lines.append('')
        lines.append(self.name)

        return '\n'.join(lines)

    def get_default_active_option(self) -> Union[bool, str]:
        """
        Get the default option value for when this bugfix is activated.
        If the bugfix has no options other than "on" or "off", this
        just returns True. Otherwise, the default option is the first
        string in that list, so it returns that.
        """
        if self.options is None:
            return True
        else:
            return self.options[0]


class Database:
    """
    Represents the whole database
    """
    entries: Dict[str, DatabaseEntry]

    def __init__(self, entries):
        self.entries = entries

    @classmethod
    def load_from_file(cls, path: Path) -> 'Self':
        """
        Load the database from a file
        """
        with path.open('r', encoding='utf-8') as f:
            text = f.read()

        entries = {}
        for part in text.split('--------\n'):
            part = part.strip('\n')
            if part:
                entry = DatabaseEntry.from_str(part)
                entries[entry.id] = entry

        return cls(entries)

    @classmethod
    def load_from_default_file(cls) -> 'Self':
        """
        Load the database from the default file location
        """
        return cls.load_from_file(DEFAULT_DB_PATH)

    def __str__(self):
        return '\n\n--------\n'.join(str(e) for k, e in sorted(self.entries.items()))
