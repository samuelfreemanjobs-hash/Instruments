#!/usr/bin/env python3
"""Deprecated: prefer instrument.map.json + map_to_sfz.py (finalize runs automatically after capture)."""
from __future__ import annotations

import argparse
import csv
import sys
from pathlib import Path


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Legacy SFZ from manifest.tsv — use finalize_multisample_session.py + map_to_sfz.py instead",
    )
    parser.add_argument("manifest", type=Path)
    parser.add_argument("--out", type=Path, default=None)
    args = parser.parse_args()

    print(
        "Note: manifest_to_sfz.py is legacy. Capture scripts write instrument.map.json; "
        "use map_to_sfz.py on that file.",
        file=sys.stderr,
    )

    rows = []
    with args.manifest.open(encoding="utf-8") as f:
        reader = csv.DictReader(f, delimiter="\t")
        for row in reader:
            rows.append(row)

    if not rows:
        raise SystemExit("Empty manifest")

    label = rows[0].get("instrument_label", "instrument")
    out_sfz = args.out or args.manifest.with_suffix(".sfz")
    manifest_dir = args.manifest.parent

    lines = [
        "// Generated from manifest — tune envelope/grouping in your sampler",
        f"// Instrument: {label}",
        "",
    ]

    for row in rows:
        wav = Path(row["wav"])
        if not wav.is_absolute():
            wav = manifest_dir / wav.name
        note = int(row["note"])
        vel = int(row.get("velocity", "100"))
        lines.extend(
            [
                "<region>",
                f"sample={wav.name}",
                f"lokey={note} hikey={note}",
                f"pitch_keycenter={note}",
                f"lovel={vel} hivel={vel}",
                "",
            ]
        )

    out_sfz.write_text("\n".join(lines), encoding="utf-8")
    print(f"Wrote {out_sfz} ({len(rows)} regions)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
