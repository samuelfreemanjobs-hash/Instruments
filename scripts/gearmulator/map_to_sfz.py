#!/usr/bin/env python3
"""Export instrument.sfz from canonical instrument.map.json."""
from __future__ import annotations

import argparse
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from map_lib import load_map


def write_sfz_from_map(map_path: Path, out_path: Path | None = None) -> Path:
    data = load_map(map_path)
    session_dir = map_path.parent
    out_sfz = out_path or session_dir / "instrument.sfz"

    lines = [
        "// Generated from instrument.map.json — edit map, re-run map_to_sfz",
        f"// instrument_id: {data['instrument_id']}",
        f"// {data['instrument_label']}",
        "",
    ]

    for zone in data["zones"]:
        note = zone["root_key"]
        lo = zone.get("lo_key", note)
        hi = zone.get("hi_key", note)
        lo_vel = zone.get("lo_vel", 1)
        hi_vel = zone.get("hi_vel", 127)
        wav = zone["wav"]
        lines.extend(
            [
                "<region>",
                f"sample={wav}",
                f"lokey={lo} hikey={hi}",
                f"pitch_keycenter={note}",
                f"lovel={lo_vel} hivel={hi_vel}",
                "",
            ]
        )

    out_sfz.write_text("\n".join(lines), encoding="utf-8")
    return out_sfz


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("map_json", type=Path)
    parser.add_argument("--out", type=Path, default=None)
    args = parser.parse_args()
    out = write_sfz_from_map(args.map_json, args.out)
    print(f"Wrote {out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
