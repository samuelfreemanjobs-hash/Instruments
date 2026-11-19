#!/usr/bin/env python3
"""Preset Agent smoke: each shipped VST instrument must expose >= 256 factory programs."""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
TARGET = 512

WAVE_WT_HEADER = ROOT / "Wave9090/Source/DSP/WavetableBank.h"

CHECKS: list[tuple[str, Path, str]] = [
    (
        "JD Upgraded",
        ROOT / "Source/Assets/FactoryPatchLibrary.h",
        r"kFactoryPatchCount\s*=\s*(\d+)",
    ),
    (
        "WAVE-9090",
        ROOT / "Wave9090/Source/Presets/PresetAgent.h",
        r"kFactoryPresetCount\s*=\s*(\d+)",
    ),
    (
        "Plugg VA",
        ROOT / "PluggVA/Source/Presets/PresetAgent.h",
        r"kFactoryPresetCount\s*=\s*(\d+)",
    ),
    (
        "SP-1200",
        ROOT / "SP1200/Source/Presets/FactoryProgramBank.h",
        r"kFactoryProgramCount\s*=\s*(\d+)",
    ),
]


def main() -> int:
    failed = False
    wt_text = WAVE_WT_HEADER.read_text(encoding="utf-8")
    classic = re.search(r"kClassicWavetableCount\s*=\s*(\d+)", wt_text)
    trap = re.search(r"kTrapWavetableCount\s*=\s*(\d+)", wt_text)
    if classic and trap:
        wt_total = int(classic.group(1)) + int(trap.group(1))
        trap_total = int(trap.group(1))
        if trap_total < 50:
            print(f"FAIL WAVE-9090 wavetables: trap count {trap_total} < 50")
            failed = True
        else:
            print(f"OK   WAVE-9090 wavetables: {wt_total} (incl. {trap_total} trap)")
    else:
        print("FAIL WAVE-9090: could not parse wavetable counts")
        failed = True

    for label, path, pattern in CHECKS:
        text = path.read_text(encoding="utf-8")
        match = re.search(pattern, text)
        if not match:
            print(f"FAIL {label}: could not parse count in {path}")
            failed = True
            continue
        count = int(match.group(1))
        if count < TARGET:
            print(f"FAIL {label}: {count} < {TARGET} ({path})")
            failed = True
        else:
            print(f"OK   {label}: {count}")

    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
