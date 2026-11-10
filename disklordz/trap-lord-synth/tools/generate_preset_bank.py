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

# Sam quota: dedicated synth categories + remainder split across classic trap families.
CATEGORY_QUOTAS: dict[str, int] = {
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
assert sum(CATEGORY_QUOTAS.values()) == 500

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
    "Swell",
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
        "category": "Church Keys/EP",
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
    {
        "name": "Rising Filter Swell",
        "category": "Swells",
        "engine": 2,
        "attack": 1.35,
        "decay": 0.55,
        "sustain": 0.82,
        "release": 1.6,
        "cutoff": 650,
        "gain": 0.18,
        "drive": 1.05,
        "detune": 11,
        "swellDepth": 0.8,
        "reverb": 0.42,
        "macroMovement": 0.45,
        "macroSpace": 0.55,
    },
    {
        "name": "Trap Cathedral Pad",
        "category": "Pads",
        "engine": 5,
        "attack": 0.65,
        "decay": 0.8,
        "sustain": 0.9,
        "release": 1.35,
        "cutoff": 3200,
        "detune": 10,
        "reverb": 0.38,
        "macroSpace": 0.45,
    },
    {
        "name": "Bright Synth Key Stack",
        "category": "Synth Keys",
        "engine": 9,
        "attack": 0.008,
        "decay": 0.4,
        "sustain": 0.75,
        "release": 0.5,
        "cutoff": 7000,
        "detune": 6,
        "reverb": 0.22,
    },
]

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

RECIPES_BY_CATEGORY: dict[str, list[dict]] = {
    "Pads": ["Trap Cathedral Pad", "Dark Orchestral Strings"],
    "Leads": ["Evil Analog Lead", "Dirty Trap Brass"],
    "Synth Keys": ["Bright Synth Key Stack", "Church Keys"],
    "Swells": ["Rising Filter Swell", "Dark Orchestral Strings"],
    "Choir": ["Trap Cathedral Pad"],
    "Brass": ["Dirty Trap Brass"],
    "Strings": ["Dark Orchestral Strings"],
    "Church Keys/EP": ["Church Keys"],
    "Organ": ["Southern Church Organ"],
    "Bells": ["Haunted Trap Bells"],
    "Bass": ["Dirty Trap Brass"],
    "Hits/FX": ["Dirty Trap Brass", "Haunted Trap Bells"],
}

RECIPE_BY_NAME = {r["name"]: r for r in BASE_RECIPES}


def clamp(v: float, lo: float, hi: float) -> float:
    return max(lo, min(hi, v))


def fingerprint(p: dict) -> str:
    keys = sorted(p.keys())
    rounded = "|".join(f"{k}:{round(float(p[k]), 3)}" for k in keys if k not in ("name", "category"))
    return hashlib.sha256(rounded.encode()).hexdigest()[:16]


def make_name(category: str, rng: random.Random, idx: int) -> str:
    short = category.replace("/", " ")
    for _ in range(40):
        name = f"{short} {rng.choice(ADJECTIVES)} {rng.choice(NOUNS)} {idx:02d}"
        if not FORBIDDEN.search(name):
            return name
    return f"{short} Neutral Stack {idx:02d}"


def pick_recipe(category: str, rng: random.Random) -> dict:
    names = RECIPES_BY_CATEGORY.get(category, ["Dirty Trap Brass"])
    name = rng.choice(names)
    return RECIPE_BY_NAME.get(name, BASE_RECIPES[0])


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
        "swellDepth": recipe.get("swellDepth", 0.0),
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
        p["swellDepth"] = 0.0
    elif category == "Hits/FX":
        p["attack"] = clamp(p["attack"] * 0.5, 0.001, 0.05)
        p["release"] = clamp(p["release"] * 0.6, 0.05, 1.2)
        p["swellDepth"] = 0.0
    elif category == "Pads":
        p["engine"] = 5 if rng.random() > 0.35 else 2
        p["attack"] = clamp(p["attack"] + rng.uniform(0.25, 0.9), 0.12, 2.8)
        p["swellDepth"] = clamp(rng.uniform(0.0, 0.25), 0, 0.25)
    elif category == "Leads":
        p["engine"] = rng.choice([6, 8, 0])
        p["attack"] = clamp(p["attack"], 0.002, 0.12)
        p["swellDepth"] = 0.0
    elif category == "Synth Keys":
        p["engine"] = rng.choice([9, 10, 3])
        p["cutoff"] = clamp(p["cutoff"] * rng.uniform(0.9, 1.2), 2500, 14000)
        p["swellDepth"] = 0.0
    elif category == "Choir":
        p["engine"] = 5
        p["attack"] = clamp(p["attack"] + 0.2, 0.08, 1.8)
        p["swellDepth"] = clamp(rng.uniform(0.0, 0.2), 0, 0.2)
    elif category == "Church Keys/EP":
        p["engine"] = rng.choice([9, 1, 3])
        p["chord"] = rng.choice([0, 2, 5])
        p["swellDepth"] = 0.0
    elif category == "Swells":
        p["engine"] = rng.choice([2, 5, 9, 2])
        p["attack"] = clamp(rng.uniform(0.35, 2.2), 0.35, 2.8)
        p["decay"] = clamp(rng.uniform(0.35, 1.0), 0.2, 2.5)
        p["release"] = clamp(rng.uniform(0.8, 2.5), 0.5, 3.5)
        p["cutoff"] = clamp(rng.uniform(350, 2200), 200, 4500)
        p["swellDepth"] = clamp(rng.uniform(0.45, 1.0), 0.4, 1.0)
        if rng.random() > 0.6:
            p["pitchEnvSemi"] = clamp(rng.uniform(-5, -1), -7, 0)
            p["pitchEnvMs"] = clamp(rng.uniform(120, 450), 80, 600)
        p["macroSpace"] = clamp(p["macroSpace"] + 0.15, 0, 1)

    return p


def generate(seed: int = 200604) -> list[dict]:
    rng = random.Random(seed)
    presets: list[dict] = []
    seen: set[str] = set()
    idx = 1

    for category, count in CATEGORY_QUOTAS.items():
        for _ in range(count):
            recipe = pick_recipe(category, rng)
            for attempt in range(96):
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

    return presets[:500]


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--check", action="store_true")
    parser.add_argument("-o", "--output", type=Path, default=None)
    args = parser.parse_args()

    out = args.output or Path(__file__).resolve().parents[1] / "assets" / "factory" / "preset_bank.json"
    presets = generate()
    payload = {"version": 2, "count": len(presets), "categoryQuotas": CATEGORY_QUOTAS, "presets": presets}
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(payload, indent=2), encoding="utf-8")

    cats: dict[str, int] = {}
    for p in presets:
        cats[p["category"]] = cats.get(p["category"], 0) + 1

    print(f"Wrote {len(presets)} presets to {out}")
    for cat, n in sorted(cats.items()):
        print(f"  {cat}: {n} (quota {CATEGORY_QUOTAS.get(cat, '?')})")

    if args.check:
        assert len(presets) == 500
        for cat, quota in CATEGORY_QUOTAS.items():
            assert cats.get(cat, 0) == quota, f"{cat} expected {quota} got {cats.get(cat)}"
        for p in presets:
            assert not FORBIDDEN.search(p["name"])
            assert 0.05 <= p["gain"] <= 0.5
        swell_count = sum(1 for p in presets if p["category"] == "Swells" and p.get("swellDepth", 0) > 0.4)
        assert swell_count >= 45
    return 0


if __name__ == "__main__":
    sys.exit(main())
