#!/usr/bin/env python3
"""Generate diverse Trap Lord factory bank with category-phrase audio gate."""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import random
import re
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path

import numpy as np

from preset_style_catalog import SUB_STYLES

ROOT = Path(__file__).resolve().parents[1]
BUILD = ROOT / "build"
CLI = BUILD / "TrapLordSynthRenderCLI_artefacts" / "Release" / "TrapLordSynthRenderCLI"
PROBE_DIR = ROOT / "build" / "factory-gate-probes"

FORBIDDEN = re.compile(
    r"(jeezy|snowman|gucci|zay|zaytoven|redd|shawty|drumma|luger|migo|future|metro|timbaland|trump)",
    re.I,
)

TARGET_QUOTAS: dict[str, int] = {
    "Pads": 60,
    "Leads": 60,
    "Synth Keys": 60,
    "Swells": 50,
    "Brass": 34,
    "Strings": 34,
    "Church Keys/EP": 34,
    "Organ": 34,
    "Bells": 34,
    "Bass": 34,
    "Choir": 33,
    "Hits/FX": 33,
}

ENGINE_ROTATION: dict[str, list[int]] = {
    "Pads": [7, 12, 15, 20, 5, 0, 14, 2, 19, 1, 13, 18, 7, 12, 15, 20, 5, 0, 14, 2],
    "Swells": [20, 14, 7, 15, 2, 0, 12, 19, 5, 20, 14, 23, 7, 15, 2],
    "Synth Keys": [18, 21, 9, 17, 7, 8, 10, 13, 3, 18, 21, 9, 17, 7, 8],
    "Leads": [6, 8, 10, 16, 0, 17, 13, 6, 8, 10, 16, 0, 17, 13],
    # Thin categories: long rotations across distinct engine timbres (not just param jitter).
    "Bass": [4, 22, 11, 23, 6, 4, 22, 8, 11, 23, 4, 22, 11, 4, 23, 6],
    "Bells": [3, 10, 13, 21, 9, 16, 17, 3, 10, 13, 21, 9, 16, 13, 3, 10],
    "Brass": [16, 0, 6, 2, 14, 10, 16, 0, 6, 2, 14, 16, 0, 6, 10, 2],
    "Strings": [2, 19, 14, 1, 5, 20, 7, 15, 2, 19, 14, 1, 5, 20, 7, 15],
    "Church Keys/EP": [9, 21, 18, 17, 8, 7, 13, 9, 21, 18, 17, 8, 3, 9, 21, 18],
    "Organ": [1, 5, 14, 8, 16, 1, 5, 14, 8, 1, 16, 5, 14, 8, 1, 5],
    "Choir": [5, 2, 19, 14, 1, 7, 15, 20, 12, 5, 2, 19, 14, 1, 7, 15, 20, 12],
    "Hits/FX": [23, 16, 10, 6, 0, 22, 23, 10, 16, 6, 0, 22, 23, 16, 10, 6],
}

BANK_BY_CAT = {
    "Church Keys/EP": 0,
    "Synth Keys": 0,
    "Organ": 0,
    "Pads": 2,
    "Choir": 2,
    "Swells": 2,
    "Brass": 1,
    "Leads": 1,
    "Bass": 1,
    "Strings": 2,
    "Bells": 2,
    "Hits/FX": 2,
}

ADJECTIVES = [
    "Dark",
    "Trap",
    "Cathedral",
    "Ominous",
    "Frozen",
    "Southern",
    "Midnight",
    "Shadow",
    "Cinematic",
    "Evil",
    "Haunted",
    "Dusty",
    "Chrome",
    "Velvet",
    "Grim",
    "Rising",
    "Reverse",
]

CATEGORY_NOUNS: dict[str, list[str]] = {
    "Pads": ["Pad", "Wash", "Drift", "Mist", "Layer", "Cloud"],
    "Leads": ["Lead", "Saw", "Hook", "Cry", "Blade", "Solo"],
    "Synth Keys": ["Keys", "Stack", "Pluck", "Glass", "Tine", "Digi"],
    "Swells": ["Swell", "Rise", "Bloom", "Tide", "Lift", "Sweep"],
    "Brass": ["Brass", "Section", "Stab", "Fanfare", "Horns", "Blast"],
    "Strings": ["Strings", "Legato", "Section", "Arc", "Bows", "Line"],
    "Church Keys/EP": ["EP", "Keys", "Rhodes", "Wurli", "Gospel", "Tines"],
    "Organ": ["Organ", "Drawbar", "Pipes", "Gospel", "Church", "Rotary"],
    "Bells": ["Bell", "Chime", "Glock", "Tine", "Spark", "Ping"],
    "Bass": ["808", "Sub", "Low End", "Drone", "Punch", "Thump"],
    "Choir": ["Choir", "Vox", "Oohs", "Aahs", "Chant", "Stack"],
    "Hits/FX": ["Hit", "Stab", "FX", "Impact", "Rise", "Crash"],
}

SHORT_CATEGORY = {"Church Keys/EP": "Church Keys", "Hits/FX": "Hits"}

CORR_MAX = 0.92
CANDIDATE_OVERSHOOT = 4.0
MAX_GATE_BATCHES = 80
POOL_CAP = 320
THIN_FACTORY_CATEGORIES = frozenset(
    {
        "Choir",
        "Bass",
        "Strings",
        "Hits/FX",
        "Bells",
        "Church Keys/EP",
        "Organ",
        "Brass",
    }
)


def clamp(v: float, lo: float, hi: float) -> float:
    return max(lo, min(hi, v))


def pick_range(spec: dict, key: str, rng: random.Random, default: tuple[float, float]) -> float:
    band = spec.get(key, default)
    if isinstance(band, (list, tuple)) and len(band) == 2:
        return rng.uniform(float(band[0]), float(band[1]))
    return float(band)


def apply_engine_texture(category: str, p: dict, engine: int, variant: int, rng: random.Random) -> None:
    """Pin parameter islands per engine so category-probe vectors stay spectrally separated."""
    if category == "Bass":
        if engine in (4, 22):
            p["cutoff"] = clamp(p["cutoff"] * 0.55, 80, 720)
            p["drive"] = clamp(p["drive"] * 1.05, 1.0, 3.2)
        elif engine == 11:
            p["cutoff"] = clamp(p["cutoff"] * 0.75, 120, 1400)
            p["drive"] = clamp(p["drive"] * 1.35, 1.4, 4.8)
        elif engine == 23:
            p["attack"] = clamp(p["attack"], 0.001, 0.02)
            p["decay"] = clamp(p["decay"], 0.06, 0.35)
            p["sustain"] = clamp(p["sustain"], 0.0, 0.2)
    elif category == "Choir":
        p["gain"] = clamp(p["gain"] + 0.06 + (variant % 5) * 0.035, 0.16, 0.45)
        if engine in (5, 1, 2):
            p["chord"] = rng.choice([1, 2, 4, 5])
            p["vibratoDepth"] = clamp(p["vibratoDepth"] + 0.08, 0, 0.35)
        elif engine in (19, 14):
            p["vibratoDepth"] = clamp(p["vibratoDepth"] + 0.22, 0.12, 0.5)
            p["macroMovement"] = clamp(p["macroMovement"] + 0.18, 0.15, 0.85)
        elif engine in (7, 12, 15, 20):
            p["macroSpace"] = clamp(p["macroSpace"] + 0.2, 0.2, 0.9)
            p["reverb"] = clamp(p["reverb"] + 0.12, 0.1, 0.8)
    elif category == "Strings":
        if engine == 19:
            p["vibratoDepth"] = clamp(p["vibratoDepth"] + 0.25, 0.15, 0.55)
        elif engine == 14:
            p["macroMovement"] = clamp(p["macroMovement"] + 0.25, 0.2, 0.95)
        elif engine == 20:
            p["swellDepth"] = clamp(p["swellDepth"] + 0.35, 0.35, 1.0)
        elif engine in (1, 5):
            p["chord"] = rng.choice([1, 2, 5])
    elif category == "Hits/FX":
        if engine == 23:
            p["pitchEnvSemi"] = clamp(p["pitchEnvSemi"] + rng.uniform(2, 10), 0, 16)
        elif engine == 16:
            p["attack"] = clamp(p["attack"], 0.001, 0.015)
            p["drive"] = clamp(p["drive"] * 1.2, 1.8, 4.8)
    elif category == "Bells":
        if engine in (3, 10, 13):
            p["decay"] = clamp(p["decay"] * 0.85, 0.05, 0.45)
            p["reverb"] = clamp(p["reverb"] + 0.1, 0.1, 0.75)
        elif engine in (21, 17, 9):
            p["arp"] = variant % 2 == 0
    elif category == "Church Keys/EP":
        if engine in (9, 21, 18):
            p["macroDirt"] = clamp(p["macroDirt"] * 0.7, 0.0, 0.28)
        elif engine in (17, 8, 7):
            p["arp"] = variant % 3 != 1
            p["arpRate"] = clamp(p["arpRate"] + (variant % 5), 2, 14)
    elif category == "Organ":
        if engine in (1, 5):
            p["vibratoDepth"] = clamp(p["vibratoDepth"] + 0.18, 0.08, 0.5)
            p["macroMovement"] = clamp(p["macroMovement"] + 0.2, 0.2, 0.95)
        elif engine == 14:
            p["macroSpace"] = clamp(p["macroSpace"] + 0.15, 0.15, 0.75)
    elif category == "Brass":
        if engine == 16:
            p["attack"] = clamp(p["attack"], 0.002, 0.04)
            p["drive"] = clamp(p["drive"] * 1.15, 1.6, 4.8)
        elif engine == 6:
            p["pitchEnvSemi"] = clamp(p["pitchEnvSemi"] + rng.uniform(1, 6), 0, 12)
        elif engine == 2:
            p["sustain"] = clamp(p["sustain"] + 0.08, 0.5, 0.95)
            p["release"] = clamp(p["release"] + 0.15, 0.2, 1.2)


def build_from_style(category: str, style: dict, variant: int, rng: random.Random) -> dict:
    engines = style.get("engines", [0])
    engine = engines[variant % len(engines)]
    chord_opts = style.get("chord", [0])
    chord = chord_opts[variant % len(chord_opts)] if chord_opts else 0
    arp_prob = float(style.get("arp", 0.0))
    arp = variant % 5 == 0 and rng.random() < arp_prob + 0.15

    p: dict = {
        "category": category,
        "bank": BANK_BY_CAT.get(category, 0),
        "subStyle": style.get("label", "Core"),
        "engine": engine,
        "filterType": int(style.get("filterType", 0)),
        "attack": pick_range(style, "attack", rng, (0.01, 0.4)),
        "decay": pick_range(style, "decay", rng, (0.1, 0.6)),
        "sustain": pick_range(style, "sustain", rng, (0.5, 0.9)),
        "release": pick_range(style, "release", rng, (0.2, 1.0)),
        "cutoff": pick_range(style, "cutoff", rng, (800, 8000)),
        "gain": pick_range(style, "gain", rng, (0.12, 0.38)),
        "drive": pick_range(style, "drive", rng, (0.9, 2.2)),
        "detune": pick_range(style, "detune", rng, (4, 14)),
        "reverb": pick_range(style, "reverb", rng, (0.08, 0.45)),
        "delay": pick_range(style, "delay", rng, (0.0, 0.2)),
        "chord": chord,
        "arp": arp,
        "arpRate": pick_range(style, "arpRate", rng, (2, 12)),
        "arpGate": rng.uniform(0.45, 0.88),
        "resonance": rng.uniform(0.35, 1.05),
        "pitchEnvSemi": pick_range(style, "pitchEnvSemi", rng, (0, 4)) if "pitchEnvSemi" in style else 0.0,
        "pitchEnvMs": rng.uniform(30, 220),
        "pulseWidth": pick_range(style, "pulseWidth", rng, (0.5, 0.5)) if "pulseWidth" in style else 0.5,
        "portamentoMs": pick_range(style, "portamentoMs", rng, (0, 0)) if "portamentoMs" in style else 0.0,
        "vibratoDepth": pick_range(style, "vibratoDepth", rng, (0.0, 0.12)) if "vibratoDepth" in style else 0.0,
        "swellDepth": pick_range(style, "swellDepth", rng, (0.0, 0.0)) if "swellDepth" in style else 0.0,
        "macroDarkness": pick_range(style, "macroDarkness", rng, (0.1, 0.45)),
        "macroDirt": pick_range(style, "macroDirt", rng, (0.05, 0.45)),
        "macroMovement": pick_range(style, "macroMovement", rng, (0.05, 0.45)),
        "macroSpace": pick_range(style, "macroSpace", rng, (0.1, 0.45)),
    }

    if category == "Bass":
        p["chord"] = 0
        p["arp"] = False
        p["swellDepth"] = 0.0
        p["gain"] = clamp(p["gain"] + (variant % 7) * 0.03, 0.14, 0.42)
    if category == "Hits/FX":
        p["attack"] = clamp(p["attack"], 0.001, 0.03)
        p["decay"] = clamp(p["decay"], 0.04, 0.35)
        p["sustain"] = clamp(p["sustain"], 0.0, 0.12)
        p["release"] = clamp(p["release"], 0.05, 0.4)
        p["swellDepth"] = 0.0
        p["chord"] = 0
        p["reverb"] = clamp(p["reverb"], 0.0, 0.35)
        p["macroDirt"] = clamp(p["macroDirt"], 0.0, 0.25)
    if category in ("Church Keys/EP", "Bells"):
        p["macroDirt"] = clamp(p["macroDirt"], 0.0, 0.28)
        p["drive"] = clamp(p["drive"], 0.9, 3.2)
    if category == "Leads":
        p["cutoff"] = clamp(p["cutoff"], 450.0, 14000.0)
        if p["filterType"] == 1:
            p["cutoff"] = clamp(p["cutoff"], 280.0, 5200.0)
    if category == "Swells":
        p["swellDepth"] = clamp(p["swellDepth"], 0.4, 1.0)
        if p["swellDepth"] < 0.35:
            p["swellDepth"] = rng.uniform(0.45, 1.0)

    rot = ENGINE_ROTATION.get(category, style.get("engines", [p["engine"]]))
    p["engine"] = rot[variant % len(rot)]
    apply_engine_texture(category, p, int(p["engine"]), variant, rng)
    p["filterType"] = int(style.get("filterType", 0) + (variant % 4)) % 6
    if p["filterType"] == 1 and category in {"Pads", "Swells", "Strings", "Choir"}:
        p["cutoff"] = clamp(p["cutoff"], 80.0, 320.0)
    elif p["filterType"] == 1:
        p["cutoff"] = clamp(p["cutoff"], 350.0, 5200.0)
    spread = 0.45 + (variant % 13) * 0.09
    p["cutoff"] = clamp(p["cutoff"] * spread, 120, 16000)
    p["detune"] = clamp(p["detune"] + (variant % 9) * 2.2, 0, 24)
    p["drive"] = clamp(p["drive"] * (0.75 + (variant % 6) * 0.12), 0.8, 4.8)
    p["macroDarkness"] = clamp(p["macroDarkness"] + (variant % 5) * 0.11, 0, 1)
    p["macroDirt"] = clamp(p["macroDirt"] + (variant % 4) * 0.13, 0, 1)
    p["macroMovement"] = clamp(p["macroMovement"] + (variant % 7) * 0.09, 0, 1)
    p["macroSpace"] = clamp(p["macroSpace"] + (variant % 6) * 0.1, 0, 1)
    p["reverb"] = clamp(p["reverb"] * (0.7 + (variant % 5) * 0.14), 0, 0.85)
    p["delay"] = clamp(p["delay"] + (variant % 4) * 0.06, 0, 0.55)
    if variant % 3 == 0:
        p["arp"] = not p["arp"]
    if variant % 4 == 1:
        p["chord"] = rng.choice(chord_opts if chord_opts else [0])
    return p


def param_fingerprint(p: dict) -> str:
    skip = {"name", "category", "subStyle"}
    keys = sorted(k for k in p.keys() if k not in skip)
    rounded = "|".join(f"{k}:{round(float(p[k]) if isinstance(p[k], (int, float)) else (1 if p[k] else 0), 3)}" for k in keys)
    return hashlib.sha256(rounded.encode()).hexdigest()[:16]


def make_name(category: str, style_label: str, rng: random.Random, idx: int, preset: dict | None = None) -> str:
    short = SHORT_CATEGORY.get(category, category.replace("/", " "))
    nouns = CATEGORY_NOUNS.get(category, ["Stack"])
    noun = rng.choice(nouns)
    if category == "Hits/FX" and preset is not None:
        label = style_label.lower()
        if "impact" in label or "hit" in label:
            noun = rng.choice(["Hit", "Impact", "Stab"])
        elif "crash" in label:
            noun = "Crash" if preset.get("cutoff", 2000) > 900 else "Thump"
        elif "stab" in label:
            noun = "Stab"
        sustain = float(preset.get("sustain", 0.2))
        if sustain > 0.2 and noun in {"Impact", "Hit"}:
            noun = "Stab"
    name = f"{short} {style_label} {rng.choice(ADJECTIVES)} {noun} {idx:02d}"
    return name if not FORBIDDEN.search(name) else f"{short} {style_label} Stack {idx:02d}"


def generate_candidates(category: str, count: int, rng: random.Random) -> list[dict]:
    styles = SUB_STYLES.get(category, SUB_STYLES["Brass"])
    out: list[dict] = []
    seen: set[str] = set()
    variant = 0
    while len(out) < count and variant < count * 12:
        if category in THIN_FACTORY_CATEGORIES and (variant % 4 == 0 or rng.random() < 0.28):
            style = rng.choice(styles)
            v_ix = variant * 5 + rng.randint(0, 31)
        else:
            style = styles[variant % len(styles)]
            v_ix = variant
        p = build_from_style(category, style, v_ix, rng)
        fp = param_fingerprint(p)
        if fp in seen:
            variant += 1
            continue
        seen.add(fp)
        p["name"] = make_name(category, style.get("label", "Core"), rng, variant + 1, p)
        out.append(p)
        variant += 1
    return out


def build_cli() -> None:
    if not BUILD.exists():
        subprocess.check_call(
            [
                "cmake",
                "-B",
                str(BUILD),
                "-DCMAKE_BUILD_TYPE=Release",
                "-DCMAKE_CXX_COMPILER=g++-12",
                "-DCMAKE_C_COMPILER=gcc-12",
            ],
            cwd=ROOT,
        )
    subprocess.check_call(["cmake", "--build", str(BUILD), "-j", "--target", "TrapLordSynthRenderCLI"], cwd=ROOT)


def write_bank(path: Path, presets: list[dict], quotas: dict[str, int], meta: dict) -> None:
    payload = {
        "version": 3,
        "count": len(presets),
        "targetQuotas": TARGET_QUOTAS,
        "achievedQuotas": quotas,
        "diversityGate": meta,
        "categoryQuotas": quotas,
        "presets": presets,
    }
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2), encoding="utf-8")


def rebuild_factory_binary() -> None:
    subprocess.check_call(
        ["cmake", "--build", str(BUILD), "-j", "--target", "TrapLordSynthFactoryData", "TrapLordSynthRenderCLI"],
        cwd=ROOT,
    )


def phrase_variant(p: dict) -> int:
    return int((int(p.get("engine", 0)) * 2 + int(p.get("chord", 0)) + int(p.get("filterType", 0))) % 6)


def probe_preset_vector(preset: dict, tag: str) -> np.ndarray | None:
    """Render category phrase for one preset dict and return gate feature vector or None."""
    sys.path.insert(0, str(ROOT / "tools"))
    from trap_lord_audio_metrics import (  # noqa: E402
        analyze_file,
        category_spectral_passes,
        gate_feature_vector,
        peak_passes_audible_gate,
    )

    json_path = PROBE_DIR / f"{tag}.json"
    wav = PROBE_DIR / f"{tag}.wav"
    json_path.write_text(json.dumps(preset), encoding="utf-8")
    subprocess.check_call(
        [str(CLI), "category-probe-json", str(json_path), str(wav)], cwd=ROOT, timeout=180
    )
    cat = str(preset.get("category", ""))
    metrics = analyze_file(wav, category=cat)
    ok, _ = category_spectral_passes(metrics, cat)
    if not ok or not peak_passes_audible_gate(metrics.peak):
        return None
    return gate_feature_vector(wav, preset)


def _render_probe(local_i: int, preset: dict) -> tuple[int, np.ndarray | None]:
    sys.path.insert(0, str(ROOT / "tools"))
    from trap_lord_audio_metrics import analyze_file, category_spectral_passes, gate_feature_vector

    json_path = PROBE_DIR / f"candidate_{local_i:05d}.json"
    wav = PROBE_DIR / f"candidate_{local_i:05d}.wav"
    json_path.write_text(json.dumps(preset), encoding="utf-8")
    subprocess.check_call(
        [str(CLI), "category-probe-json", str(json_path), str(wav)], cwd=ROOT, timeout=180
    )
    from trap_lord_audio_metrics import AUDITION_PEAK_MIN_LINEAR, peak_passes_audible_gate

    metrics = analyze_file(wav, category=str(preset.get("category", "")))
    ok, _ = category_spectral_passes(metrics, str(preset.get("category", "")))
    if not ok:
        return local_i, None
    if not peak_passes_audible_gate(metrics.peak):
        return local_i, None
    return local_i, gate_feature_vector(wav, preset)


def render_candidate_pool(candidates: list[dict]) -> tuple[list[dict], list[np.ndarray]]:
    PROBE_DIR.mkdir(parents=True, exist_ok=True)
    workers = min(6, max(2, (os.cpu_count() or 4)))
    vecs: list[np.ndarray | None] = [None] * len(candidates)
    with ThreadPoolExecutor(max_workers=workers) as pool:
        futures = [pool.submit(_render_probe, i, p) for i, p in enumerate(candidates)]
        for fut in as_completed(futures):
            idx, vec = fut.result()
            vecs[idx] = vec
    out_cands: list[dict] = []
    out_vecs: list[np.ndarray] = []
    for i, p in enumerate(candidates):
        if vecs[i] is not None:
            out_cands.append(p)
            out_vecs.append(vecs[i])
    return out_cands, out_vecs


def select_diverse_presets(
    candidates: list[dict],
    vectors: list[np.ndarray],
    want: int,
    corr_max: float,
    existing_vecs: list[np.ndarray] | None = None,
) -> tuple[list[dict], list[np.ndarray]]:
    if not candidates:
        return [], []
    existing = list(existing_vecs or [])
    remaining = list(range(len(candidates)))
    chosen: list[int] = []
    while remaining and len(chosen) < want:
        best_i: int | None = None
        best_score = -1.0
        refs = existing + [vectors[j] for j in chosen]
        if not refs:
            # Seed with the least typical preset (lowest mean similarity to the pool).
            for i in remaining:
                if len(remaining) < 2:
                    score = 1.0
                else:
                    mean_sim = float(
                        np.mean(
                            [float(np.dot(vectors[i], vectors[j])) for j in remaining if j != i]
                        )
                    )
                    score = 1.0 - mean_sim
                if score > best_score:
                    best_score = score
                    best_i = i
        else:
            for i in remaining:
                v = vectors[i]
                worst = max(float(np.dot(v, r)) for r in refs)
                score = -1.0 if worst >= corr_max else 1.0 - worst
                if score > best_score:
                    best_score = score
                    best_i = i
        if best_i is None or best_score < 0.0:
            break
        chosen.append(best_i)
        remaining.remove(best_i)
    new_vecs = [vectors[i] for i in chosen]
    return [candidates[i] for i in chosen], new_vecs


def run_generation(
    seed: int,
    target: dict[str, int],
    audio_gate: bool,
    resume_presets: list[dict] | None = None,
    regen_only: set[str] | None = None,
) -> tuple[list[dict], dict]:
    rng = random.Random(seed)
    resume_presets = list(resume_presets or [])
    resume_by_cat: dict[str, list[dict]] = {}
    for p in resume_presets:
        resume_by_cat.setdefault(p["category"], []).append(p)

    all_presets: list[dict] = list(resume_presets)
    achieved: dict[str, int] = {}
    for p in resume_presets:
        achieved[p["category"]] = achieved.get(p["category"], 0) + 1
    gate_meta: dict = {
        "corrMax": CORR_MAX,
        "crossCategoryCorrMax": 0.96,
        "feature": "category_probe_audio",
        "categories": {},
    }
    out_path = ROOT / "assets" / "factory" / "preset_bank.json"

    if audio_gate:
        build_cli()

    cross_category_refs: list[np.ndarray] = []
    if audio_gate and resume_presets:
        print(f"Resume: seeding cross-category refs from {len(resume_presets)} presets…", flush=True)
        for i, p in enumerate(resume_presets):
            v = probe_preset_vector(p, f"resume_{i:05d}")
            if v is not None:
                cross_category_refs.append(v)

    for category, quota in target.items():
        want = max(1, int(quota))
        if regen_only is not None and category not in regen_only:
            existing = list(resume_by_cat.get(category, []))[:want]
            achieved[category] = len(existing)
            gate_meta["categories"][category] = {
                "target": quota,
                "achieved": achieved[category],
                "batches": 0,
                "poolSizeLastBatch": 0,
                "preserved": True,
            }
            trimmed = [p for p in all_presets if p.get("category") != category]
            trimmed.extend(existing)
            all_presets = trimmed
            if audio_gate and all_presets:
                write_bank(out_path, all_presets, achieved, gate_meta)
            print(f"{category}: {achieved[category]}/{quota} (preserved)", flush=True)
            continue

        fresh = regen_only is not None and category in regen_only
        accepted: list[dict] = [] if fresh else list(resume_by_cat.get(category, []))[:want]
        accepted_vecs: list[np.ndarray] = []
        if audio_gate and accepted and not fresh:
            for i, p in enumerate(accepted):
                v = probe_preset_vector(p, f"resume_{category}_{i:04d}")
                if v is not None:
                    accepted_vecs.append(v)
        batch = 0
        pool_size = 0
        if len(accepted) >= want:
            achieved[category] = len(accepted[:want])
            gate_meta["categories"][category] = {
                "target": quota,
                "achieved": achieved[category],
                "batches": 0,
                "poolSizeLastBatch": 0,
                "resumed": len(accepted),
            }
            trimmed = [p for p in all_presets if p.get("category") != category]
            trimmed.extend(accepted[:want])
            all_presets = trimmed
            if audio_gate and all_presets:
                write_bank(out_path, all_presets, achieved, gate_meta)
            print(f"{category}: {achieved[category]}/{quota} (resumed)", flush=True)
            continue
        while len(accepted) < want and batch < MAX_GATE_BATCHES:
            batch += 1
            need = want - len(accepted)
            pool_size = min(POOL_CAP, max(need * 6, int(want * CANDIDATE_OVERSHOOT)))
            print(
                f"{category}: batch {batch} need={need} pool={pool_size} accepted={len(accepted)}/{want}",
                flush=True,
            )
            batch_rng = random.Random(rng.randint(0, 2**30) ^ (batch * 1009) ^ hash(category))
            cands = generate_candidates(category, pool_size, batch_rng)

            if audio_gate:
                filtered_cands, vectors = render_candidate_pool(cands)
                cross_ok_c: list[dict] = []
                cross_ok_v: list[np.ndarray] = []
                for c, v in zip(filtered_cands, vectors):
                    if cross_category_refs:
                        worst = max(float(np.dot(v, r)) for r in cross_category_refs)
                        if worst > 0.96:
                            continue
                    cross_ok_c.append(c)
                    cross_ok_v.append(v)
                picked, new_vecs = select_diverse_presets(
                    cross_ok_c, cross_ok_v, need, CORR_MAX, accepted_vecs
                )
                accepted.extend(picked)
                accepted_vecs.extend(new_vecs)
                cross_category_refs.extend(new_vecs)
            else:
                accepted.extend(cands[:need])

        achieved[category] = min(len(accepted), want)
        gate_meta["categories"][category] = {
            "target": quota,
            "achieved": achieved[category],
            "batches": batch,
            "poolSizeLastBatch": pool_size if batch else 0,
        }
        trimmed = [p for p in all_presets if p.get("category") != category]
        trimmed.extend(accepted[:want])
        all_presets = trimmed
        if audio_gate and all_presets:
            write_bank(out_path, all_presets, achieved, gate_meta)
        print(f"{category}: {achieved[category]}/{quota} (batch {batch})", flush=True)

    if audio_gate and all_presets:
        write_bank(out_path, all_presets, achieved, gate_meta)
        rebuild_factory_binary()

    return all_presets, {"achievedQuotas": achieved, "gate": gate_meta}


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--seed", type=int, default=200604)
    parser.add_argument("--no-audio-gate", action="store_true")
    parser.add_argument("-o", "--output", type=Path, default=None)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()

    out = args.output or ROOT / "assets" / "factory" / "preset_bank.json"
    presets, meta = run_generation(args.seed, TARGET_QUOTAS, audio_gate=not args.no_audio_gate)
    write_bank(out, presets, meta["achievedQuotas"], meta["gate"])

    total = len(presets)
    print(f"Wrote {total} presets to {out}")
    for cat, n in sorted(meta["achievedQuotas"].items()):
        print(f"  {cat}: {n} (target {TARGET_QUOTAS.get(cat, '?')})")

    report = Path("/opt/cursor/artifacts/factory-diversity-gate-report.json")
    report.parent.mkdir(parents=True, exist_ok=True)
    report.write_text(json.dumps({"count": total, **meta}, indent=2), encoding="utf-8")

    if args.check:
        for p in presets:
            assert not FORBIDDEN.search(p["name"])
            assert 0.05 <= p["gain"] <= 0.5
    return 0


if __name__ == "__main__":
    sys.exit(main())
