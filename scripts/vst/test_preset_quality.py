#!/usr/bin/env python3
"""Self-check: daily gold presets must meet MIN_DAILY_SCORE."""

from __future__ import annotations

import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(REPO / "scripts/vst"))

from preset_quality import MIN_DAILY_SCORE, daily_plugg_presets, daily_wave_presets  # noqa: E402


def main() -> int:
    day = "20990101"
    wave = daily_wave_presets(day)
    plugg = daily_plugg_presets(day)
    if len(wave) != 8 or len(plugg) != 8:
        print(f"FAIL: expected 8 slots, got wave={len(wave)} plugg={len(plugg)}")
        return 1
    for p in wave + plugg:
        if p.score < MIN_DAILY_SCORE:
            print(f"FAIL: {p.name} score {p.score} < {MIN_DAILY_SCORE}")
            return 1
    print(f"OK: 16 gold presets >= {MIN_DAILY_SCORE}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
