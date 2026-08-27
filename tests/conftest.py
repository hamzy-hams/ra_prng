"""Shared pytest setup for the ra_prng test suite.

Ensures `import ra_prng2` and `from bindings.cpython._ra_prng2 import RaPrng2`
resolve regardless of whether `make dev-install` has been run, and
regardless of the working directory pytest is invoked from.
"""
import sys
from pathlib import Path

_REPO_ROOT = Path(__file__).resolve().parent.parent
_PY_SRC = _REPO_ROOT / "src" / "ra_prng2" / "python"

for _p in (str(_REPO_ROOT), str(_PY_SRC)):
    if _p not in sys.path:
        sys.path.insert(0, _p)
