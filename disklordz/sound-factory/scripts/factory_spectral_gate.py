#!/usr/bin/env python3
"""Optional SpectralDiff gate vs category golden WAV."""

from __future__ import annotations

import argparse
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
GOLDEN = ROOT / "golden" / "category_refs"
SPECTRAL = Path(__file__).resolve().parents[3] / "build" / "SpectralDiff"


def gate(test_wav: Path, category: str, max_rms_db: float = 12.0) -> bool:
    ref = GOLDEN / f"{category}.wav"
    if not ref.is_file() or not SPECTRAL.is_file():
        return True
    proc = subprocess.run(
        [str(SPECTRAL), str(ref), str(test_wav), "--max-rms-db", str(max_rms_db)],
        capture_output=True,
        text=True,
    )
    return proc.returncode == 0


def main() -> None:
    p = argparse.ArgumentParser()
    p.add_argument("wav", type=Path)
    p.add_argument("--category", default="trap_lead")
    p.add_argument("--max-rms-db", type=float, default=12.0)
    args = p.parse_args()
    ok = gate(args.wav, args.category, args.max_rms_db)
    if not ok:
        raise SystemExit(1)
    print('{"pass": true}')


if __name__ == "__main__":
    main()
