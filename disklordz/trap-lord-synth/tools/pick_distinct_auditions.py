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

_AUDITION_PEAK_CACHE: dict[int, float] = {}
# Enough distinct pairs for cross-category checks without scanning every preset.
_MAX_AUDIBLE_CANDIDATES_PER_CATEGORY = 16

sys.path.insert(0, str(ROOT / "tools"))
from preset_probe import probe_wav_path, render_category_probe  # noqa: E402
from trap_lord_audio_metrics import (  # noqa: E402
    AUDITION_PEAK_MIN_LINEAR,
    CROSS_CATEGORY_CORR_MAX,
    analyze_file,
    category_feature_vector,
    category_spectral_passes_audition,
    peak_passes_audible_gate,
)


def bank_fingerprint() -> str:
    return hashlib.sha256(BANK.read_bytes()).hexdigest()


def sync_probe_cache() -> None:
    meta = PROBE_DIR / ".bank_fingerprint"
    fp = bank_fingerprint()
    if meta.exists() and meta.read_text(encoding="utf-8").strip() == fp:
        return
    _AUDITION_PEAK_CACHE.clear()
    shutil.rmtree(PROBE_DIR, ignore_errors=True)
    PROBE_DIR.mkdir(parents=True, exist_ok=True)
    meta.write_text(fp, encoding="utf-8")


def audition_peak(idx: int) -> float:
    cached = _AUDITION_PEAK_CACHE.get(idx)
    if cached is not None:
        return cached
    wav = PROBE_DIR / f"aud_check_{idx:04d}.wav"
    if not wav.exists():
        subprocess.check_call([str(CLI), "audition", str(idx), str(wav)], cwd=ROOT)
    pk = analyze_file(wav).peak
    _AUDITION_PEAK_CACHE[idx] = pk
    return pk


def is_audible_preset(idx: int, presets: list[dict]) -> bool:
    cat = presets[idx]["category"]
    pk = audition_peak(idx)
    if not peak_passes_audible_gate(pk):
        return False
    audition_wav = PROBE_DIR / f"aud_check_{idx:04d}.wav"
    audition_metrics = analyze_file(audition_wav, category=cat)
    ok, _ = category_spectral_passes_audition(audition_metrics, cat)
    if not ok:
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


def ranked_audible_candidates(
    idxs: list[int], presets: list[dict]
) -> list[tuple[float, int, np.ndarray]]:
    scored: list[tuple[float, int, np.ndarray]] = []
    for idx in idxs:
        if len(scored) >= _MAX_AUDIBLE_CANDIDATES_PER_CATEGORY:
            break
        if not is_audible_preset(idx, presets):
            continue
        wav = probe_wav_path(PROBE_DIR, idx)
        if not wav.exists():
            render_category_probe(presets[idx], idx, PROBE_DIR)
        scored.append((audition_peak(idx), idx, category_feature_vector(wav)))
    scored.sort(key=lambda t: t[0], reverse=True)
    return scored


def pick_pair_from_candidates(candidates: list[tuple[float, int, np.ndarray]]) -> list[int]:
    if not candidates:
        raise SystemExit("No audible presets in category")
    if len(candidates) == 1:
        return [candidates[0][1]]
    vecs = {idx: vec for _pk, idx, vec in candidates}
    audible = [idx for _pk, idx, _vec in candidates]
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
        candidates = ranked_audible_candidates(by_cat[category], presets)
        idxs = [idx for _pk, idx, _vec in candidates]
        vecs: dict[int, np.ndarray] = {}
        for _pk, idx, vec in candidates:
            vecs[idx] = vec
            vecs_all[idx] = vec

        best_picks = pick_pair_from_candidates(candidates)
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
