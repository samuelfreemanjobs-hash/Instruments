#!/usr/bin/env python3
"""Cull factory bank to audio-distinct sets (category phrase probes, corr < 0.92)."""

from __future__ import annotations

import hashlib
import json
import os
import shutil
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
from trap_lord_audio_metrics import (  # noqa: E402
    CROSS_CATEGORY_CORR_MAX,
    analyze_file,
    category_feature_vector,
    category_spectral_passes,
    peak_passes_audible_gate,
    preset_audible_at_notes,
)

AUDIBLE_DIR = ROOT / "build" / "audio-cull-audible"


def main() -> int:
    bank = json.loads(BANK.read_text(encoding="utf-8"))
    presets = bank["presets"]
    target = bank.get("targetQuotas") or bank.get("categoryQuotas") or {}
    by_cat: dict[str, list[int]] = {}
    for i, p in enumerate(presets):
        by_cat.setdefault(p["category"], []).append(i)

    bank_fp = hashlib.sha256(BANK.read_bytes()).hexdigest()
    fp_meta = PROBE_DIR / ".bank_fingerprint"
    if not fp_meta.exists() or fp_meta.read_text(encoding="utf-8").strip() != bank_fp:
        shutil.rmtree(PROBE_DIR, ignore_errors=True)
        fp_meta.parent.mkdir(parents=True, exist_ok=True)
        fp_meta.write_text(bank_fp, encoding="utf-8")
    PROBE_DIR.mkdir(parents=True, exist_ok=True)
    achieved: dict[str, int] = {}
    keep_indices: list[int] = []
    global_kept: list[tuple[str, np.ndarray]] = []
    spectral_rejects = 0
    cross_rejects = 0
    audible_rejects = 0

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
        filtered_idxs: list[int] = []
        aud_cache: dict[int, bool] = {}

        def aud_ok_index(i: int) -> bool:
            if i in aud_cache:
                return aud_cache[i]
            ok, _pk, _reason = preset_audible_at_notes(CLI, ROOT, i, AUDIBLE_DIR, category=cat)
            aud_cache[i] = ok
            return ok

        for i in idxs:
            wav = PROBE_DIR / f"p_{i:04d}.wav"
            if i not in vec_map:
                vec_map[i] = category_feature_vector(wav)
            metrics = analyze_file(wav, category=cat)
            if not peak_passes_audible_gate(metrics.peak):
                audible_rejects += 1
                continue
            if not aud_ok_index(i):
                audible_rejects += 1
                continue
            ok, _reason = category_spectral_passes(metrics, cat)
            if not ok:
                spectral_rejects += 1
                continue
            v = vec_map[i]
            too_close = False
            for other_cat, ov in global_kept:
                if other_cat == cat:
                    continue
                if float(np.dot(v, ov)) > CROSS_CATEGORY_CORR_MAX:
                    too_close = True
                    break
            if too_close:
                cross_rejects += 1
                continue
            filtered_idxs.append(i)
            vecs.append(v)
        print(f"{cat}: probing {len(idxs)} presets, eligible {len(filtered_idxs)}…", flush=True)
        quota = int(target.get(cat, len(idxs)))
        fake = [{"bankIndex": i} for i in filtered_idxs]
        picked, _ = select_diverse_presets(fake, vecs, min(quota, len(filtered_idxs)), CORR_MAX, None)
        cat_keep = [p["bankIndex"] for p in picked]
        for bi in cat_keep:
            global_kept.append((cat, vec_map[bi]))
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
            "crossCategoryCorrMax": CROSS_CATEGORY_CORR_MAX,
            "mode": "post_cull_category_probe_json",
            "feature": "category_probe_audio",
            "spectralRejects": spectral_rejects,
            "audibleRejects": audible_rejects,
            "audiblePeakMinDbfs": -30.0,
            "crossCategoryRejects": cross_rejects,
        },
        "categoryQuotas": achieved,
        "presets": new_presets,
    }
    BANK.write_text(json.dumps(out, indent=2), encoding="utf-8")
    report = ROOT / "build" / "factory-audio-cull-report.json"
    report.parent.mkdir(parents=True, exist_ok=True)
    report.write_text(json.dumps({"achievedQuotas": achieved, "total": len(new_presets)}, indent=2), encoding="utf-8")
    artifacts_report = Path("/opt/cursor/artifacts/factory-audio-cull-report.json")
    try:
        artifacts_report.parent.mkdir(parents=True, exist_ok=True)
        artifacts_report.write_text(report.read_text(encoding="utf-8"), encoding="utf-8")
    except OSError as err:
        print(f"WARN: could not write {artifacts_report}: {err}")
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
