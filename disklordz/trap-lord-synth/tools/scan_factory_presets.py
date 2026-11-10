#!/usr/bin/env python3
"""Render middle-C probe for every factory preset; report DC / sub-30 Hz failures."""

from __future__ import annotations

import csv
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CLI = ROOT / "build" / "TrapLordSynthRenderCLI_artefacts" / "Release" / "TrapLordSynthRenderCLI"
BANK = ROOT / "assets" / "factory" / "preset_bank.json"
OUT_DIR = Path("/opt/cursor/artifacts/factory-preset-scan")
REPORT = Path("/opt/cursor/artifacts/factory-preset-dc-report.csv")

sys.path.insert(0, str(ROOT / "tools"))
from trap_lord_audio_metrics import analyze_file  # noqa: E402

DC_MAX = 0.01
SUB30_MAX = 0.12


def main() -> int:
    if not CLI.exists():
        subprocess.check_call(
            ["cmake", "--build", str(ROOT / "build"), "-j", "--target", "TrapLordSynthRenderCLI"],
            cwd=ROOT,
        )
    bank = json.loads(BANK.read_text(encoding="utf-8"))
    presets = bank["presets"]
    OUT_DIR.mkdir(parents=True, exist_ok=True)

    rows: list[dict] = []
    failures: list[dict] = []

    for i, preset in enumerate(presets):
        wav = OUT_DIR / f"preset_{i:03d}.wav"
        subprocess.check_call([str(CLI), "preset-probe", str(i), str(wav)], cwd=ROOT)
        m = analyze_file(wav)
        row = {
            "index": i,
            "name": preset["name"],
            "category": preset["category"],
            "peak": m.peak,
            "rms": m.rms,
            "dc_offset": m.dc_offset,
            "centroid_hz": m.centroid_hz,
            "sub30_ratio": m.sub30_energy_ratio,
        }
        fail_reasons = []
        if abs(m.dc_offset) > DC_MAX:
            fail_reasons.append(f"dc={m.dc_offset:.4f}")
        if m.sub30_energy_ratio > SUB30_MAX:
            fail_reasons.append(f"sub30={m.sub30_energy_ratio:.3f}")
        if m.peak < 1e-4:
            fail_reasons.append("silent")
        row["pass"] = not fail_reasons
        row["reasons"] = ";".join(fail_reasons)
        rows.append(row)
        if fail_reasons:
            failures.append(row)
        if (i + 1) % 25 == 0:
            print(f"scanned {i + 1}/{len(presets)}")

    with REPORT.open("w", newline="", encoding="utf-8") as f:
        w = csv.DictWriter(f, fieldnames=list(rows[0].keys()))
        w.writeheader()
        w.writerows(rows)

    summary = Path("/opt/cursor/artifacts/factory-preset-dc-summary.json")
    summary.write_text(
        json.dumps(
            {
                "total": len(presets),
                "failures": len(failures),
                "failed_indices": [r["index"] for r in failures],
            },
            indent=2,
        ),
        encoding="utf-8",
    )
    print(f"Failures: {len(failures)} / {len(presets)} -> {REPORT}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
