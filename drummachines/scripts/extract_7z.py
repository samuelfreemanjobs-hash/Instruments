#!/usr/bin/env python3
"""Extract a .7z archive into a target directory (used by fetch_and_extract.sh)."""

from __future__ import annotations

import sys
from pathlib import Path

try:
    import py7zr
except ImportError:
    print("py7zr is required: pip install py7zr", file=sys.stderr)
    sys.exit(1)


def main() -> None:
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <archive.7z> <out_dir>", file=sys.stderr)
        sys.exit(1)
    archive = Path(sys.argv[1]).resolve()
    out_dir = Path(sys.argv[2]).resolve()
    if not archive.is_file():
        raise SystemExit(f"Archive not found: {archive}")
    out_dir.mkdir(parents=True, exist_ok=True)
    with py7zr.SevenZipFile(archive, mode="r") as zf:
        zf.extractall(path=out_dir)
    print(f"Extracted {archive.name} -> {out_dir}")


if __name__ == "__main__":
    main()
