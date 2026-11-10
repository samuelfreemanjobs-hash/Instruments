#!/usr/bin/env python3
"""Generate 500 neutral ATL-trap-style factory presets (JSON)."""

from __future__ import annotations

import argparse
import hashlib
import json
import random
import re
import sys
from pathlib import Path

FORBIDDEN = re.compile(
    r"(jeezy|snowman|gucci|zay|zaytoven|redd|shawty|drumma|luger|migo|future|metro|timbaland|trump)",
    re.I,
)

CATEGORIES = [
    "Brass",
    "Strings",
    "Keys",
    "Organ",
    "Bells",
    "Leads",
    "Bass",
    "Pads/Choir",
    "Hits/FX",
]

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
]

NOUNS = [
    "Brass",
    "Strings",
    "Keys",
    "Organ",
    "Bell",
    "Lead",
    "Bass",
    "Pad",
    "Hit",
    "Stab",
    "Stack",
    "Choir",
    "Pulse",
    "Cathedral",
]

BASE_RECIPES = [
    {
        "name": "Dirty Trap Brass",
        "category": "Brass",
        "engine": 0,
        "attack": 0.005,
        "decay": 0.26,
        "sustain": 0.65,
        "release": 0.18,
        "cutoff": 2400,
        "gain": 0.26,
        "drive": 2.3,
        "detune": 9,
        "resonance": 0.85,
        "pitchEnvSemi": 3,
        "pitchEnvMs": 45,
        "macroDarkness": 0.35,
        "macroDirt": 0.55,
    },
    {
        "name": "Church Keys",
        "category": "Keys",
        "engine": 9,
        "attack": 0.012,
        "decay": 0.35,
        "sustain": 0.72,
        "release": 0.45,
        "cutoff": 6200,
        "gain": 0.22,
        "drive": 1.1,
        "detune": 5,
        "chord": 2,
        "reverb": 0.32,
        "macroMovement": 0.45,
        "macroSpace": 0.38,
    },
    {
        "name": "Evil Analog Lead",
        "category": "Leads",
        "engine": 8,
        "pulseWidth": 0.14,
        "attack": 0.008,
        "decay": 0.18,
        "sustain": 0.7,
        "release": 0.28,
        "cutoff": 1800,
        "resonance": 1.1,
        "portamentoMs": 85,
        "drive": 1.9,
        "macroDarkness": 0.55,
    },
    {
        "name": "Haunted Trap Bells",
        "category": "Bells",
        "engine": 10,
        "attack": 0.002,
        "decay": 0.15,
        "sustain": 0.2,
        "release": 1.8,
        "reverb": 0.58,
        "macroDarkness": 0.48,
        "macroSpace": 0.62,
    },
    {
        "name": "Southern Church Organ",
        "category": "Organ",
        "engine": 1,
        "vibratoDepth": 0.22,
        "attack": 0.06,
        "sustain": 0.88,
        "release": 0.85,
        "macroMovement": 0.72,
    },
    {
        "name": "Dark Orchestral Strings",
        "category": "Strings",
        "engine": 2,
        "attack": 0.9,
        "decay": 0.7,
        "sustain": 0.85,
        "release": 1.1,
        "detune": 14,
        "macroMovement": 0.55,
        "macroSpace": 0.48,
    },
]

BANK_BY_CAT = {
    "Brass": 1,
    "Strings": 2,
    "Keys": 0,
    "Organ": 0,
    "Bells": 2,
    "Leads": 1,
    "Bass": 1,
    "Pads/Choir": 2,
    "Hits/FX": 2,
}


def clamp(v: float, lo: float, hi: float) -> float:
    return max(lo, min(hi, v))


def fingerprint(p: dict) -> str:
    keys = sorted(p.keys())
    rounded = "|".join(f"{k}:{round(float(p[k]), 3)}" for k in keys if k not in ("name", "category"))
    return hashlib.sha256(rounded.encode()).hexdigest()[:16]


def make_name(category: str, rng: random.Random, idx: int) -> str:
    for _ in range(40):
        name = f"{category} {rng.choice(ADJECTIVES)} {rng.choice(NOUNS)} {idx:02d}"
        if not FORBIDDEN.search(name):
            return name
    return f"{category} Neutral Stack {idx:02d}"


def mutate(recipe: dict, rng: random.Random, category: str) -> dict:
    p = {
        "category": category,
        "bank": BANK_BY_CAT.get(category, 0),
        "engine": recipe.get("engine", 0),
        "attack": recipe.get("attack", 0.01),
        "decay": recipe.get("decay", 0.3),
        "sustain": recipe.get("sustain", 0.7),
        "release": recipe.get("release", 0.4),
        "cutoff": recipe.get("cutoff", 4500),
        "gain": recipe.get("gain", 0.2),
        "drive": recipe.get("drive", 1.0),
        "detune": recipe.get("detune", 7),
        "reverb": recipe.get("reverb", 0.22),
        "delay": recipe.get("delay", 0.05),
        "chord": recipe.get("chord", 0),
        "resonance": recipe.get("resonance", 0.55),
        "pitchEnvSemi": recipe.get("pitchEnvSemi", 0),
        "pitchEnvMs": recipe.get("pitchEnvMs", 45),
        "pulseWidth": recipe.get("pulseWidth", 0.5),
        "portamentoMs": recipe.get("portamentoMs", 0),
        "vibratoDepth": recipe.get("vibratoDepth", 0),
        "macroDarkness": recipe.get("macroDarkness", 0.2),
        "macroDirt": recipe.get("macroDirt", 0.2),
        "macroMovement": recipe.get("macroMovement", 0.2),
        "macroSpace": recipe.get("macroSpace", 0.25),
    }

    p["attack"] = clamp(p["attack"] * rng.uniform(0.6, 1.6), 0.001, 2.5)
    p["decay"] = clamp(p["decay"] * rng.uniform(0.7, 1.4), 0.01, 3.0)
    p["sustain"] = clamp(p["sustain"] + rng.uniform(-0.12, 0.12), 0.05, 1.0)
    p["release"] = clamp(p["release"] * rng.uniform(0.6, 1.5), 0.05, 3.5)
    p["cutoff"] = clamp(p["cutoff"] * rng.uniform(0.65, 1.35), 200, 16000)
    p["gain"] = clamp(p["gain"] * rng.uniform(0.85, 1.15), 0.08, 0.45)
    p["drive"] = clamp(p["drive"] * rng.uniform(0.75, 1.35), 0.8, 4.5)
    p["detune"] = clamp(p["detune"] + rng.uniform(-4, 4), 0, 24)

    if category == "Bass":
        p["engine"] = 4
        p["cutoff"] = clamp(p["cutoff"] * 0.55, 150, 2000)
    elif category == "Hits/FX":
        p["attack"] = clamp(p["attack"] * 0.5, 0.001, 0.05)
        p["release"] = clamp(p["release"] * 0.6, 0.05, 1.2)
    elif category == "Pads/Choir":
        p["engine"] = 5 if rng.random() > 0.35 else 2
        p["attack"] = clamp(p["attack"] + 0.3, 0.1, 2.5)

    return p


def generate(seed: int = 200604) -> list[dict]:
    rng = random.Random(seed)
    per_cat = 500 // len(CATEGORIES)
    remainder = 500 % len(CATEGORIES)
    presets: list[dict] = []
    seen: set[str] = set()
    idx = 1

    for cat_i, category in enumerate(CATEGORIES):
        count = per_cat + (1 if cat_i < remainder else 0)
        for _ in range(count):
            recipe = rng.choice(BASE_RECIPES)
            if recipe["category"] != category and rng.random() > 0.35:
                recipe = {"engine": 0, "category": category}
            for attempt in range(64):
                params = mutate(recipe, rng, category)
                fp = fingerprint(params)
                if fp in seen:
                    continue
                seen.add(fp)
                params["name"] = make_name(category, rng, idx)
                if FORBIDDEN.search(params["name"]):
                    continue
                presets.append(params)
                idx += 1
                break

    while len(presets) < 500:
        category = rng.choice(CATEGORIES)
        params = mutate(rng.choice(BASE_RECIPES), rng, category)
        fp = fingerprint(params)
        if fp in seen:
            continue
        seen.add(fp)
        params["name"] = make_name(category, rng, idx)
        presets.append(params)
        idx += 1

    return presets[:500]


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--check", action="store_true")
    parser.add_argument("-o", "--output", type=Path, default=None)
    args = parser.parse_args()

    out = args.output or Path(__file__).resolve().parents[1] / "assets" / "factory" / "preset_bank.json"
    presets = generate()
    payload = {"version": 1, "count": len(presets), "presets": presets}
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(payload, indent=2), encoding="utf-8")

    cats: dict[str, int] = {}
    for p in presets:
        cats[p["category"]] = cats.get(p["category"], 0) + 1

    print(f"Wrote {len(presets)} presets to {out}")
    for cat, n in sorted(cats.items()):
        print(f"  {cat}: {n}")

    if args.check:
        assert len(presets) == 500
        for p in presets:
            assert not FORBIDDEN.search(p["name"])
            assert 0.05 <= p["gain"] <= 0.5
    return 0


if __name__ == "__main__":
    sys.exit(main())
