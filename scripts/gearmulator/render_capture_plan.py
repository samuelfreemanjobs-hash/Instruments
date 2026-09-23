#!/usr/bin/env python3
"""Run multiple multisample capture sessions from a TSV plan file."""
from __future__ import annotations

import argparse
import csv
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from _dawdreamer_session import render_session_grid


def parse_bool(value: str) -> bool:
    return value.strip().lower() in ("1", "true", "yes", "y")


def main() -> int:
    repo_root = Path(__file__).resolve().parents[2]
    parser = argparse.ArgumentParser(description="Execute multisample capture plan (TSV)")
    parser.add_argument(
        "--plan",
        type=Path,
        default=repo_root / "gearmulator-lane/multisamples/capture-plan.example.tsv",
    )
    parser.add_argument("--out-dir", type=Path, default=repo_root / "gearmulator-lane/multisamples/out")
    args = parser.parse_args()

    if not args.plan.is_file():
        print(f"Plan not found: {args.plan}", file=sys.stderr)
        return 1

    with args.plan.open(newline="", encoding="utf-8") as f:
        rows = list(csv.DictReader(f, delimiter="\t"))

    ran = 0
    for row in rows:
        if row.get("enabled", "true").strip() and not parse_bool(row.get("enabled", "true")):
            continue

        plugin = Path(row["plugin_path"]).expanduser()
        if not plugin.is_file() and not plugin.is_dir():
            print(f"Skip missing plugin: {plugin}", file=sys.stderr)
            continue

        preset_raw = row.get("preset_path", "").strip()
        preset = Path(preset_raw).expanduser() if preset_raw else None
        name = row["session_name"]
        dest = args.out_dir / name

        print(f"=== Plan row: {name} ({row.get('instrument_label', name)}) ===")
        manifest = render_session_grid(
            plugin,
            dest,
            program=int(row.get("program", "0")),
            note_start=int(row["note_start"]),
            note_end=int(row["note_end"]),
            note_step=int(row.get("note_step", "1")),
            velocity=int(row.get("velocity", "100")),
            seconds=float(row.get("seconds", "3")),
            sample_rate=int(row.get("sample_rate", "48000")),
            block_size=int(row.get("block_size", "512")),
            preset_path=preset,
            instrument_label=row.get("instrument_label", name),
            source_plugin=row.get("source_plugin", str(plugin)),
        )
        print(f"Manifest: {manifest}\n")
        ran += 1

    if ran == 0:
        print("No sessions rendered.", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
