#!/usr/bin/env python3
"""VST Preset Agent — factory bank verification and product map for JUCE instruments."""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[3]

PRODUCTS = [
    {
        "id": "jd-upgraded",
        "name": "JD Upgraded",
        "preset_header": REPO / "Source/Assets/FactoryPatchLibrary.h",
        "preset_pattern": r"kFactoryPatchCount\s*=\s*(\d+)",
        "wavetable_header": None,
    },
    {
        "id": "wave9090",
        "name": "WAVE-9090",
        "preset_header": REPO / "Wave9090/Source/Presets/PresetAgent.h",
        "preset_pattern": r"kFactoryPresetCount\s*=\s*(\d+)",
        "wavetable_header": REPO / "Wave9090/Source/DSP/WavetableBank.h",
        "wavetable_pattern": r"kWavetableCount\s*=\s*(\d+)",
    },
    {
        "id": "plugg-va",
        "name": "Plugg VA",
        "preset_header": REPO / "PluggVA/Source/Presets/PresetAgent.h",
        "preset_pattern": r"kFactoryPresetCount\s*=\s*(\d+)",
        "wavetable_header": None,
    },
    {
        "id": "sp1200",
        "name": "SP-1200",
        "preset_header": REPO / "SP1200/Source/Presets/FactoryProgramBank.h",
        "preset_pattern": r"kFactoryProgramCount\s*=\s*(\d+)",
        "wavetable_header": None,
    },
]


def _parse_count(path: Path, pattern: str) -> int | None:
    text = path.read_text(encoding="utf-8")
    match = re.search(pattern, text)
    return int(match.group(1)) if match else None


def self_test() -> dict[str, object]:
    verify = REPO / "scripts/vst/verify_factory_preset_counts.py"
    proc = subprocess.run(
        [sys.executable, str(verify)],
        cwd=REPO,
        capture_output=True,
        text=True,
        check=False,
    )
    products: list[dict[str, object]] = []
    for p in PRODUCTS:
        preset = _parse_count(p["preset_header"], p["preset_pattern"])
        wt = None
        if p.get("wavetable_header"):
            header = p["wavetable_header"].read_text(encoding="utf-8")
            classic = re.search(r"kClassicWavetableCount\s*=\s*(\d+)", header)
            trap = re.search(r"kTrapWavetableCount\s*=\s*(\d+)", header)
            if classic and trap:
                wt = int(classic.group(1)) + int(trap.group(1))
        products.append(
            {
                "id": p["id"],
                "factory_programs": preset,
                "wavetables": wt,
            }
        )
    return {
        "ok": proc.returncode == 0,
        "verify_stdout": proc.stdout.strip(),
        "verify_stderr": proc.stderr.strip(),
        "products": products,
        "doc": "disklordz/agents/profit/vst-preset-agent/knowledge/SYNTH_DSP_PRIMER.md",
    }


def main() -> int:
    parser = argparse.ArgumentParser(description="VST Preset Agent (Instruments monorepo)")
    parser.add_argument("--self-test", action="store_true")
    parser.add_argument("--json", action="store_true", help="Print product map JSON")
    args = parser.parse_args()

    if args.self_test or args.json:
        payload = self_test()
        print(json.dumps(payload, indent=2))
        return 0 if payload.get("ok") else 1

    parser.print_help()
    return 0


if __name__ == "__main__":
    sys.exit(main())
