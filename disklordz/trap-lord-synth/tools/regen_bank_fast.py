#!/usr/bin/env python3
"""Regenerate bank at pragmatic per-category targets (post DSP gate fixes)."""

from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))

from generate_preset_bank import run_generation, write_bank  # noqa: E402

BANK = ROOT / "assets/factory/preset_bank.json"
SEED = 301508

# Enough candidates for diversity cull without full 500-target marathon.
FAST_TARGETS = {
    "Pads": 22,
    "Leads": 18,
    "Synth Keys": 16,
    "Swells": 20,
    "Brass": 14,
    "Strings": 18,
    "Church Keys/EP": 12,
    "Organ": 12,
    "Bells": 12,
    "Bass": 14,
    "Choir": 18,
    "Hits/FX": 14,
}


def main() -> int:
    presets, meta = run_generation(SEED, FAST_TARGETS, audio_gate=True)
    write_bank(BANK, presets, meta["achievedQuotas"], meta["gate"])
    report = Path("/opt/cursor/artifacts/factory-diversity-gate-report.json")
    report.parent.mkdir(parents=True, exist_ok=True)
    report.write_text(json.dumps({"count": len(presets), "mode": "fast_targets", **meta}, indent=2))
    print(f"Wrote {len(presets)} presets")
    return 0


if __name__ == "__main__":
    sys.exit(main())
