#!/usr/bin/env python3
"""Pick the two most distinct presets per category using category-probe features."""

from __future__ import annotations

import hashlib
import json
import shutil
import subprocess
import sys
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[1]
CLI = ROOT / "build" / "TrapLordSynthRenderCLI_artefacts" / "Release" / "TrapLordSynthRenderCLI"
BANK = ROOT / "assets/factory/preset_bank.json"
PROBE_DIR = ROOT / "build" / "category-probes"
MANIFEST = ROOT / "assets" / "factory" / "audition_manifest.json"

sys.path.insert(0, str(ROOT / "tools"))
from preset_probe import probe_wav_path, render_category_probe  # noqa: E402
from trap_lord_audio_metrics import (  # noqa: E402
    AUDITION_PEAK_MIN_LINEAR,
    CROSS_CATEGORY_CORR_MAX,
    analyze_file,
    category_feature_vector,
    peak_passes_audible_gate,
)


def bank_fingerprint() -> str:
    return hashlib.sha256(BANK.read_bytes()).hexdigest()


def sync_probe_cache() -> None:
    meta = PROBE_DIR / ".bank_fingerprint"
    fp = bank_fingerprint()
    if meta.exists() and meta.read_text(encoding="utf-8").strip() == fp:
        return
    shutil.rmtree(PROBE_DIR, ignore_errors=True)
    PROBE_DIR.mkdir(parents=True, exist_ok=True)
    meta.write_text(fp, encoding="utf-8")


def audition_peak(idx: int) -> float:
    wav = PROBE_DIR / f"aud_check_{idx:04d}.wav"
    subprocess.check_call([str(CLI), "audition", str(idx), str(wav)], cwd=ROOT)
    return analyze_file(wav).peak


def is_audible_preset(idx: int, presets: list[dict]) -> bool:
    pk = audition_peak(idx)
    if not peak_passes_audible_gate(pk):
        return False
    wav = probe_wav_path(PROBE_DIR, idx)
    if not wav.exists():
        render_category_probe(presets[idx], idx, PROBE_DIR)
    probe_pk = analyze_file(wav).peak
    return peak_passes_audible_gate(probe_pk)


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


def ranked_audible_indices(idxs: list[int], presets: list[dict]) -> list[int]:
    scored: list[tuple[float, int]] = []
    for idx in idxs:
        if not is_audible_preset(idx, presets):
            continue
        wav = probe_wav_path(PROBE_DIR, idx)
        if not wav.exists():
            render_category_probe(presets[idx], idx, PROBE_DIR)
        scored.append((audition_peak(idx), idx))
    scored.sort(key=lambda t: t[0], reverse=True)
    return [i for _, i in scored]


def pick_pair_for_category(idxs: list[int], presets: list[dict]) -> list[int]:
    vecs: dict[int, np.ndarray] = {}
    for idx in ranked_audible_indices(idxs, presets):
        wav = probe_wav_path(PROBE_DIR, idx)
        vecs[idx] = category_feature_vector(wav)
        if len(vecs) >= max(2, len(idxs)):
            break
    audible = list(vecs.keys())
    if len(audible) < 1:
        raise SystemExit(f"No audible presets in category (indices {idxs[:5]}…)")
    if len(audible) == 1:
        return audible[:1]
    ranked_pairs: list[tuple[float, tuple[int, int]]] = []
    for i, a in enumerate(audible):
        for b in audible[i + 1 :]:
            corr = float(np.dot(vecs[a], vecs[b]))
            ranked_pairs.append((1.0 - corr, (a, b)))
    ranked_pairs.sort(key=lambda t: t[0], reverse=True)
    _score, (a, b) = ranked_pairs[0]
    return [a, b]


def cross_category_ok(
    chosen: dict[str, list[int]], vecs: dict[int, np.ndarray], cat_a: str, cat_b: str
) -> bool:
    for ia in chosen.get(cat_a, []):
        for ib in chosen.get(cat_b, []):
            if float(np.dot(vecs[ia], vecs[ib])) > CROSS_CATEGORY_CORR_MAX:
                return False
    return True


def main() -> int:
    sync_probe_cache()
    bank = json.loads(BANK.read_text(encoding="utf-8"))
    presets = bank["presets"]
    by_cat: dict[str, list[int]] = {}
    for i, p in enumerate(presets):
        by_cat.setdefault(p["category"], []).append(i)

    vecs_all: dict[int, np.ndarray] = {}
    auditions: list[dict] = []
    chosen: dict[str, list[int]] = {}

    for category in sorted(by_cat):
        picks = pick_pair_for_category(by_cat[category], presets)
        # If this category is too close to an earlier pick (e.g. Bells vs Swells), try alt pairs.
        idxs = ranked_audible_indices(by_cat[category], presets)
        vecs: dict[int, np.ndarray] = {}
        for idx in idxs:
            wav = probe_wav_path(PROBE_DIR, idx)
            if not wav.exists():
                render_category_probe(presets[idx], idx, PROBE_DIR)
            vecs[idx] = category_feature_vector(wav)
            vecs_all[idx] = vecs[idx]

        best_picks = picks
        if len(idxs) >= 2:
            pairs: list[tuple[float, list[int]]] = []
            for i, a in enumerate(idxs):
                for b in idxs[i + 1 :]:
                    corr = float(np.dot(vecs[a], vecs[b]))
                    pairs.append((1.0 - corr, [a, b]))
            pairs.sort(key=lambda t: t[0], reverse=True)
            for _sep, candidate in pairs:
                trial = dict(chosen)
                trial[category] = candidate
                conflict = False
                for other in trial:
                    if other == category:
                        continue
                    if not cross_category_ok(trial, vecs_all, category, other):
                        conflict = True
                        break
                if not conflict:
                    best_picks = candidate
                    break
        chosen[category] = best_picks
        slot_count = 2 if len(by_cat[category]) >= 2 else 1
        for slot, idx in enumerate(best_picks[:slot_count], start=1):
            pk = audition_peak(idx)
            if not peak_passes_audible_gate(pk):
                raise SystemExit(f"Manifest reject: {presets[idx]['name']} peak {pk}")
            auditions.append(
                {
                    "category": category,
                    "slot": slot,
                    "index": idx,
                    "name": presets[idx]["name"],
                    "subStyle": presets[idx].get("subStyle", ""),
                }
            )

    payload = {
        "auditions": auditions,
        "wavCount": len(auditions),
        "selection": "farthest_pair_audible_cross_checked",
        "audiblePeakMinLinear": AUDITION_PEAK_MIN_LINEAR,
    }
    MANIFEST.write_text(json.dumps(payload, indent=2), encoding="utf-8")
    print(f"Wrote {MANIFEST} ({len(auditions)} slots)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
