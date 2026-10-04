"""Compatibility entrypoint; option switches are now part of integration checks."""
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from Run import main
if __name__ == '__main__':
    main(['integration'])
