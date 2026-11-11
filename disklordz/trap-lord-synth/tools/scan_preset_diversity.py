#!/usr/bin/env python3
"""Cluster factory presets by spectral+envelope similarity (probe renders)."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[1]
CLI = ROOT / "build" / "TrapLordSynthRenderCLI_artefacts" / "Release" / "TrapLordSynthRenderCLI"
BANK = ROOT / "assets" / "factory" / "preset_bank.json"
PROBE_DIR = Path("/opt/cursor/artifacts/factory-preset-scan")
OUT = Path("/opt/cursor/artifacts/factory-preset-diversity.json")

sys.path.insert(0, str(ROOT / "tools"))
from preset_probe import render_category_probe  # noqa: E402
from trap_lord_audio_metrics import gate_feature_vector  # noqa: E402

DUPLICATE_CORR = 0.92
BATCH = 500


def main() -> int:
    bank = json.loads(BANK.read_text(encoding="utf-8"))
    presets = bank["presets"]
    if not CLI.exists():
        subprocess.check_call(
            ["cmake", "--build", str(ROOT / "build"), "-j", "--target", "TrapLordSynthRenderCLI"],
            cwd=ROOT,
        )

    by_cat: dict[str, list[int]] = {}
    for i, p in enumerate(presets):
        by_cat.setdefault(p["category"], []).append(i)

    PROBE_DIR.mkdir(parents=True, exist_ok=True)
    vectors: list[np.ndarray] = []
    for i in range(len(presets)):
        wav = PROBE_DIR / f"preset_{i:03d}.wav"
        if not wav.exists():
            render_category_probe(presets[i], i, PROBE_DIR)
        vectors.append(gate_feature_vector(wav, presets[i]))

    n = len(vectors)
    parent = list(range(n))

    def find(a: int) -> int:
        while parent[a] != a:
            parent[a] = parent[parent[a]]
            a = parent[a]
        return a

    def unite(a: int, b: int) -> None:
        ra, rb = find(a), find(b)
        if ra != rb:
            parent[rb] = ra

    for cat, cat_indices in by_cat.items():
        for ai, i in enumerate(cat_indices):
            for j in cat_indices[ai + 1 :]:
                corr = float(np.dot(vectors[i], vectors[j]))
                if corr >= DUPLICATE_CORR:
                    unite(i, j)

    clusters: dict[int, list[int]] = {}
    for i in range(n):
        clusters.setdefault(find(i), []).append(i)

    multi = [sorted(v) for v in clusters.values() if len(v) > 1]
    multi.sort(key=len, reverse=True)
    effective_duplicates = sum(len(c) - 1 for c in multi)

    payload = {
        "preset_count": n,
        "duplicate_threshold_corr": DUPLICATE_CORR,
        "near_duplicate_clusters": len(multi),
        "effectively_duplicate_presets": effective_duplicates,
        "largest_clusters": [
            {
                "size": len(c),
                "indices": c[:12],
                "names": [presets[i]["name"] for i in c[:8]],
                "categories": list({presets[i]["category"] for i in c}),
            }
            for c in multi[:40]
        ],
    }
    OUT.write_text(json.dumps(payload, indent=2), encoding="utf-8")
    print(json.dumps({k: payload[k] for k in payload if k != "largest_clusters"}, indent=2))
    print(f"Wrote {OUT}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
