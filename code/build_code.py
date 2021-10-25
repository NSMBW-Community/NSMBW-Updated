# MIT License

# Copyright (c) 2021 RoadrunnerWMC

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.


from pathlib import Path
import subprocess
import sys


# Paths to things
KAMEK_ROOT_DIR = Path('Kamek')

CODEWARRIOR_EXE = KAMEK_ROOT_DIR / 'cw' / 'mwcceppc.exe'
KAMEK_EXE = KAMEK_ROOT_DIR / 'Kamek' / 'bin' / 'Debug' / 'Kamek.exe'
KSTDLIB_DIR = KAMEK_ROOT_DIR / 'k_stdlib'

ADDRESS_MAP_TXT = Path('address-map.txt')

SRC_DIR = Path('src')
INCLUDE_DIR = Path('include')

OUTPUT_DIR = Path('bin')

# Source files
CPP_FILES = list(SRC_DIR.glob('**/*.cpp'))


print('Compiling...')

CC = [str(CODEWARRIOR_EXE)]
if sys.platform != 'win32':
    CC = ['wine'] + CC

CFLAGS = [
    '-I-',
    '-i', str(KSTDLIB_DIR),
    '-i', str(INCLUDE_DIR),
    '-Cpp_exceptions', 'off',
    '-enum', 'int',
    '-O4,s',
    '-use_lmw_stmw', 'on',
    '-fp', 'hard',
    '-rostr',
    '-sdata', '0',
    '-sdata2', '0',
    '-RTTI', 'off']

for fp in CPP_FILES:
    print(f'Compiling {fp}...')
    subprocess.run(CC + CFLAGS + ['-c', '-o', str(fp.with_suffix('.o')), str(fp)])


print('Linking...')

if not OUTPUT_DIR.is_dir():
    OUTPUT_DIR.mkdir()

subprocess.run([
    str(KAMEK_EXE),
    *[str(fn.with_suffix('.o')) for fn in CPP_FILES],
    '-dynamic',
    '-versions=' + str(ADDRESS_MAP_TXT),
    '-output-kamek=' + str(OUTPUT_DIR / '$KV$.bin'),
])
