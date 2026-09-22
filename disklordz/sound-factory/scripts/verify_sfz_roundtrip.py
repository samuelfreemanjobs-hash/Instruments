#!/usr/bin/env python3
"""Verify SFZ + instrument-map + samples consistency (factory round-trip)."""

from __future__ import annotations

import argparse
import json
import re
from pathlib import Path


def verify(inst_dir: Path) -> list[str]:
    errors: list[str] = []
    maps = list(inst_dir.glob("instrument-map.json"))
    if not maps:
        return ["missing instrument-map.json"]
    imap = json.loads(maps[0].read_text(encoding="utf-8"))
    iid = imap.get("instrumentId", "unknown")
    sfz = inst_dir / f"{iid}.sfz"
    if not sfz.is_file():
        errors.append(f"missing sfz {sfz.name}")
        return errors
    text = sfz.read_text(encoding="utf-8")
    sample_lines = re.findall(r"^sample=(.+)$", text, re.MULTILINE)
    zone_paths = {z["samplePath"] for z in imap.get("zones", [])}
    for line in sample_lines:
        if line not in zone_paths:
            errors.append(f"sfz sample not in map: {line}")
        p = inst_dir / line
        if not p.is_file():
            errors.append(f"missing wav: {line}")
    for zp in zone_paths:
        if zp not in sample_lines:
            errors.append(f"map zone missing from sfz: {zp}")
    return errors


def main() -> None:
    p = argparse.ArgumentParser()
    p.add_argument("instrument_dir", type=Path)
    args = p.parse_args()
    err = verify(args.instrument_dir)
    if err:
        print(json.dumps({"pass": False, "errors": err}, indent=2))
        raise SystemExit(1)
    print(json.dumps({"pass": True}, indent=2))


if __name__ == "__main__":
    main()
