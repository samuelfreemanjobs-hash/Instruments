#!/usr/bin/env python3
"""Drop minimal presets until verify cross-category gate would pass."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[1]
BANK = ROOT / "assets/factory/preset_bank.json"
CLI = ROOT / "build" / "TrapLordSynthRenderCLI_artefacts" / "Release" / "TrapLordSynthRenderCLI"
PRIORITY = frozenset({"Pads", "Synth Keys", "Leads", "Swells"})

sys.path.insert(0, str(ROOT / "tools"))
from trap_lord_audio_metrics import CROSS_CATEGORY_CORR_MAX, category_feature_vector  # noqa: E402


def measure(bank: dict) -> tuple[int, tuple[float, int, int], list[np.ndarray], list[str], list[str]]:
    d = ROOT / "build/cross-dedupe"
    d.mkdir(exist_ok=True)
    vecs: list[np.ndarray] = []
    cats: list[str] = []
    names: list[str] = []
    for i, p in enumerate(bank["presets"]):
        jp = d / f"{i}.json"
        wav = d / f"{i}.wav"
        jp.write_text(json.dumps(p), encoding="utf-8")
        subprocess.check_call(
            [str(CLI), "category-probe-json", str(jp), str(wav)],
            cwd=ROOT,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
        )
        vecs.append(category_feature_vector(wav))
        cats.append(p["category"])
        names.append(p.get("name", ""))
    worst = (0.0, -1, -1)
    fails = 0
    for a in range(len(vecs)):
        for b in range(a + 1, len(vecs)):
            if cats[a] == cats[b]:
                continue
            c = float(np.dot(vecs[a], vecs[b]))
            if c > worst[0]:
                worst = (c, a, b)
            if c > CROSS_CATEGORY_CORR_MAX:
                fails += 1
    return fails, worst, vecs, names, cats


def main() -> int:
    removals: list[dict] = []
    for _ in range(30):
        bank = json.loads(BANK.read_text(encoding="utf-8"))
        fails, worst, _v, names, cats = measure(bank)
        if fails == 0:
            print(f"Cross-dedupe clean: {len(bank['presets'])} presets")
            break
        a, b = worst[1], worst[2]
        drop_idx = b
        if cats[b] in PRIORITY and cats[a] not in PRIORITY:
            drop_idx = a
        elif cats[a] in PRIORITY and cats[b] not in PRIORITY:
            drop_idx = b
        elif cats[a] not in PRIORITY and cats[b] not in PRIORITY:
            drop_idx = b
        other = a if drop_idx == b else b
        name = names[drop_idx]
        removals.append(
            {
                "category": cats[drop_idx],
                "name": name,
                "reason": f"cross_dedupe corr {worst[0]:.4f} vs {names[other]} ({cats[other]})",
            }
        )
        presets = list(bank["presets"])
        presets.pop(drop_idx)
        bank["presets"] = presets
        bank["count"] = len(presets)
        BANK.write_text(json.dumps(bank, indent=2), encoding="utf-8")
        print(f"Removed {name!r} (failures={fails})", flush=True)
    else:
        raise SystemExit("Could not clear cross-category failures in 30 steps")
    report = ROOT / "build" / "priority-cull-removals.json"
    existing = json.loads(report.read_text(encoding="utf-8")) if report.exists() else []
    report.write_text(json.dumps(existing + removals, indent=2), encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())