from pathlib import Path

import db as db_lib


README_PATH = Path('readme.md')
SPLIT_AT_STR = 'ID | Description | Tags | Versions | Options'


def make_table() -> str:
    """
    Build the Markdown table string
    """
    db = db_lib.Database.load_from_default_file()

    lines = []
    lines.append('ID | Description | Tags | Versions | Options')
    lines.append('-- | ----------- | ---- | -------- | -------')

    for id in sorted(db.entries):
        entry = db.entries[id]

        if entry.tags:
            tags_str = ' ' + entry.comma_separated_tags_str()
        else:
            tags_str = ''

        if entry.fixed_in:
            versions_str = f' Fixed in: {entry.comma_separated_fixed_in_str()}'
        elif entry.only_in:
            versions_str = f' Only in: {entry.comma_separated_only_in_str()}'
        else:
            versions_str = ''

        if entry.options:
            options = list(entry.options)
        else:
            options = ['on']
        options.append('off')
        options_str = ', '.join(options)

        lines.append(
            f'**{id}**'
            f' | {entry.name}'
            f' |{tags_str}'
            f' |{versions_str}'
            f' | {options_str}')
    lines.append('')

    return '\n'.join(lines)


def main() -> None:
    readme = README_PATH.read_text(encoding='utf-8')

    if SPLIT_AT_STR not in readme:
        raise ValueError(f"Couldn't find {SPLIT_AT_STR!r} in the readme")

    new_readme = readme.split(SPLIT_AT_STR)[0] + make_table()

    README_PATH.write_text(new_readme, encoding='utf-8')


if __name__ == '__main__':
    main()
