import os.path

# Everything highly specific to Staffroll Tool is in this section, to
# make it simpler to copypaste this script across all of the
# NSMBW-related projects that use the same technologies (Reggie, Puzzle,
# BRFNTify, etc)

PROJECT_NAME = 'NSMBW Staffroll Tool'
FULL_PROJECT_NAME = 'NSMBW Staffroll Tool'
PROJECT_VERSION = '1.0'

MAC_BUNDLE_IDENTIFIER = 'roadrunnerwmc.nsmbwstaffrolltool'

SCRIPT_FILE = 'staffroll.py'
DATA_FOLDERS = []
DATA_FILES = ['README.md', 'LICENSE']
EXTRA_IMPORT_PATHS = []

USE_PYQT = False
USE_NSMBLIB = False

EXCLUDE_SELECT = True
EXCLUDE_THREADING = True
EXCLUDE_HASHLIB = True
EXCLUDE_LOCALE = False

# macOS only
AUTO_APP_BUNDLE_NAME = SCRIPT_FILE.split('.')[0] + '.app'
FINAL_APP_BUNDLE_NAME = FULL_PROJECT_NAME + '.app'
