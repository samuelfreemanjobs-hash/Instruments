#!/usr/bin/env python3
"""Generate VoyagerPresetsGenerated.inc — 300 Moog / Voyager factory programs."""

from __future__ import annotations

import random
from pathlib import Path

TARGET_COUNT = 300
OUT = Path(__file__).resolve().parents[1] / "VoyagerPresetsGenerated.inc"


def _snap(
    name: str,
    levels,
    waves,
    octs,
    fines,
    noise,
    mix,
    cut,
    res,
    fdrive,
    fenv,
    fkb,
    fenv_times,
    amp_times,
    lfo_r,
    lfo_p,
    lfo_f,
    glide,
    master,
) -> dict:
    return {
        "name": name,
        "levels": levels,
        "waves": waves,
        "octs": octs,
        "fines": fines,
        "noise": noise,
        "mix": mix,
        "cut": cut,
        "res": res,
        "fdrive": fdrive,
        "fenv": fenv,
        "fkb": fkb,
        "fA": fenv_times[0],
        "fD": fenv_times[1],
        "fS": fenv_times[2],
        "fR": fenv_times[3],
        "aA": amp_times[0],
        "aD": amp_times[1],
        "aS": amp_times[2],
        "aR": amp_times[3],
        "lfoR": lfo_r,
        "lfoP": lfo_p,
        "lfoF": lfo_f,
        "glide": glide,
        "master": master,
    }


def _clamp(x: float, lo: float, hi: float) -> float:
    return max(lo, min(hi, x))


# First 24: hand-curated (Voyager + G-Funk + classic Moog roles)
CURATED: list[dict] = [
    _snap(
        "001 Voyager Init",
        [0.75, 0.50, 0.40],
        [0, 0, 2],
        [2, 2, 2],
        [0, 7, -7],
        0.03,
        2.0,
        2400,
        0.45,
        1.4,
        0.65,
        0.45,
        (0.01, 0.30, 0.30, 0.50),
        (0.005, 0.20, 0.85, 0.45),
        0.5,
        0,
        0.12,
        0.05,
        -2,
    ),
    _snap(
        "002 Classic Mono Lead",
        [0.85, 0.45, 0.30],
        [0, 0, 1],
        [2, 2, 3],
        [0, 12, -8],
        0.02,
        2.3,
        3200,
        0.52,
        1.5,
        0.75,
        0.55,
        (0.003, 0.18, 0.40, 0.40),
        (0.003, 0.15, 0.90, 0.35),
        0.4,
        0.02,
        0.10,
        0.08,
        -1.5,
    ),
    _snap(
        "003 G-Funk Lead",
        [0.78, 0.62, 0.48],
        [0, 0, 1],
        [2, 2, 3],
        [0, 9, -11],
        0.02,
        2.35,
        1800,
        0.58,
        1.55,
        0.82,
        0.62,
        (0.004, 0.22, 0.35, 0.45),
        (0.004, 0.18, 0.88, 0.35),
        0.35,
        0.04,
        0.08,
        0.14,
        -1.5,
    ),
    _snap(
        "004 Cali Glide Lead",
        [0.80, 0.55, 0.35],
        [0, 0, 0],
        [2, 2, 2],
        [0, 15, -15],
        0.01,
        2.2,
        2600,
        0.50,
        1.45,
        0.70,
        0.50,
        (0.008, 0.25, 0.45, 0.50),
        (0.006, 0.20, 0.92, 0.50),
        0.25,
        0,
        0.05,
        0.45,
        -2,
    ),
    _snap(
        "005 Bright Reso Lead",
        [0.70, 0.65, 0.55],
        [0, 1, 1],
        [2, 3, 3],
        [0, 5, 5],
        0.01,
        2.5,
        4500,
        0.72,
        1.6,
        0.60,
        0.70,
        (0.002, 0.15, 0.55, 0.35),
        (0.002, 0.12, 0.90, 0.30),
        0.6,
        0.03,
        0.15,
        0.06,
        -2,
    ),
    _snap(
        "006 Square Sizzle",
        [0.55, 0.80, 0.40],
        [1, 1, 0],
        [2, 2, 4],
        [0, 0, 20],
        0.04,
        2.6,
        3800,
        0.48,
        1.7,
        0.55,
        0.60,
        (0.005, 0.20, 0.50, 0.40),
        (0.004, 0.16, 0.88, 0.35),
        0.8,
        0.05,
        0.20,
        0.10,
        -1,
    ),
    _snap(
        "007 G-Funk Bass",
        [0.95, 0.55, 0.35],
        [0, 0, 0],
        [2, 2, 2],
        [0, 0, 0],
        0.05,
        2.1,
        520,
        0.62,
        1.85,
        0.55,
        0.40,
        (0.005, 0.35, 0.25, 0.50),
        (0.003, 0.18, 0.92, 0.35),
        0.3,
        0,
        0.05,
        0.11,
        2,
    ),
    _snap(
        "008 Sub Foundation",
        [1.0, 0.35, 0.20],
        [0, 0, 0],
        [1, 2, 2],
        [0, 0, 0],
        0.02,
        2.0,
        380,
        0.55,
        1.9,
        0.45,
        0.35,
        (0.008, 0.40, 0.20, 0.55),
        (0.004, 0.25, 0.95, 0.40),
        0.2,
        0,
        0,
        0.05,
        3,
    ),
    _snap(
        "009 Funk Pick Bass",
        [0.88, 0.60, 0.25],
        [0, 1, 0],
        [2, 2, 2],
        [0, 8, -5],
        0.06,
        2.4,
        680,
        0.58,
        2.0,
        0.65,
        0.45,
        (0.002, 0.22, 0.15, 0.35),
        (0.001, 0.12, 0.85, 0.25),
        0.4,
        0,
        0.08,
        0.07,
        1.5,
    ),
    _snap(
        "010 Rubber Bass",
        [0.90, 0.50, 0.30],
        [0, 2, 0],
        [2, 2, 1],
        [0, -10, 5],
        0.03,
        1.9,
        900,
        0.50,
        1.5,
        0.50,
        0.50,
        (0.015, 0.45, 0.35, 0.60),
        (0.010, 0.30, 0.90, 0.55),
        0.15,
        0,
        0.03,
        0.12,
        0,
    ),
    _snap(
        "011 Deep Sub Drive",
        [0.92, 0.45, 0.15],
        [0, 0, 2],
        [1, 2, 2],
        [0, 0, 0],
        0.08,
        2.8,
        420,
        0.65,
        2.2,
        0.40,
        0.30,
        (0.010, 0.50, 0.18, 0.60),
        (0.005, 0.30, 0.98, 0.45),
        0.1,
        0,
        0,
        0.04,
        4,
    ),
    _snap(
        "012 Silk Pad",
        [0.55, 0.55, 0.50],
        [2, 2, 2],
        [2, 2, 3],
        [0, 7, -7],
        0.01,
        1.6,
        1400,
        0.35,
        1.2,
        0.35,
        0.55,
        (0.80, 1.20, 0.70, 1.50),
        (0.60, 0.80, 0.95, 1.80),
        0.12,
        0.02,
        0.25,
        0.20,
        -3,
    ),
    _snap(
        "013 Voyager Dream",
        [0.60, 0.48, 0.42],
        [0, 2, 2],
        [2, 2, 4],
        [0, 11, -11],
        0.02,
        1.7,
        1100,
        0.42,
        1.25,
        0.40,
        0.60,
        (1.00, 1.50, 0.75, 2.00),
        (0.80, 1.00, 0.98, 2.20),
        0.08,
        0.03,
        0.30,
        0.25,
        -3.5,
    ),
    _snap(
        "014 Dark Atlas Pad",
        [0.50, 0.45, 0.55],
        [0, 0, 2],
        [1, 2, 3],
        [0, -5, 5],
        0.04,
        2.0,
        850,
        0.38,
        1.4,
        0.30,
        0.45,
        (0.50, 0.90, 0.65, 1.20),
        (0.40, 0.70, 0.92, 1.50),
        0.20,
        0,
        0.35,
        0.18,
        -2,
    ),
    _snap(
        "015 Soft Triangle Keys",
        [0.65, 0.40, 0.35],
        [2, 2, 2],
        [2, 2, 2],
        [0, 3, -3],
        0.01,
        1.5,
        3200,
        0.28,
        1.15,
        0.25,
        0.65,
        (0.02, 0.35, 0.45, 0.50),
        (0.008, 0.25, 0.75, 0.55),
        0.3,
        0,
        0.05,
        0.02,
        -2,
    ),
    _snap(
        "016 Brass Stab",
        [0.70, 0.75, 0.45],
        [0, 1, 1],
        [2, 2, 2],
        [0, 5, 0],
        0.02,
        2.7,
        2200,
        0.55,
        2.1,
        0.90,
        0.55,
        (0.001, 0.25, 0.05, 0.30),
        (0.001, 0.20, 0.70, 0.25),
        0.5,
        0,
        0.20,
        0.03,
        -1,
    ),
    _snap(
        "017 Pulse Stepper",
        [0.50, 0.70, 0.60],
        [3, 3, 1],
        [2, 2, 3],
        [0, 0, 0],
        0.01,
        2.2,
        1600,
        0.40,
        1.5,
        0.50,
        0.50,
        (0.005, 0.15, 0.30, 0.25),
        (0.003, 0.10, 0.80, 0.20),
        1.2,
        0,
        0.40,
        0.05,
        -2,
    ),
    _snap(
        "018 Filter Sweep FX",
        [0.40, 0.40, 0.40],
        [0, 0, 0],
        [2, 2, 2],
        [0, 0, 0],
        0.12,
        2.0,
        600,
        0.75,
        1.8,
        0.95,
        0.20,
        (0.05, 2.50, 0.10, 1.80),
        (0.02, 1.50, 0.90, 1.50),
        0.25,
        0,
        0.85,
        0.02,
        -3,
    ),
    _snap(
        "019 LFO Wobble",
        [0.65, 0.55, 0.45],
        [0, 0, 2],
        [2, 2, 2],
        [0, 10, -10],
        0.02,
        2.1,
        1200,
        0.48,
        1.45,
        0.35,
        0.40,
        (0.01, 0.30, 0.40, 0.50),
        (0.008, 0.22, 0.88, 0.45),
        2.5,
        0.08,
        0.75,
        0.10,
        -2,
    ),
    _snap(
        "020 Noise Rumble",
        [0.35, 0.30, 0.25],
        [0, 0, 2],
        [1, 2, 2],
        [0, 0, 0],
        0.45,
        2.5,
        450,
        0.60,
        2.0,
        0.50,
        0.25,
        (0.02, 0.60, 0.30, 0.80),
        (0.010, 0.40, 0.95, 0.70),
        0.15,
        0,
        0.10,
        0.04,
        0,
    ),
    _snap(
        "021 Filter Whistle",
        [0.30, 0.30, 0.25],
        [2, 2, 2],
        [3, 3, 4],
        [0, 0, 0],
        0.05,
        1.8,
        8000,
        0.92,
        1.3,
        0.20,
        0.80,
        (0.02, 0.40, 0.60, 0.50),
        (0.010, 0.30, 0.85, 0.40),
        0.4,
        0.10,
        0.05,
        0.06,
        -4,
    ),
    _snap(
        "022 West Coast Pluck",
        [0.75, 0.50, 0.35],
        [0, 0, 1],
        [2, 2, 3],
        [0, 14, -14],
        0.02,
        2.4,
        2800,
        0.55,
        1.65,
        0.85,
        0.58,
        (0.001, 0.18, 0.05, 0.28),
        (0.001, 0.15, 0.05, 0.22),
        0.35,
        0.02,
        0.12,
        0.09,
        -1,
    ),
    _snap(
        "023 Night Drive",
        [0.68, 0.52, 0.44],
        [0, 1, 2],
        [2, 2, 2],
        [0, -7, 7],
        0.03,
        2.3,
        950,
        0.52,
        1.75,
        0.60,
        0.48,
        (0.012, 0.35, 0.28, 0.45),
        (0.008, 0.28, 0.90, 0.40),
        0.45,
        0.03,
        0.18,
        0.16,
        -1,
    ),
    _snap(
        "024 Full Stack Unison",
        [0.72, 0.72, 0.68],
        [0, 0, 1],
        [2, 2, 3],
        [0, 18, -18],
        0.02,
        2.65,
        3400,
        0.56,
        1.75,
        0.68,
        0.65,
        (0.004, 0.20, 0.42, 0.38),
        (0.003, 0.16, 0.92, 0.32),
        0.55,
        0.05,
        0.14,
        0.11,
        -2.5,
    ),
]


def _jitter_snap(base: dict, rng: random.Random, strength: float) -> dict:
    s = dict(base)
    s["levels"] = [_clamp(v + rng.uniform(-0.12, 0.12) * strength, 0.15, 1.0) for v in base["levels"]]
    s["fines"] = [_clamp(v + rng.uniform(-8, 8) * strength, -50, 50) for v in base["fines"]]
    s["noise"] = _clamp(base["noise"] + rng.uniform(-0.04, 0.04) * strength, 0, 0.55)
    s["mix"] = _clamp(base["mix"] + rng.uniform(-0.35, 0.35) * strength, 1.0, 3.5)
    s["cut"] = _clamp(base["cut"] * (1.0 + rng.uniform(-0.35, 0.35) * strength), 80, 16000)
    s["res"] = _clamp(base["res"] + rng.uniform(-0.15, 0.15) * strength, 0.08, 0.95)
    s["fdrive"] = _clamp(base["fdrive"] + rng.uniform(-0.4, 0.4) * strength, 1.0, 3.2)
    s["fenv"] = _clamp(base["fenv"] + rng.uniform(-0.2, 0.2) * strength, 0, 1)
    s["fkb"] = _clamp(base["fkb"] + rng.uniform(-0.2, 0.2) * strength, 0, 1)
    s["glide"] = _clamp(base["glide"] + rng.uniform(-0.08, 0.08) * strength, 0, 1.2)
    s["lfoR"] = _clamp(base["lfoR"] * (1.0 + rng.uniform(-0.5, 0.5) * strength), 0.05, 18)
    s["master"] = _clamp(base["master"] + rng.uniform(-2, 2) * strength, -6, 5)
    if rng.random() < 0.15 * strength:
        wi = rng.randint(0, 2)
        waves = list(s["waves"])
        waves[wi] = rng.randint(0, 3)
        s["waves"] = waves
    return s


# Moog-era archetypes for programs 025–300
ARCHETYPES: list[tuple[str, dict]] = [
    ("Model D Lead", _snap("", [0.82, 0.38, 0.28], [0, 0, 1], [2, 2, 3], [0, 11, -9], 0.02, 2.25, 3000, 0.50, 1.55, 0.72, 0.52, (0.003, 0.16, 0.38, 0.35), (0.003, 0.14, 0.88, 0.32), 0.45, 0.02, 0.11, 0.06, -1.5)),
    ("Mini Bass", _snap("", [0.93, 0.48, 0.22], [0, 0, 0], [2, 2, 1], [0, 0, 0], 0.04, 2.15, 480, 0.58, 1.95, 0.52, 0.38, (0.004, 0.32, 0.22, 0.45), (0.003, 0.20, 0.94, 0.38), 0.25, 0, 0.04, 0.06, 2.5)),
    ("Voyager Mono", _snap("", [0.76, 0.58, 0.46], [0, 1, 1], [2, 2, 3], [0, 7, -7], 0.025, 2.3, 2100, 0.54, 1.6, 0.78, 0.58, (0.005, 0.20, 0.32, 0.42), (0.004, 0.17, 0.86, 0.36), 0.38, 0.03, 0.09, 0.12, -1)),
    ("Memorymoog Pad", _snap("", [0.58, 0.52, 0.50], [0, 2, 2], [2, 2, 4], [0, 9, -9], 0.015, 1.75, 1250, 0.36, 1.25, 0.38, 0.58, (0.65, 1.10, 0.72, 1.40), (0.55, 0.85, 0.96, 1.70), 0.18, 0.025, 0.28, 0.22, -3)),
    ("Modular FX", _snap("", [0.42, 0.38, 0.35], [0, 0, 2], [2, 3, 4], [0, 0, 0], 0.18, 2.05, 900, 0.68, 1.85, 0.88, 0.35, (0.02, 1.80, 0.15, 1.20), (0.015, 1.20, 0.88, 1.10), 0.55, 0.04, 0.65, 0.03, -2.5)),
    ("Ladder Reso", _snap("", [0.64, 0.62, 0.40], [0, 0, 2], [2, 2, 3], [0, 5, -5], 0.02, 2.4, 5200, 0.78, 1.65, 0.55, 0.62, (0.004, 0.18, 0.48, 0.38), (0.003, 0.15, 0.90, 0.30), 0.7, 0.04, 0.18, 0.05, -2)),
    ("West Coast Funk", _snap("", [0.80, 0.64, 0.42], [0, 0, 1], [2, 2, 3], [0, 10, -12], 0.03, 2.45, 1650, 0.60, 1.7, 0.80, 0.55, (0.004, 0.24, 0.30, 0.40), (0.003, 0.19, 0.87, 0.34), 0.42, 0.05, 0.10, 0.13, -0.5)),
    ("Triangle Keys", _snap("", [0.62, 0.44, 0.38], [2, 2, 2], [2, 2, 2], [0, 4, -4], 0.012, 1.55, 3400, 0.30, 1.2, 0.28, 0.62, (0.015, 0.32, 0.42, 0.48), (0.010, 0.26, 0.78, 0.52), 0.35, 0, 0.06, 0.02, -2)),
    ("Pulse Fifth", _snap("", [0.52, 0.68, 0.55], [3, 1, 1], [2, 2, 3], [0, 0, 7], 0.02, 2.2, 1900, 0.44, 1.5, 0.62, 0.50, (0.006, 0.18, 0.35, 0.32), (0.004, 0.14, 0.82, 0.28), 0.9, 0, 0.35, 0.04, -1.5)),
    ("Sub Octave", _snap("", [0.98, 0.32, 0.18], [0, 0, 0], [1, 2, 2], [0, 0, 0], 0.025, 2.05, 320, 0.52, 2.0, 0.42, 0.32, (0.006, 0.38, 0.18, 0.50), (0.004, 0.28, 0.97, 0.42), 0.12, 0, 0, 0.03, 3.5)),
    ("Glide Solo", _snap("", [0.78, 0.52, 0.36], [0, 0, 0], [2, 2, 2], [0, 14, -14], 0.015, 2.2, 2800, 0.48, 1.48, 0.68, 0.48, (0.008, 0.22, 0.40, 0.45), (0.006, 0.18, 0.91, 0.42), 0.28, 0.02, 0.08, 0.38, -1.5)),
    ("Brass Hit", _snap("", [0.72, 0.78, 0.48], [0, 1, 1], [2, 2, 2], [0, 6, 0], 0.025, 2.65, 2400, 0.52, 2.15, 0.92, 0.52, (0.001, 0.22, 0.06, 0.28), (0.001, 0.18, 0.72, 0.22), 0.55, 0, 0.22, 0.02, -0.5)),
    ("Noise Air", _snap("", [0.38, 0.32, 0.28], [2, 2, 2], [2, 3, 3], [0, 0, 0], 0.35, 1.95, 2200, 0.42, 1.35, 0.32, 0.55, (0.02, 0.45, 0.35, 0.55), (0.015, 0.35, 0.85, 0.50), 0.22, 0.02, 0.15, 0.08, -2)),
    ("Unison Stack", _snap("", [0.70, 0.70, 0.66], [0, 0, 1], [2, 2, 3], [0, 16, -16], 0.02, 2.55, 3600, 0.54, 1.72, 0.66, 0.64, (0.004, 0.19, 0.44, 0.36), (0.003, 0.16, 0.91, 0.30), 0.52, 0.06, 0.12, 0.09, -2)),
    ("Filter Growl", _snap("", [0.86, 0.52, 0.38], [0, 0, 2], [2, 2, 2], [0, -6, 6], 0.05, 2.75, 720, 0.66, 2.25, 0.58, 0.42, (0.005, 0.28, 0.24, 0.48), (0.004, 0.22, 0.93, 0.40), 0.35, 0, 0.12, 0.08, 1.5)),
]

MOOG_ADJECTIVES = [
    "Fat",
    "Warm",
    "Creamy",
    "Vintage",
    "Classic",
    "Golden",
    "Dark",
    "Bright",
    "Smooth",
    "Crispy",
    "Mighty",
    "Deep",
    "Silky",
    "Driven",
    "Liquid",
    "Analog",
    "Legend",
    "Studio",
    "Live",
    "Night",
]

MOOG_TAGS = [
    "Ladder",
    "Osc Bank",
    "Touch",
    "Wheel",
    "Panel",
    "Rack",
    "Stage",
    "Booth",
    "Session",
    "Take",
]


def _variant_name(index: int, archetype: str, variant: int, rng: random.Random) -> str:
    adj = MOOG_ADJECTIVES[(index + variant) % len(MOOG_ADJECTIVES)]
    tag = MOOG_TAGS[variant % len(MOOG_TAGS)]
    short = archetype.replace(" ", "")
    if variant % 4 == 0:
        label = f"{adj} {archetype}"
    elif variant % 4 == 1:
        label = f"{archetype} {tag}"
    elif variant % 4 == 2:
        label = f"{adj} Moog {short}"
    else:
        label = f"Moog {adj} {archetype.split()[-1]}"
    return f"{index:03d} {label}"


def build_bank() -> list[dict]:
    bank = [dict(c) for c in CURATED]
    assert len(bank) == 24

    idx = 25
    archetype_i = 0
    variant = 0
    while len(bank) < TARGET_COUNT:
        arche_name, arche = ARCHETYPES[archetype_i % len(ARCHETYPES)]
        rng = random.Random(0x4D00_47_00 + len(bank) * 9973)
        snap = _jitter_snap(arche, rng, strength=0.85 + (variant % 5) * 0.12)
        snap["name"] = _variant_name(idx, arche_name, variant, rng)
        bank.append(snap)
        idx += 1
        variant += 1
        if variant % 19 == 0:
            archetype_i += 1

    return bank


def _fmt_float(v: float) -> str:
    if abs(v - round(v)) < 1e-6 and abs(v) < 1e6:
        if abs(v) >= 100:
            return f"{v:.0f}.f"
        return f"{v:.1f}f" if abs(v) < 10 else f"{v:.0f}.f"
    return f"{v:.3f}f"


def emit_inc(bank: list[dict]) -> str:
    lines = [
        "// Auto-generated by scripts/generate_factory_presets.py — do not edit.",
        f"// {len(bank)} Moog / Voyager factory programs.",
        "const Snapshot kFactory[] = {",
    ]
    for s in bank:
        name = s["name"].replace('"', '\\"')
        lv = ", ".join(_fmt_float(x) for x in s["levels"])
        wv = ", ".join(str(x) for x in s["waves"])
        oc = ", ".join(str(x) for x in s["octs"])
        fi = ", ".join(_fmt_float(x) for x in s["fines"])
        lines.append(f'    {{ "{name}",')
        lines.append(f"      {{{lv}}}, {{{wv}}}, {{{oc}}}, {{{fi}}},")
        lines.append(
            f"      {_fmt_float(s['noise'])}, {_fmt_float(s['mix'])}, {_fmt_float(s['cut'])}, "
            f"{_fmt_float(s['res'])}, {_fmt_float(s['fdrive'])}, {_fmt_float(s['fenv'])}, {_fmt_float(s['fkb'])},"
        )
        lines.append(
            f"      {_fmt_float(s['fA'])},{_fmt_float(s['fD'])},{_fmt_float(s['fS'])},{_fmt_float(s['fR'])}, "
            f"{_fmt_float(s['aA'])},{_fmt_float(s['aD'])},{_fmt_float(s['aS'])},{_fmt_float(s['aR'])}, "
            f"{_fmt_float(s['lfoR'])},{_fmt_float(s['lfoP'])},{_fmt_float(s['lfoF'])}, "
            f"{_fmt_float(s['glide'])}, {_fmt_float(s['master'])} }},"
        )
    lines.append("};")
    lines.append("")
    lines.append("constexpr int kCount = static_cast<int>(sizeof(kFactory) / sizeof(kFactory[0]));")
    lines.append("static_assert(kCount == 300, \"V Voyager factory bank must contain 300 programs\");")
    return "\n".join(lines) + "\n"


def main() -> None:
    bank = build_bank()
    if len(bank) != TARGET_COUNT:
        raise SystemExit(f"Expected {TARGET_COUNT} presets, got {len(bank)}")
    OUT.write_text(emit_inc(bank), encoding="utf-8")
    print(f"Wrote {len(bank)} presets → {OUT}")


if __name__ == "__main__":
    main()
