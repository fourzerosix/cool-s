"""
cool_s -- Draw the legendary Cool S in your terminal.

This package installs the `cool-s` command-line tool, which is written in C
and compiled from source on first use.  The Python wrapper handles compilation
and then exec's the binary so all terminal features (ANSI color, TTY detection,
cursor control) work exactly as they do when building from source.

Usage from the command line (after `pip install cool-s`):
    cool-s
    cool-s -o
    cool-s -f -s 5 -r

Usage from Python (unusual but possible):
    from cool_s import run
    run()          # equivalent to running `cool-s` with no args
    run(["-f"])    # fast mode
"""

__version__ = "1.0.0"
__all__ = ["main", "run"]

import os
import sys
import subprocess
import shutil
import tempfile
from pathlib import Path


# ── Path helpers ───────────────────────────────────────────────────────────────

def _package_dir() -> Path:
    return Path(__file__).parent


def _binary_path() -> Path:
    """Return the path where we store the compiled binary (inside the package)."""
    return _package_dir() / "_cool_s_bin"


def _source_path() -> Path:
    return _package_dir() / "cool-s.c"


# ── Compilation ────────────────────────────────────────────────────────────────

def _find_compiler() -> str:
    for cc in ("gcc", "cc", "clang"):
        if shutil.which(cc):
            return cc
    raise RuntimeError(
        "cool-s: no C compiler found. Install gcc (e.g. `sudo apt install gcc` "
        "or `brew install gcc`) and re-run `pip install cool-s`."
    )


def _compile() -> Path:
    """Compile the C source to a binary adjacent to the package. Returns binary path."""
    binary = _binary_path()
    if binary.exists():
        return binary

    source = _source_path()
    if not source.exists():
        raise FileNotFoundError(
            f"cool-s: C source not found at {source}. "
            "Try reinstalling: `pip install --force-reinstall cool-s`"
        )

    cc = _find_compiler()
    print(f"cool-s: compiling from source with {cc} (one-time setup)...",
          file=sys.stderr)

    # Compile to a temp file first, then atomically move into place
    tmp = Path(tempfile.mktemp(suffix="_cool_s_bin", dir=_package_dir()))
    try:
        result = subprocess.run(
            [cc, "-O2", "-std=gnu99", "-o", str(tmp), str(source), "-lm"],
            capture_output=True, text=True
        )
        if result.returncode != 0:
            raise RuntimeError(
                f"cool-s: compilation failed:\n{result.stderr}"
            )
        tmp.chmod(0o755)
        tmp.rename(binary)
        print("cool-s: compiled successfully.", file=sys.stderr)
    except Exception:
        if tmp.exists():
            tmp.unlink()
        raise

    return binary


# ── Entry points ───────────────────────────────────────────────────────────────

def run(args: list = None):
    """
    Run cool-s with the given argument list (default: sys.argv[1:]).

    Replaces the current process with the cool-s binary (os.execv) so that
    all TTY / terminal features work correctly.  Does not return.
    """
    if args is None:
        args = sys.argv[1:]

    binary = _compile()
    os.execv(str(binary), ["cool-s"] + list(args))


def main():
    """Entry point for the `cool-s` console script."""
    try:
        run()
    except KeyboardInterrupt:
        sys.exit(0)
    except Exception as exc:
        print(f"cool-s error: {exc}", file=sys.stderr)
        sys.exit(1)
