#!/usr/bin/env python3
"""Cull factory bank to audio-distinct sets (category phrase probes, corr < 0.92)."""

from __future__ import annotations

import json
import os
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[1]
CLI = ROOT / "build" / "TrapLordSynthRenderCLI_artefacts" / "Release" / "TrapLordSynthRenderCLI"
BANK = ROOT / "assets/factory/preset_bank.json"
PROBE_DIR = ROOT / "build" / "audio-cull-probes"
CORR_MAX = 0.92

sys.path.insert(0, str(ROOT / "tools"))
from generate_preset_bank import select_diverse_presets  # noqa: E402
from trap_lord_audio_metrics import category_feature_vector  # noqa: E402


def main() -> int:
    bank = json.loads(BANK.read_text(encoding="utf-8"))
    presets = bank["presets"]
    target = bank.get("targetQuotas") or bank.get("categoryQuotas") or {}
    by_cat: dict[str, list[int]] = {}
    for i, p in enumerate(presets):
        by_cat.setdefault(p["category"], []).append(i)

    PROBE_DIR.mkdir(parents=True, exist_ok=True)
    achieved: dict[str, int] = {}
    keep_indices: list[int] = []

    def probe_index(i: int, preset: dict) -> tuple[int, np.ndarray]:
        wav = PROBE_DIR / f"p_{i:04d}.wav"
        json_path = PROBE_DIR / "json" / f"p_{i:04d}.json"
        if not wav.exists():
            json_path.parent.mkdir(parents=True, exist_ok=True)
            json_path.write_text(json.dumps(preset), encoding="utf-8")
            subprocess.check_call([str(CLI), "category-probe-json", str(json_path), str(wav)], cwd=ROOT)
        return i, category_feature_vector(wav)

    workers = min(6, max(2, (os.cpu_count() or 4)))

    for cat in sorted(by_cat.keys()):
        idxs = by_cat[cat]
        vec_map: dict[int, np.ndarray] = {}
        need_probe = [(i, presets[i]) for i in idxs if not (PROBE_DIR / f"p_{i:04d}.wav").exists()]
        if need_probe:
            with ThreadPoolExecutor(max_workers=workers) as pool:
                futures = [pool.submit(probe_index, i, p) for i, p in need_probe]
                for fut in as_completed(futures):
                    i, v = fut.result()
                    vec_map[i] = v
        vecs = []
        for i in idxs:
            wav = PROBE_DIR / f"p_{i:04d}.wav"
            if i not in vec_map:
                vec_map[i] = category_feature_vector(wav)
            vecs.append(vec_map[i])
        print(f"{cat}: probing {len(idxs)} presets…", flush=True)
        quota = int(target.get(cat, len(idxs)))
        fake = [{"bankIndex": i} for i in idxs]
        picked, _ = select_diverse_presets(fake, vecs, min(quota, len(idxs)), CORR_MAX, None)
        cat_keep = [p["bankIndex"] for p in picked]
        achieved[cat] = len(cat_keep)
        keep_indices.extend(cat_keep)

    keep_indices.sort()
    new_presets = [presets[i] for i in keep_indices]
    out = {
        "version": 4,
        "count": len(new_presets),
        "targetQuotas": target,
        "achievedQuotas": achieved,
        "diversityGate": {
            "audioCorrMax": CORR_MAX,
            "mode": "post_cull_category_probe_json",
            "feature": "category_probe_audio",
        },
        "categoryQuotas": achieved,
        "presets": new_presets,
    }
    BANK.write_text(json.dumps(out, indent=2), encoding="utf-8")
    report = Path("/opt/cursor/artifacts/factory-audio-cull-report.json")
    report.write_text(json.dumps({"achievedQuotas": achieved, "total": len(new_presets)}, indent=2), encoding="utf-8")
    print(f"Culled bank: {len(new_presets)} presets (was {len(presets)})")
    for cat, n in sorted(achieved.items()):
        print(f"  {cat}: {n} audio-distinct (target {target.get(cat)})")
    subprocess.check_call(
        ["cmake", "--build", str(ROOT / "build"), "-j", "--target", "TrapLordSynthFactoryData", "TrapLordSynthRenderCLI"],
        cwd=ROOT,
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
