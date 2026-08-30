"""Imperative build customization pyproject.toml's static [tool.setuptools]
table cannot express: compiling src/ra_prng2/c/ra_prng2_struct.c into
bindings/cpython/libra_prng2.so via gcc at build time, and marking the
wheel as platform-specific. All package metadata lives in pyproject.toml.
"""
import shutil
import subprocess
import sys
from pathlib import Path

from setuptools import setup
from setuptools.command.build_py import build_py as _build_py
from setuptools.dist import Distribution

_ROOT = Path(__file__).resolve().parent
_C_SOURCE = _ROOT / "src" / "ra_prng2" / "c" / "ra_prng2_struct.c"
# Kept identical to Makefile's CFLAGS (minus -fPIC -shared, added below) so
# `make build-ext` and `pip install` produce equivalent binaries.
_CFLAGS = ["-O3", "-march=native", "-std=gnu17", "-include", "stdalign.h"]


class BuildPyWithCEngine(_build_py):
    """After the normal .py copy step, compiles the ra_prng2 C engine and
    drops libra_prng2.so directly into build_lib/bindings/cpython/, next to
    _ra_prng2.py, so it is picked up by whatever copies build_lib into the
    installed package / wheel (install_lib / bdist_wheel)."""

    def run(self):
        super().run()
        self._build_shared_library()

    def _build_shared_library(self):
        gcc = shutil.which("gcc")
        if gcc is None:
            sys.exit(
                "ra-prng: gcc not found on PATH. bindings.cpython._ra_prng2 "
                "requires compiling src/ra_prng2/c/ra_prng2_struct.c from "
                "source at install time; install gcc and retry."
            )
        out_dir = Path(self.build_lib) / "bindings" / "cpython"
        out_dir.mkdir(parents=True, exist_ok=True)
        out_path = out_dir / "libra_prng2.so"
        cmd = [gcc, *_CFLAGS, "-fPIC", "-shared", str(_C_SOURCE), "-o", str(out_path)]
        self.announce("building ra_prng2 C engine: " + " ".join(cmd), level=2)
        subprocess.run(cmd, check=True)


class BinaryDistribution(Distribution):
    """Declares this distribution as containing compiled, platform-specific
    code (libra_prng2.so) even though no setuptools.Extension is declared,
    so bdist_wheel doesn't tag the wheel py3-none-any."""

    def has_ext_modules(self):
        return True


setup(cmdclass={"build_py": BuildPyWithCEngine}, distclass=BinaryDistribution)
