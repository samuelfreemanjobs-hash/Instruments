#!/usr/bin/env python3
"""Create instrument.map.json (canonical) from session manifest.tsv; optional SFZ."""
from __future__ import annotations

import argparse
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from map_lib import finalize_session_dir, load_map


def main() -> int:
    parser = argparse.ArgumentParser(description="Finalize multisample session → instrument.map.json")
    parser.add_argument("session_dir", type=Path, help="Folder containing manifest.tsv and WAVs")
    parser.add_argument("--instrument-id", default=None, help="Override instrument_id slug")
    parser.add_argument("--sfz", action="store_true", help="Also write instrument.sfz via map_to_sfz")
    args = parser.parse_args()

    try:
        map_path = finalize_session_dir(
            args.session_dir,
            instrument_id=args.instrument_id,
            write_sfz=args.sfz,
        )
    except FileNotFoundError as exc:
        print(f"Missing {exc}", file=sys.stderr)
        return 1

    zones = len(load_map(map_path)["zones"])
    print(f"Wrote {map_path} ({zones} zones)")
    if args.sfz:
        print(f"Wrote {map_path.parent / 'instrument.sfz'}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
