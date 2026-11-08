#!/usr/bin/env python3
"""Preset Agent smoke: each shipped VST instrument must expose >= 256 factory programs."""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
TARGET = 256

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
