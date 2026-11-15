#!/usr/bin/env python3
"""Verify no within-category gate correlation exceeds threshold."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[1]
CLI = ROOT / "build" / "TrapLordSynthRenderCLI_artefacts" / "Release" / "TrapLordSynthRenderCLI"
BANK = ROOT / "assets/factory/preset_bank.json"
PROBE_DIR = ROOT / "build" / "gate-verify"
THRESH = 0.92

sys.path.insert(0, str(ROOT / "tools"))
from preset_probe import render_category_probe  # noqa: E402
from trap_lord_audio_metrics import gate_feature_vector  # noqa: E402


def main() -> int:
    bank = json.loads(BANK.read_text(encoding="utf-8"))
    presets = bank["presets"]
    by_cat: dict[str, list[int]] = {}
    for i, p in enumerate(presets):
        by_cat.setdefault(p["category"], []).append(i)

    PROBE_DIR.mkdir(parents=True, exist_ok=True)
    failures = []
    report: dict[str, dict] = {}

    for cat, indices in sorted(by_cat.items()):
        vecs: list[np.ndarray] = []
        for i in indices:
            wav = PROBE_DIR / f"p_{i:04d}.wav"
            if not wav.exists():
                render_category_probe(presets[i], i, PROBE_DIR)
            vecs.append(gate_feature_vector(wav, presets[i]))
        worst = 0.0
        worst_pair = (-1, -1)
        for ai in range(len(indices)):
            for bj in range(ai + 1, len(indices)):
                c = float(np.dot(vecs[ai], vecs[bj]))
                i, j = indices[ai], indices[bj]
                if c > worst:
                    worst = c
                    worst_pair = (i, j)
                if c > THRESH:
                    failures.append((cat, i, j, c))
        report[cat] = {"count": len(indices), "max_corr": worst, "worst_pair": worst_pair}

    out = Path("/opt/cursor/artifacts/factory-gate-verify.json")
    out.write_text(json.dumps({"threshold": THRESH, "failures": failures, "categories": report}, indent=2), encoding="utf-8")
    print(json.dumps({"failures": len(failures), "categories": len(report)}, indent=2))
    if failures:
        print("FAIL pairs:", failures[:5])
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
