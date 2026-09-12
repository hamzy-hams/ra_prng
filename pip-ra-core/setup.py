"""Imperative build customization pyproject.toml's static [tool.setuptools]
table cannot express:
  1. compiling ../src/ra_core/c/ra_core_engine.c (which #includes the
     canonical, unmodified ../src/ra_core/c/ra_core.c -- no formula is
     duplicated) into ra_core/libra_core_engine.so via gcc at build time,
     for the ctypes-facing API (ra_core/_engine.py);
  2. a Cython extension (ra_core/_bitgen.pyx), compiled together with the
     SAME ra_core_engine.c source, for the numpy.random.BitGenerator
     integration -- that one needs compiled C function pointers, ctypes
     cannot provide them (numpy.random.BitGenerator.capsule is only
     writable from Cython/C, see _bitgen.pyx's header comment for the
     dead-end that was tried first);
  3. marking the wheel as platform-specific.
All package metadata lives in pyproject.toml. (1) and (3) mirror the root
repo's setup.py (ra-prng/ra_prng2) exactly, just pointed at ra_core's
engine source instead.
"""
import os
import shutil
import subprocess
import sys
from pathlib import Path

import numpy
from Cython.Build import cythonize
from setuptools import Extension, setup
from setuptools.command.build_py import build_py as _build_py
from setuptools.dist import Distribution

_ROOT = Path(__file__).resolve().parent
_C_SOURCE = _ROOT.parent / "src" / "ra_core" / "c" / "ra_core_engine.c"
_C_INCLUDE_DIR = _C_SOURCE.parent  # ra_core_engine.h, for _bitgen.pyx's cdef extern
# Kept identical to Makefile's CFLAGS (minus -fPIC -shared, added below) so
# `make build-ext-core` (repo root) and `pip install` produce equivalent
# binaries. -include stdalign.h is redundant here (ra_core_engine.c already
# #includes <stdalign.h> itself) but kept for parity with the root Makefile.
_CFLAGS = ["-O3", "-march=native", "-std=gnu17", "-include", "stdalign.h"]

# setuptools' Extension.sources rejects absolute paths outright ("setup()
# arguments must always be /-separated paths relative to the setup.py
# directory") -- relpath() gives it "../src/ra_core/c/ra_core_engine.c",
# which it accepts even though it climbs outside pip-ra-core/ (only sdist
# packaging, not `pip install` from this checkout, would need that file
# actually present under this directory).
def _relpath(p: Path) -> str:
    return os.path.relpath(p, _ROOT).replace(os.sep, "/")


_ext_modules = cythonize(
    [
        Extension(
            "ra_core._bitgen",
            sources=[_relpath(_ROOT / "ra_core" / "_bitgen.pyx"), _relpath(_C_SOURCE)],
            include_dirs=[numpy.get_include(), str(_C_INCLUDE_DIR)],
            extra_compile_args=_CFLAGS,
        )
    ],
    language_level=3,
)
# cythonize() rewrites the .pyx source entry to its generated .c file using
# an absolute path, regardless of what was passed in above -- re-relativize
# every entry, since setuptools rejects Extension.sources containing any
# absolute path.
for _ext in _ext_modules:
    _ext.sources = [_relpath(Path(s)) for s in _ext.sources]


class BuildPyWithCEngine(_build_py):
    """After the normal .py copy step, compiles the ra_core engine and drops
    libra_core_engine.so directly into build_lib/ra_core/, next to
    __init__.py, so it is picked up by whatever copies build_lib into the
    installed package / wheel (install_lib / bdist_wheel)."""

    def run(self):
        super().run()
        self._build_shared_library()

    def _build_shared_library(self):
        gcc = shutil.which("gcc")
        if gcc is None:
            sys.exit(
                "ra-core: gcc not found on PATH. ra_core._engine requires "
                "compiling src/ra_core/c/ra_core_engine.c from source at "
                "install time; install gcc and retry."
            )
        out_dir = Path(self.build_lib) / "ra_core"
        out_dir.mkdir(parents=True, exist_ok=True)
        out_path = out_dir / "libra_core_engine.so"
        cmd = [gcc, *_CFLAGS, "-fPIC", "-shared", str(_C_SOURCE), "-o", str(out_path)]
        self.announce("building ra_core engine: " + " ".join(cmd), level=2)
        subprocess.run(cmd, check=True)


class BinaryDistribution(Distribution):
    """Declares this distribution as containing compiled, platform-specific
    code (libra_core_engine.so) even though no setuptools.Extension is
    declared, so bdist_wheel doesn't tag the wheel py3-none-any."""

    def has_ext_modules(self):
        return True


setup(
    cmdclass={"build_py": BuildPyWithCEngine},
    distclass=BinaryDistribution,
    ext_modules=_ext_modules,
)
