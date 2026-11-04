#!/usr/bin/env python3
"""Stage WAVs for manual MPC 500 Compact Flash copy (stock OS, lane pad maps)."""

from __future__ import annotations

import argparse
import json
import re
import shutil
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
LANES = REPO / "mpc500" / "lanes"

# Loose filename → pad slot (first match wins when scanning --from)
PHONK_ALIASES: dict[str, str] = {
    r"kick|808.?kick|bd": "KICK808.WAV",
    r"snare|snap|sd": "SNARE.WAV",
    r"clap|cp": "CLAP.WAV",
    r"chh|closed.?hat|closedhat|hh.?closed": "CHH.WAV",
    r"ohh|open.?hat|openhat": "OHH.WAV",
    r"rim|ghost": "RIM.WAV",
    r"cowb|bell": "COWB.WAV",
    r"vox|fx.?vox|memphis|fxvox": "FXVox.WAV",
    r"sub|808.?tail|808.?sub": "SUB808.WAV",
    r"rev|reverse|rise|fxrev": "FXREV.WAV",
    r"vinyl|noise|crackle": "VINYL.WAV",
    r"perc|tamb|shaker|conga": "PERC.WAV",
}


def load_pad_map(lane: str) -> dict:
    path = LANES / lane / "pad-map.json"
    if not path.is_file():
        raise SystemExit(f"Unknown lane or missing pad-map: {path}")
    return json.loads(path.read_text(encoding="utf-8"))


def match_cf_name(filename: str) -> str | None:
    stem = filename.lower()
    for pattern, cf_name in PHONK_ALIASES.items():
        if re.search(pattern, stem):
            return cf_name
    return None


def stage_lane(from_dir: Path, out_dir: Path, lane: str, *, dry_run: bool) -> dict:
    pad_map = load_pad_map(lane)
    cf_names = {p["cfName"] for p in pad_map["pads"]}
    samples_dest = out_dir / "MPC500" / "BANK_A" / "PHONK" / "SAMPLES"
    samples_dest.mkdir(parents=True, exist_ok=True)

    wavs = sorted(from_dir.glob("**/*.wav")) + sorted(from_dir.glob("**/*.WAV"))
    assigned: dict[str, str] = {}
    unmapped: list[str] = []

    for wav in wavs:
        cf = match_cf_name(wav.name)
        if cf is None or cf in assigned:
            unmapped.append(str(wav.relative_to(from_dir)))
            continue
        dest = samples_dest / cf
        assigned[cf] = str(wav.relative_to(from_dir))
        if not dry_run:
            shutil.copy2(wav, dest)

    missing = sorted(cf_names - set(assigned.keys()))

    manifest = {
        "lane": lane,
        "defaultBpm": pad_map.get("defaultBpm"),
        "profile": {
            "os": "stock",
            "copy": "manual",
            "clock": "mpc_master",
        },
        "stagingRoot": str(out_dir),
        "cfSamplesDir": "MPC500/BANK_A/PHONK/SAMPLES",
        "assigned": assigned,
        "missingCfNames": missing,
        "unmappedSources": unmapped,
        "padMap": pad_map,
    }

    checklist_src = LANES / lane / "COPY_CHECKLIST.txt"
    if checklist_src.is_file() and not dry_run:
        shutil.copy2(checklist_src, out_dir / "COPY_CHECKLIST.txt")
        (out_dir / "MPC500" / "README.txt").write_text(
            checklist_src.read_text(encoding="utf-8"), encoding="utf-8"
        )

    if not dry_run:
        (out_dir / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")

    return manifest


def main() -> int:
    parser = argparse.ArgumentParser(description="Stage MPC 500 CF folders for manual copy")
    parser.add_argument("--lane", default="phonk", help="Lane id (subdir of mpc500/lanes/)")
    parser.add_argument("--from", dest="from_dir", required=True, type=Path, help="Source kit folder (WAVs)")
    parser.add_argument("--out", dest="out_dir", required=True, type=Path, help="Staging output directory")
    parser.add_argument("--dry-run", action="store_true", help="Report mapping only; no copies")
    args = parser.parse_args()

    from_dir = args.from_dir.expanduser().resolve()
    if not from_dir.is_dir():
        print(f"Not a directory: {from_dir}", file=sys.stderr)
        return 1

    out_dir = args.out_dir.expanduser().resolve()
    manifest = stage_lane(from_dir, out_dir, args.lane, dry_run=args.dry_run)
    print(json.dumps({k: manifest[k] for k in ("lane", "assigned", "missingCfNames", "unmappedSources")}, indent=2))
    return 0 if not manifest["missingCfNames"] else 2


if __name__ == "__main__":
    raise SystemExit(main())
