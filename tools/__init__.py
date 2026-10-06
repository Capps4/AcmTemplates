"""CTL (Capps Template Library) configuration. Edit defaults here, not in commands."""
import os
from pathlib import Path
import shutil

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / 'src'
MANIFEST = ROOT / 'Manifest.json'
DOCUMENT = ROOT / 'Library.md'
RENDER_SETTINGS = ROOT / 'tools/Library.json'  # Stable anchors and card presentation only.
CACHE = ROOT / '.cache'
REPORTS = CACHE / 'reports'
GENERATED = CACHE / 'generated'

# None selects the only discovered VS Code user configuration.
SNIPPETS_DIR = None
VSCODE_USER_DATA = None
MIGRATE_LEGACY_SNIPPETS = True
SNIPPETS_FILE = 'acm-templates.code-snippets'

YUQUE_DOCUMENT = 'capps/template/library'
YUQUE_STATE = ROOT / '.yuque'
YUQUE_TOKEN = YUQUE_STATE / 'token'  # Never store the token value in this file.
YUQUE_CLI_VERSION = '1.1.0'
YUQUE_CLI = YUQUE_STATE / 'runtime/node_modules/yuque-open-cli/dist/bin.js'
YUQUE_REGISTRY = 'https://registry.npmjs.org'
YUQUE_REQUEST_TIMEOUT = 150
YUQUE_PROMPT_TOKEN = True
# After merging remote edits, set this to the exact revision printed by CTL.
# It acknowledges that snapshot only; clear it after a successful update.
YUQUE_EXPECTED_REVISION = None
INITIAL_REVISIONS = {'capps/template/library': '44c53cd4362e24ba599c5ec5f82a6afecc384b276ff7d172bb66e7d50f19b259'}

CXX = os.environ.get('CXX') or shutil.which('g++-15') or 'g++'
SAN_CXX = os.environ.get('SAN_CXX') or shutil.which('clang++') or 'clang++'
FLAGS = {
    'strict': ['-std=gnu++17', '-Wall', '-Wextra', '-Werror', '-Weffc++',
               '-O0', '-g', '-D_GLIBCXX_DEBUG', '-rdynamic', '-fno-omit-frame-pointer'],
    'optimized': ['-std=gnu++17', '-O2', '-Wall', '-Wextra'],
    'sanitized': ['-std=gnu++17', '-O1', '-g', '-fsanitize=address,undefined',
                  '-fno-omit-frame-pointer'],
}
RIGHT_PROFILES = ('strict', 'optimized', 'sanitized')
TEST_MODULES = None  # None covers every module; a tuple selects a local working scope.
TEST_JOBS = 6
TEST_RUN_JOBS = 32  # Compiler concurrency stays at TEST_JOBS.
TEST_CASE = None  # Exact case ID or its module-relative suffix; local reproduction only.
RIGHT_SEED = 20261001
PERF_SEED = 20261005
TEST_TIMEOUT = 180
COMPILE_TIMEOUT = 300
PERF_DEFAULT_SIZES = (10000, 100000)
PERF_REPEATS = 3

# Workload targets, excluding compiler/process startup and strict/Sanitizer overhead.
CORRECTNESS_TARGET_MS = (0, 20)
PERFORMANCE_TARGET_MS = (0, 100)
