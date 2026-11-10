#!/usr/bin/env python3
"""Pick the two most distinct presets per category using category-probe features."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[1]
CLI = ROOT / "build" / "TrapLordSynthRenderCLI_artefacts" / "Release" / "TrapLordSynthRenderCLI"
BANK = ROOT / "assets" / "factory" / "preset_bank.json"
PROBE_DIR = ROOT / "build" / "category-probes"
MANIFEST = ROOT / "assets" / "factory" / "audition_manifest.json"

sys.path.insert(0, str(ROOT / "tools"))
from preset_probe import probe_wav_path, render_category_probe  # noqa: E402
from trap_lord_audio_metrics import category_feature_vector  # noqa: E402


def farthest_pair(vectors: dict[int, np.ndarray]) -> tuple[int, int]:
    indices = list(vectors.keys())
    if len(indices) < 2:
        return indices[0], indices[0]
    best = (indices[0], indices[1])
    best_score = -1.0
    for i, a in enumerate(indices):
        for b in indices[i + 1 :]:
            corr = float(np.dot(vectors[a], vectors[b]))
            score = 1.0 - corr
            if score > best_score:
                best_score = score
                best = (a, b)
    return best


def main() -> int:
    bank = json.loads(BANK.read_text(encoding="utf-8"))
    presets = bank["presets"]
    by_cat: dict[str, list[int]] = {}
    for i, p in enumerate(presets):
        by_cat.setdefault(p["category"], []).append(i)

    PROBE_DIR.mkdir(parents=True, exist_ok=True)
    auditions = []
    for category in sorted(by_cat):
        vecs: dict[int, np.ndarray] = {}
        for idx in by_cat[category]:
            wav = probe_wav_path(PROBE_DIR, idx)
            if not wav.exists():
                render_category_probe(presets[idx], idx, PROBE_DIR)
            vecs[idx] = category_feature_vector(wav)
        a, b = farthest_pair(vecs)
        picks = [a] if a == b else [a, b]
        for slot, idx in enumerate(picks[:2], start=1):
            auditions.append(
                {
                    "category": category,
                    "slot": slot,
                    "index": idx,
                    "name": presets[idx]["name"],
                    "subStyle": presets[idx].get("subStyle", ""),
                }
            )

    payload = {"auditions": auditions, "wavCount": len(auditions), "selection": "farthest_pair_category_probe"}
    MANIFEST.write_text(json.dumps(payload, indent=2), encoding="utf-8")
    print(f"Wrote {MANIFEST} ({len(auditions)} slots)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
