#!/usr/bin/env python3
"""Verify extracted drum-machine folders under drummachines/samples/."""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SAMPLES = ROOT / "samples"


def main() -> int:
    if not SAMPLES.is_dir():
        print(f"Missing samples dir: {SAMPLES}", file=sys.stderr)
        return 1
    dirs = sorted(p.name for p in SAMPLES.iterdir() if p.is_dir() and not p.name.startswith("."))
    if not dirs:
        print("No machine folders found. Run ./scripts/fetch_and_extract.sh first.", file=sys.stderr)
        return 1
    print(f"Top-level machine folders ({len(dirs)}):")
    for name in dirs:
        wav_count = sum(1 for _ in SAMPLES.joinpath(name).rglob("*.wav"))
        print(f"  - {name} ({wav_count} .wav)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
