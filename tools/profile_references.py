#!/usr/bin/env python3
"""Scan a folder of reference WAVs into spectral_references."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

_TOOLS_DIR = Path(__file__).resolve().parent
if str(_TOOLS_DIR) not in sys.path:
    sys.path.insert(0, str(_TOOLS_DIR))

from disklordz.references import profile_directory  # noqa: E402


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("folder", type=Path, help="Directory of reference .wav files")
    parser.add_argument("--db", type=Path, default=_TOOLS_DIR / "data" / "disklordz.db")
    args = parser.parse_args()
    if not args.folder.is_dir():
        print(f"Not a directory: {args.folder}", file=sys.stderr)
        return 1
    n = profile_directory(args.db, args.folder)
    print(f"Profiled {n} references into {args.db}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
