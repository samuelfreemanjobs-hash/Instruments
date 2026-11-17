#!/usr/bin/env python3
"""DiskLordz RAGE APOCALYPSE — Rage, Opium, Hypertrap & Industrial Trap Factory.

Implements:
- 6 Rage sound banks: Redline, Chaos, Alien, Dark_Matter, Hyperdrive, Nebula
- 12 Rage Bass Reactor 808 designs synthesized at real musical pitches (Short/Med/Long)
- Clean / Mixed / Destroyed harmonic staging preserving mono sub below crossover
- 5 Kick architectures, 6 Snare families (incl. Chrome Snare), staggered Claps (Chaos Clap)
- Velocity-timbre 6-oscillator Hats (Hyper Hat), Experimental Percussion (Alien Laser, Glitch, Industrial, Metallic)
- Directed RAGE DNA mutation (balanced, violent, futuristic, expensive, minimal, experimental)
- Rage Groove Architect (6 groove families, 32-step MIDI export) and DISKLORDZ_RAGE_APOCALYPSE_VOL_1 layout
"""
from __future__ import annotations

import argparse
import hashlib
import json
import struct
from pathlib import Path

import numpy as np
import soundfile as sf

import forge_core_v2 as core

ENGINE = "DiskLordz RAGE APOCALYPSE v1"
TPQ = 480

BANKS = {
    "Redline": {"drive": 3.5, "bright": 1.08, "punch": 1.22, "space": 0.05, "bpm": 156.0},
    "Chaos": {"drive": 5.2, "bright": 1.15, "punch": 1.30, "space": 0.08, "bpm": 162.0},
    "Alien": {"drive": 2.4, "bright": 1.18, "punch": 1.05, "space": 0.14, "bpm": 152.0},
    "Dark_Matter": {"drive": 3.1, "bright": 0.78, "punch": 0.95, "space": 0.22, "bpm": 144.0},
    "Hyperdrive": {"drive": 4.4, "bright": 1.32, "punch": 1.28, "space": 0.10, "bpm": 172.0},
    "Nebula": {"drive": 1.8, "bright": 0.96, "punch": 0.88, "space": 0.18, "bpm": 146.0},
}

BASS_DESIGNS = (
    "Redline",
    "Chaos",
    "Alien_Slide",
    "Monster",
    "Nuclear",
    "Digital",
    "Hollow",
    "Elastic",
    "Short_Fuse",
    "Ghost",
    "Detuned",
    "Hybrid",
)

KICK_ARCHS = ("Apocalypse", "Attack", "Punch", "Distorted", "Digital")
SNARE_ARCHS = ("Chrome", "Dry_Crack", "Industrial", "Digital", "Air", "Laser")
CLAP_ARCHS = ("Chaos", "Short", "Wide", "Metallic", "Crushed", "Bright")
PERC_ARCHS = ("Digital_Rim", "Metallic_Pipe", "Spring_Tick")
FM_ARCHS = ("Alien_Laser", "Alien_Perc", "Plasma_Bell")
GLITCH_ARCHS = ("Bit_Pulse", "Stutter_Burst", "Industrial_Impact")
FX_ARCHS = ("Rage_Riser", "Downlifter_Zap")
HYBRID_ARCHS = ("Sub_Impact", "Layered_Slam")

LENGTH_SCALE = {"Short": 0.68, "Med": 1.0, "Long": 1.42}
DESTRUCTION_MODES = ("Clean", "Mixed", "Destroyed")
MUTATION_INTENTS = ("balanced", "violent", "futuristic", "expensive", "minimal", "experimental")

RAGE_NOTES_MIDI = {
    "E1": 28,
    "F1": 29,
    "Fs1": 30,
    "G1": 31,
    "A1": 33,
    "C1": 24,
}

CATEGORY_DIRS = {
    "808": "01_808_BASS",
    "kick": "02_KICKS",
    "snare": "03_SNARES",
    "clap": "04_CLAPS",
    "closed_hat": "05_CLOSED_HATS",
    "open_hat": "06_OPEN_HATS_AND_CYMBALS",
    "rim_perc": "07_RIMS_AND_PERCUSSION",
    "fm_digital": "08_FM_AND_DIGITAL",
    "glitch_ind": "09_GLITCH_AND_INDUSTRIAL",
    "fx": "10_TRANSITIONS_AND_FX",
    "hybrid": "11_HYBRID_HITS",
}

FULL_COUNTS = {
    "808": 80,
    "kick": 60,
    "snare": 60,
    "clap": 60,
    "closed_hat": 80,
    "open_hat": 40,
    "rim_perc": 50,
    "fm_digital": 50,
    "glitch_ind": 50,
    "fx": 40,
    "hybrid": 30,
}


def _time_for_tau(sr: int, tau_s: float, min_s: float = 0.09, max_s: float = 3.2) -> np.ndarray:
    duration = float(np.clip(tau_s * 6.2, min_s, max_s))
    return np.arange(int(sr * duration)) / sr


def _asymmetric_oversampled(x: np.ndarray, drive: float, fold: float = 0.0) -> np.ndarray:
    from scipy.signal import resample_poly

    up = resample_poly(x, 4, 1, axis=0)
    driven = up * max(drive, 0.5)
    pos = np.tanh(driven * 1.2)
    neg = -np.tanh(-driven * 0.9) * 0.92
    shaped = np.where(driven >= 0.0, pos, neg)
    if fold > 0.0:
        shaped = (1.0 - fold) * shaped + fold * np.sin(shaped * np.pi * (1.0 + 0.8 * fold))
    down = resample_poly(shaped, 1, 4, axis=0)[: len(x)]
    if len(down) < len(x):
        down = np.pad(down, (0, len(x) - len(down)))
    down = down - np.mean(down, axis=0)
    norm = max(np.tanh(max(drive, 0.5)), 0.35)
    return down / norm


def _split_band_destroy(
    x: np.ndarray,
    sr: int,
    drive: float,
    destruction: str = "Mixed",
    crossover_hz: float = 110.0,
    fold: float = 0.0,
) -> np.ndarray:
    """Parallel Clean Sub (low-pass) + Dirt Layer (oversampled waveshaper -> high-pass)."""
    sub = core.filter_signal(x, crossover_hz, "lowpass", sr, 4)
    air_clean = core.filter_signal(x, crossover_hz, "highpass", sr, 4)
    if destruction == "Clean":
        dirt = core.filter_signal(_asymmetric_oversampled(x, max(1.05, drive * 0.35), 0.0), crossover_hz, "highpass", sr, 3)
        out = sub + 0.70 * air_clean + 0.18 * dirt
    elif destruction == "Destroyed":
        dirt = core.filter_signal(
            _asymmetric_oversampled(x, max(2.5, drive * 1.65), min(0.65, fold + 0.25)), crossover_hz, "highpass", sr, 3
        )
        sub_warm = core.filter_signal(_asymmetric_oversampled(sub, 1.35, 0.0), crossover_hz * 1.15, "lowpass", sr, 3)
        out = 0.92 * sub_warm + 1.15 * dirt
    else:
        dirt = core.filter_signal(_asymmetric_oversampled(x, max(1.6, drive), fold), crossover_hz, "highpass", sr, 3)
        out = sub + 0.25 * air_clean + 0.68 * dirt
    return core.filter_signal(out, 20.0, "highpass", sr, 2)


def apply_rage_dna(bank_cfg: dict, intent: str, strength: float, rng: np.random.Generator) -> dict:
    cfg = dict(bank_cfg)
    s = float(np.clip(strength, 0.0, 1.0))
    jitter = float(rng.uniform(-0.06, 0.06)) * s
    if intent == "violent":
        cfg["drive"] *= 1.0 + 0.55 * s + jitter
        cfg["punch"] *= 1.0 + 0.35 * s
    elif intent == "futuristic":
        cfg["bright"] *= 1.0 + 0.32 * s
        cfg["space"] = min(0.32, cfg["space"] + 0.08 * s)
    elif intent == "expensive":
        cfg["drive"] *= 1.0 - 0.15 * s
        cfg["punch"] *= 1.0 + 0.14 * s
        cfg["bright"] = float(np.clip(cfg["bright"], 0.9, 1.15))
    elif intent == "minimal":
        cfg["space"] *= max(0.15, 1.0 - 0.65 * s)
        cfg["bright"] *= 1.0 - 0.18 * s
    elif intent == "experimental":
        cfg["drive"] *= 1.0 + 0.35 * s
        cfg["bright"] *= 1.0 + float(rng.uniform(-0.2, 0.35)) * s
    else:
        cfg["drive"] *= 1.0 + float(rng.uniform(-0.18, 0.22)) * s
        cfg["punch"] *= 1.0 + float(rng.uniform(-0.12, 0.16)) * s
    return cfg


def gen_rage_808(
    sr: int,
    rng: np.random.Generator,
    design: str = "Redline",
    midi: int = 30,
    length: str = "Med",
    destruction: str = "Mixed",
    bank_cfg: dict | None = None,
) -> tuple[np.ndarray, float]:
    """Synthesize one of the 12 Rage Bass Reactor 808 designs at exact MIDI pitch."""
    cfg = bank_cfg or BANKS["Redline"]
    root_hz = core.note_to_hz(midi)
    scale = LENGTH_SCALE.get(length, 1.0)

    specs = {
        "Redline": (175.0, 0.032, 0.480, 3.5, 0.20, 0.0, 0.0),
        "Chaos": (220.0, 0.022, 0.360, 5.2, 0.28, 0.18, 0.0),
        "Alien_Slide": (140.0, 0.055, 0.750, 2.2, 0.15, 0.0, 0.090),
        "Monster": (165.0, 0.028, 0.680, 3.8, 0.32, 0.08, 0.0),
        "Nuclear": (205.0, 0.024, 0.540, 6.4, 0.35, 0.32, 0.0),
        "Digital": (185.0, 0.020, 0.420, 4.1, 0.42, 0.24, 0.0),
        "Hollow": (150.0, 0.026, 0.460, 2.8, 0.18, 0.0, 0.0),
        "Elastic": (195.0, 0.048, 0.500, 3.0, 0.22, 0.05, 0.045),
        "Short_Fuse": (210.0, 0.015, 0.210, 4.0, 0.24, 0.08, 0.0),
        "Ghost": (125.0, 0.030, 0.560, 1.6, 0.08, 0.0, 0.0),
        "Detuned": (170.0, 0.028, 0.520, 3.4, 0.25, 0.10, 0.0),
        "Hybrid": (185.0, 0.026, 0.490, 4.3, 0.30, 0.14, 0.0),
    }
    start_hz, sweep_tau, amp_tau, base_drive, tri_mix, fold, glide_s = specs[design]
    amp_tau = amp_tau * scale
    t = _time_for_tau(sr, amp_tau, min_s=0.45, max_s=3.4)

    pitch = root_hz + max(25.0, start_hz - root_hz) * np.exp(-t / sweep_tau)
    if glide_s > 0.0:
        slide_from = root_hz * (2.0 ** (5.0 / 12.0))
        glide_env = np.exp(-t / glide_s) * np.exp(-np.maximum(t - 0.18, 0.0) / 0.04)
        pitch = pitch + (slide_from - root_hz) * 0.35 * glide_env * (t < 0.20)
    if design == "Elastic":
        pitch = pitch + 9.0 * np.sin(2.0 * np.pi * 11.0 * t) * np.exp(-t / 0.09)

    phase = 2.0 * np.pi * np.cumsum(pitch) / sr
    sine = np.sin(phase)
    tri = (2.0 / np.pi) * np.arcsin(np.sin(phase))
    osc_sig = (1.0 - tri_mix) * sine + tri_mix * tri

    if design == "Monster":
        osc_sig = osc_sig + 0.28 * np.sin(2.0 * phase) + 0.12 * np.sin(3.0 * phase)
    elif design == "Digital":
        pd = np.sin(phase + 0.85 * np.sin(phase) * np.exp(-t / 0.18))
        osc_sig = 0.65 * osc_sig + 0.35 * pd
    elif design == "Hollow":
        osc_sig = osc_sig - 0.24 * np.sin(2.0 * phase)
    elif design == "Detuned":
        beat_hi = core.filter_signal(
            0.22 * np.sin(2.0 * np.pi * np.cumsum(pitch * 2.015) / sr)
            + 0.22 * np.sin(2.0 * np.pi * np.cumsum(pitch * 1.985) / sr),
            135.0,
            "highpass",
            sr,
            3,
        )
        osc_sig = osc_sig + beat_hi

    amp = (1.0 - np.exp(-t / 0.0006)) * np.exp(-t / amp_tau)
    click = core.filter_signal(rng.standard_normal(len(t)), 1900.0, "highpass", sr, 2)
    click *= np.exp(-t / 0.0022) * 0.08 * cfg["punch"]

    raw = osc_sig * amp + click
    drive = base_drive * (cfg["drive"] / 3.5)
    voiced = _split_band_destroy(raw, sr, drive=drive, destruction=destruction, crossover_hz=110.0, fold=fold)
    return voiced, root_hz


def gen_rage_kick(
    sr: int,
    rng: np.random.Generator,
    arch: str = "Apocalypse",
    destruction: str = "Mixed",
    bank_cfg: dict | None = None,
) -> tuple[np.ndarray, None]:
    cfg = bank_cfg or BANKS["Redline"]
    if arch == "Apocalypse":
        t = _time_for_tau(sr, 0.105, min_s=0.22, max_s=0.55)
        freq = 54.0 + (145.0 - 54.0) * np.exp(-t / 0.018)
        body = core.osc(freq, sr) * (1.0 - np.exp(-t / 0.0003)) * np.exp(-t / 0.105)
        click = core.filter_signal(rng.standard_normal(len(t)), 2400.0, "highpass", sr, 2)
        click *= np.exp(-t / 0.003) * 0.22 * cfg["punch"]
        drive = 2.0
    elif arch == "Attack":
        t = _time_for_tau(sr, 0.075, min_s=0.18, max_s=0.4)
        freq = 56.0 + 165.0 * np.exp(-t / 0.011)
        body = core.osc(freq, sr) * np.exp(-t / 0.075)
        click = core.filter_signal(rng.standard_normal(len(t)), 3200.0, "highpass", sr, 2) * np.exp(-t / 0.0018) * 0.30
        drive = 2.3
    elif arch == "Punch":
        t = _time_for_tau(sr, 0.115, min_s=0.24, max_s=0.5)
        freq = 52.0 + 125.0 * np.exp(-t / 0.022)
        body = core.osc(freq, sr) * np.exp(-t / 0.115)
        click = core.filter_signal(rng.standard_normal(len(t)), 1800.0, "highpass", sr, 2) * np.exp(-t / 0.0025) * 0.2
        drive = 2.6
    elif arch == "Distorted":
        t = _time_for_tau(sr, 0.12, min_s=0.25, max_s=0.55)
        freq = 50.0 + 155.0 * np.exp(-t / 0.016)
        body = _asymmetric_oversampled(core.osc(freq, sr) * np.exp(-t / 0.12), 4.2, 0.12)
        click = core.filter_signal(rng.standard_normal(len(t)), 2600.0, "highpass", sr, 2) * np.exp(-t / 0.002) * 0.26
        return body + click, None
    else:
        t = _time_for_tau(sr, 0.09, min_s=0.2, max_s=0.45)
        mod = np.sin(2.0 * np.pi * 310.0 * t) * 2.8 * np.exp(-t / 0.009)
        phase = 2.0 * np.pi * np.cumsum(55.0 + 160.0 * np.exp(-t / 0.013)) / sr
        body = np.sin(phase + mod) * np.exp(-t / 0.09)
        click = core.filter_signal(rng.standard_normal(len(t)), 3500.0, "highpass", sr, 2) * np.exp(-t / 0.0015) * 0.22
        drive = 2.4
    raw = body + click
    return _split_band_destroy(raw, sr, drive=drive * (cfg["drive"] / 3.5), destruction=destruction, crossover_hz=120.0), None


def gen_rage_snare(
    sr: int,
    rng: np.random.Generator,
    arch: str = "Chrome",
    destruction: str = "Mixed",
    bank_cfg: dict | None = None,
) -> tuple[np.ndarray, None]:
    cfg = bank_cfg or BANKS["Redline"]
    t = _time_for_tau(sr, 0.095, min_s=0.26, max_s=0.65)
    noise = rng.standard_normal(len(t))
    if arch == "Chrome":
        r1 = core.osc(190.0 + 75.0 * np.exp(-t / 0.010), sr) * np.exp(-t / 0.045)
        r2 = core.osc(335.0 + 90.0 * np.exp(-t / 0.008), sr) * np.exp(-t / 0.045) * 0.58
        bp = core.filter_signal(core.filter_signal(noise, 1300.0, "highpass", sr, 2), 8200.0, "lowpass", sr, 2)
        raw = 0.58 * (r1 + r2) + bp * np.exp(-t / 0.095) * 0.72
    elif arch == "Dry_Crack":
        r1 = core.osc(215.0 + 95.0 * np.exp(-t / 0.007), sr) * np.exp(-t / 0.032)
        r2 = core.osc(390.0, sr) * np.exp(-t / 0.022) * 0.42
        snap = core.filter_signal(noise, 2200.0, "highpass", sr, 3) * np.exp(-t / 0.052) * 0.65
        raw = 0.65 * (r1 + r2) + snap
    elif arch == "Industrial":
        body = core.osc(182.0 + 110.0 * np.exp(-t / 0.012), sr) * np.exp(-t / 0.05)
        clang = np.sin(2.0 * np.pi * 612.0 * t + 2.4 * np.sin(2.0 * np.pi * 1490.0 * t) * np.exp(-t / 0.03)) * np.exp(-t / 0.055)
        wires = core.filter_signal(noise, 1600.0, "highpass", sr, 2) * np.exp(-t / 0.09)
        raw = 0.55 * body + 0.32 * clang + 0.62 * wires
    elif arch == "Digital":
        mod = np.sin(2.0 * np.pi * 540.0 * t) * 3.1 * np.exp(-t / 0.025)
        tone = np.sin(2.0 * np.pi * 225.0 * t + mod) * np.exp(-t / 0.042)
        wires = core.filter_signal(noise, 2600.0, "highpass", sr, 3) * np.exp(-t / 0.075) * 0.68
        raw = np.round((0.55 * tone + wires) * 64.0) / 64.0
    elif arch == "Air":
        body = core.osc(200.0 + 60.0 * np.exp(-t / 0.01), sr) * np.exp(-t / 0.038) * 0.48
        air = core.filter_signal(noise, 3400.0, "highpass", sr, 2) * np.exp(-t / 0.125) * 0.78
        raw = body + air
    else:
        sweep = 980.0 * np.exp(-t / 0.018) + 185.0
        raw = 0.58 * core.osc(sweep, sr) * np.exp(-t / 0.055) + core.filter_signal(noise, 2000.0, "highpass", sr, 2) * np.exp(-t / 0.07) * 0.58

    drive = 1.1 if destruction == "Clean" else (3.6 if destruction == "Destroyed" else 2.1)
    shaped = _asymmetric_oversampled(raw, drive * (cfg["drive"] / 3.5), 0.08 if destruction == "Destroyed" else 0.0)
    if cfg["space"] > 0.06:
        return core.plate_reverb(shaped, sr, decay=0.14 + cfg["space"] * 0.3, wet=min(0.22, cfg["space"])), None
    return shaped, None


def gen_rage_clap(
    sr: int,
    rng: np.random.Generator,
    arch: str = "Chaos",
    destruction: str = "Mixed",
    bank_cfg: dict | None = None,
) -> tuple[np.ndarray, None]:
    cfg = bank_cfg or BANKS["Redline"]
    tail_tau = 0.065 if arch != "Short" else 0.038
    t = _time_for_tau(sr, tail_tau, min_s=0.22, max_s=0.55)
    offsets_ms = (0.0, 9.0, 19.0, 31.0) if arch == "Chaos" else (0.0, 8.0 + float(rng.uniform(0, 3)), 17.0 + float(rng.uniform(0, 4)), 28.0)
    env = np.zeros_like(t)
    for idx, ms in enumerate(offsets_ms):
        s = ms * 0.001
        tau = 0.007 if idx < 3 else tail_tau
        env += np.exp(-np.maximum(t - s, 0.0) / tau) * (t >= s)

    n_l = rng.standard_normal(len(t))
    n_r = rng.standard_normal(len(t)) if arch == "Wide" else n_l
    lo_cut = 1100.0 if arch != "Bright" else 1800.0
    hi_cut = 3400.0 if arch == "Chaos" else min(sr * 0.42, 7600.0 * cfg["bright"])

    def _band(sig: np.ndarray) -> np.ndarray:
        y = core.filter_signal(sig, lo_cut, "highpass", sr, 2)
        return core.filter_signal(y, hi_cut, "lowpass", sr, 2) * env * 0.55

    left = _band(n_l)
    right = _band(n_r)
    stereo = np.column_stack((left, right)) if arch == "Wide" else left
    if arch == "Crushed" or destruction == "Destroyed":
        stereo = np.round(stereo * 32.0) / 32.0
    drive = 1.15 if destruction == "Clean" else (3.0 if destruction == "Destroyed" else 1.85)
    return _asymmetric_oversampled(stereo, drive), None


def gen_rage_hat(
    sr: int,
    rng: np.random.Generator,
    is_open: bool = False,
    velocity: float = 0.9,
    destruction: str = "Mixed",
    bank_cfg: dict | None = None,
) -> tuple[np.ndarray, None]:
    """6-oscillator metallic network where velocity changes brightness and attack, not just level."""
    cfg = bank_cfg or BANKS["Redline"]
    vel = float(np.clip(velocity, 0.15, 1.0))
    tau = (0.19 if is_open else 0.024) * (0.82 + 0.32 * vel)
    t = _time_for_tau(sr, tau, min_s=0.09 if not is_open else 0.45, max_s=1.2)

    base = min(float(rng.uniform(3050.0, 3550.0)) * (0.86 + 0.22 * vel), sr * 0.19)
    ratios = (1.0, 1.37, 1.79, 2.31, 2.93, 3.67)
    metal = np.zeros(len(t))
    for i, ratio in enumerate(ratios):
        freq = min(base * ratio, sr * 0.45)
        tilt = (0.54 + 0.18 * vel) ** i
        metal += tilt * np.sin(2.0 * np.pi * freq * t + float(rng.uniform(0, 2 * np.pi)))

    white = rng.standard_normal(len(t))
    hp_hz = float(np.clip((5800.0 + 2600.0 * vel) * cfg["bright"], 3600.0, min(13500.0, sr * 0.36)))
    air = core.filter_signal(white, hp_hz, "highpass", sr, 3)
    attack_s = 0.00018 if vel > 0.65 else 0.0009
    env = (1.0 - np.exp(-t / attack_s)) * np.exp(-t / tau)
    raw = (0.56 * metal + 0.44 * air) * env * (0.45 + 0.55 * vel)
    if destruction == "Destroyed":
        raw = _asymmetric_oversampled(np.round(raw * 48.0) / 48.0, 2.2)
    return raw, None


def gen_experimental(
    sr: int,
    rng: np.random.Generator,
    category: str,
    arch: str,
    destruction: str = "Mixed",
) -> tuple[np.ndarray, None]:
    if category == "fm_digital":
        if arch == "Alien_Laser":
            t = _time_for_tau(sr, 0.110, min_s=0.28, max_s=0.75)
            car_hz = 220.0 + (960.0 - 220.0) * np.exp(-t / 0.042)
            mod_phase = 2.0 * np.pi * np.cumsum(car_hz * 2.7) / sr
            mod = np.sin(mod_phase) * (3.4 * np.exp(-t / 0.055))
            sig = np.sin(2.0 * np.pi * np.cumsum(car_hz) / sr + mod) * np.exp(-t / 0.110)
            sig = core.filter_signal(sig, 4200.0, "lowpass", sr, 2)
            return _asymmetric_oversampled(sig, 1.9), None
        if arch == "Plasma_Bell":
            t = _time_for_tau(sr, 0.20, min_s=0.45, max_s=1.1)
            car = float(rng.uniform(760.0, 1080.0))
            mod = np.sin(2.0 * np.pi * car * 2.5 * t) * (2.4 * np.exp(-t / 0.07))
            sig = np.sin(2.0 * np.pi * car * t + mod) * np.exp(-t / 0.20)
            return sig, None
        t = _time_for_tau(sr, 0.075, min_s=0.2, max_s=0.5)
        car = float(rng.uniform(440.0, 680.0))
        mod = np.sin(2.0 * np.pi * car * 3.14159 * t) * (5.2 * np.exp(-t / 0.03))
        return np.sin(2.0 * np.pi * car * t + mod) * np.exp(-t / 0.075), None

    if category == "rim_perc":
        t = _time_for_tau(sr, 0.045, min_s=0.14, max_s=0.35)
        if arch == "Digital_Rim":
            r = np.sin(2.0 * np.pi * 400.0 * t) * np.exp(-t / 0.022) + 0.65 * np.sin(2.0 * np.pi * 1200.0 * t) * np.exp(-t / 0.014)
        else:
            base = float(rng.uniform(520.0, 880.0))
            r = sum((0.6 ** i) * np.sin(2.0 * np.pi * base * ratio * t) for i, ratio in enumerate((1.0, 1.58, 2.23, 2.91)))
            r = r * np.exp(-t / 0.045)
        click = core.filter_signal(rng.standard_normal(len(t)), 2600.0, "highpass", sr, 2) * np.exp(-t / 0.0018) * 0.25
        return _asymmetric_oversampled(r + click, 1.7), None

    if category == "glitch_ind":
        t = _time_for_tau(sr, 0.085, min_s=0.22, max_s=0.55)
        gate = (np.sin(2.0 * np.pi * float(rng.uniform(28.0, 54.0)) * t) > -0.15).astype(float)
        src = core.osc(140.0 + 520.0 * np.exp(-t / 0.025), sr) + 0.55 * core.filter_signal(rng.standard_normal(len(t)), 1500.0, "highpass", sr, 2)
        crushed = np.round(src * 16.0) / 16.0
        sig = crushed * gate * np.exp(-t / 0.085)
        return _asymmetric_oversampled(sig, 2.8 if destruction != "Clean" else 1.3), None

    if category == "fx":
        duration = 0.55
        t = np.arange(int(sr * duration)) / sr
        if arch == "Rage_Riser":
            freq = 240.0 * (2.0 ** (3.2 * (t / duration)))
            env = (t / duration) ** 1.8 * np.exp(-np.maximum(t - duration * 0.88, 0.0) / 0.02)
        else:
            freq = 2200.0 * np.exp(-t / 0.14) + 75.0
            env = np.exp(-t / 0.18)
        sweep = core.osc(freq, sr) + 0.35 * core.filter_signal(rng.standard_normal(len(t)), 1800.0, "highpass", sr, 2)
        return _asymmetric_oversampled(sweep * env, 2.0), None

    t = _time_for_tau(sr, 0.14, min_s=0.32, max_s=0.75)
    sub = core.osc(48.0 + 170.0 * np.exp(-t / 0.015), sr) * np.exp(-t / 0.14)
    metal = np.sin(2.0 * np.pi * 740.0 * t + 3.0 * np.sin(2.0 * np.pi * 1980.0 * t) * np.exp(-t / 0.025)) * np.exp(-t / 0.05)
    return _split_band_destroy(sub + 0.42 * metal, sr, drive=3.4, destruction=destruction), None


def _vlq(value: int) -> bytes:
    buf = bytearray([value & 0x7F])
    value >>= 7
    while value:
        buf.append((value & 0x7F) | 0x80)
        value >>= 7
    return bytes(reversed(buf))


def build_rage_groove(bank: str, rng: np.random.Generator, root_midi: int = 30) -> tuple[list[tuple[int, int, int, int, int]], float]:
    """32-step (2-bar) Rage Groove Architect pattern with exact rational tick math."""
    bpm = BANKS[bank]["bpm"]
    step_ticks = TPQ // 4
    events: list[tuple[int, int, int, int, int]] = []
    kick_steps = {0, 6, 16, 22, 26} if bank != "Dark_Matter" else {0, 10, 16, 26}
    snare_steps = {4, 12, 20, 28}
    roll_steps = {7, 15, 23, 31} if bank in ("Hyperdrive", "Chaos") else {15, 31}

    for step in range(32):
        tick = step * step_ticks
        if step in kick_steps and (step in (0, 16) or rng.random() < 0.85):
            events.append((tick, 80, 36, 118 if step in (0, 16) else 102, 9))
        if step in snare_steps:
            events.append((tick, 70, 38, 114, 9))
            if bank in ("Redline", "Hyperdrive"):
                events.append((tick, 65, 39, 98, 9))
        if step in roll_steps:
            sub_count = 3 if bank == "Alien" else 4
            for r in range(sub_count):
                rt = tick + (step_ticks * r) // sub_count
                events.append((rt, max(18, step_ticks // sub_count - 4), 42, 72 + 12 * r, 9))
        elif bank != "Dark_Matter" or step % 2 == 0:
            events.append((tick, 42, 42, 96 if step % 4 == 0 else 72, 9))
        if step in (14, 30):
            events.append((tick, 95, 46, 94, 9))
        if step in (0, 6, 16, 22):
            note = root_midi + (7 if (bank == "Alien" and step == 22) else 0)
            events.append((tick, step_ticks * 4, note, 116, 0))
    return events, bpm


def write_midi_file(path: Path, events: list[tuple[int, int, int, int, int]], bpm: float) -> None:
    tempo = int(60_000_000 / bpm)
    msgs = []
    for start, dur, note, vel, ch in events:
        s = max(0, int(start))
        msgs.append((s, 1, 0x90 | (ch & 0x0F), note & 0x7F, vel & 0x7F))
        msgs.append((s + max(1, int(dur)), 0, 0x80 | (ch & 0x0F), note & 0x7F, 0))
    msgs.sort()
    track = bytearray(b"\x00\xff\x51\x03" + struct.pack(">I", tempo)[1:])
    last = 0
    for tick, _prio, status, note, vel in msgs:
        track += _vlq(tick - last) + bytes((status, note, vel))
        last = tick
    track += b"\x00\xff\x2f\x00"
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(b"MThd" + struct.pack(">IHHH", 6, 0, 1, TPQ) + b"MTrk" + struct.pack(">I", len(track)) + track)


def build_apocalypse_pack(
    out_dir: Path,
    sr: int = 48000,
    bits: int = 24,
    seed: int = 666808,
    tier: str = "demo",
    ceiling_db: float = -1.0,
) -> dict:
    out_dir.mkdir(parents=True, exist_ok=True)
    scale = {"demo": 0.04, "starter": 0.20, "standard": 0.50, "mega": 1.0}[tier]
    counts = {k: max(1, int(round(v * scale))) for k, v in FULL_COUNTS.items()}
    groove_count = {"demo": 6, "starter": 24, "standard": 60, "mega": 120}[tier]
    subtype = "PCM_24" if bits == 24 else "PCM_16"

    bank_names = list(BANKS)
    note_items = list(RAGE_NOTES_MIDI.items())
    lengths = ("Short", "Med", "Long")
    manifest_rows = []

    for cat, total in counts.items():
        for idx in range(total):
            bank = bank_names[idx % len(bank_names)]
            intent = MUTATION_INTENTS[idx % len(MUTATION_INTENTS)]
            destruction = DESTRUCTION_MODES[idx % len(DESTRUCTION_MODES)]
            sample_seed = seed + sum(cat.encode()) * 1000 + idx * 37
            rng = np.random.default_rng(sample_seed)
            cfg = apply_rage_dna(BANKS[bank], intent, 0.45, rng)

            root_hz = None
            note_tag = ""
            if cat == "808":
                design = BASS_DESIGNS[idx % len(BASS_DESIGNS)]
                note_tag, midi = note_items[idx % len(note_items)]
                length_tag = lengths[idx % len(lengths)]
                audio, root_hz = gen_rage_808(sr, rng, design, midi, length_tag, destruction, cfg)
                sub_folder = design if design in ("Redline", "Chaos", "Alien_Slide", "Monster") else "Other_808s"
                fname = f"DL_RAGE_{design}808_{note_tag}_{length_tag}_{idx + 1:03d}.wav"
                rel = Path(CATEGORY_DIRS[cat]) / sub_folder / fname
            elif cat == "kick":
                arch = KICK_ARCHS[idx % len(KICK_ARCHS)]
                audio, _ = gen_rage_kick(sr, rng, arch, destruction, cfg)
                rel = Path(CATEGORY_DIRS[cat]) / f"DL_RAGE_{bank}Kick_{arch}_{idx + 1:03d}.wav"
            elif cat == "snare":
                arch = SNARE_ARCHS[idx % len(SNARE_ARCHS)]
                audio, _ = gen_rage_snare(sr, rng, arch, destruction, cfg)
                rel = Path(CATEGORY_DIRS[cat]) / f"DL_RAGE_{arch}Snare_{destruction}_{idx + 1:03d}.wav"
            elif cat == "clap":
                arch = CLAP_ARCHS[idx % len(CLAP_ARCHS)]
                audio, _ = gen_rage_clap(sr, rng, arch, destruction, cfg)
                rel = Path(CATEGORY_DIRS[cat]) / f"DL_RAGE_{arch}Clap_{destruction}_{idx + 1:03d}.wav"
            elif cat in ("closed_hat", "open_hat"):
                vel = (0.48, 0.76, 1.0)[idx % 3]
                vel_tag = ("Soft", "Med", "Hard")[idx % 3]
                audio, _ = gen_rage_hat(sr, rng, is_open=(cat == "open_hat"), velocity=vel, destruction=destruction, bank_cfg=cfg)
                kind = "OpenHat" if cat == "open_hat" else "HyperHat"
                rel = Path(CATEGORY_DIRS[cat]) / f"DL_RAGE_{bank}_{kind}_{vel_tag}_{idx + 1:03d}.wav"
            else:
                pool = {
                    "rim_perc": PERC_ARCHS,
                    "fm_digital": FM_ARCHS,
                    "glitch_ind": GLITCH_ARCHS,
                    "fx": FX_ARCHS,
                    "hybrid": HYBRID_ARCHS,
                }[cat]
                arch = pool[idx % len(pool)]
                audio, _ = gen_experimental(sr, rng, cat, arch, destruction)
                rel = Path(CATEGORY_DIRS[cat]) / f"DL_RAGE_{bank}_{arch}_{idx + 1:03d}.wav"

            mastered = core.master(audio, sr, ceiling_db - 0.12)
            dest = out_dir / rel
            dest.parent.mkdir(parents=True, exist_ok=True)
            sf.write(dest, mastered, sr, subtype=subtype)
            tp = core.true_peak_db(mastered)
            manifest_rows.append(
                {
                    "file": rel.as_posix(),
                    "bank": bank,
                    "category": cat,
                    "destruction": destruction,
                    "intent": intent,
                    "root_note": note_tag.replace("s", "#") if note_tag else "",
                    "root_hz": round(root_hz, 4) if root_hz else "",
                    "true_peak_dbtp": round(tp, 3),
                    "seed": sample_seed,
                    "sha256": hashlib.sha256(dest.read_bytes()).hexdigest(),
                }
            )

    midi_paths = []
    for g_idx in range(groove_count):
        bank = bank_names[g_idx % len(bank_names)]
        rng = np.random.default_rng(seed + 90_000 + g_idx)
        note_tag, midi = note_items[g_idx % len(note_items)]
        events, bpm = build_rage_groove(bank, rng, midi)
        rel = Path("12_MIDI_GROOVES") / bank / f"DL_RAGE_{bank}_Groove_{int(bpm)}BPM_{note_tag}_{g_idx + 1:03d}.mid"
        write_midi_file(out_dir / rel, events, bpm)
        midi_paths.append(rel.as_posix())

    for folder in ("13_DRUM_LOOPS", "14_LOOP_STEMS", "15_JUCE_SYNTH_PRESETS", "16_DRUM_KIT_MAPPINGS"):
        (out_dir / folder).mkdir(parents=True, exist_ok=True)

    preset_doc = {
        "schema": "disklordz-rage-apocalypse-presets/v1",
        "banks": BANKS,
        "bass_reactor_designs": list(BASS_DESIGNS),
    }
    (out_dir / "15_JUCE_SYNTH_PRESETS" / "RageBankRecipes.json").write_text(json.dumps(preset_doc, indent=2), encoding="utf-8")

    mapping_doc = {
        "bank_a_gm_notes": {str(36 + i): name for i, name in enumerate(
            ("ApocalypseKick", "DigitalRim", "ChromeSnare", "ChaosClap", "Redline808_Low", "HyperHat_Closed",
             "Chaos808_Mid", "HyperHat_Accent", "AlienSlide808", "OpenHat", "IndustrialSnare", "AlienLaser",
             "PlasmaBell", "BitPulse", "SubImpact", "RageRiser")
        )}
    }
    (out_dir / "16_DRUM_KIT_MAPPINGS" / "PadBankA_16.json").write_text(json.dumps(mapping_doc, indent=2), encoding="utf-8")

    summary = {
        "engine": ENGINE,
        "tier": tier,
        "sample_rate": sr,
        "bit_depth": bits,
        "seed": seed,
        "one_shots": len(manifest_rows),
        "midi_grooves": len(midi_paths),
        "samples": manifest_rows,
        "midi": midi_paths,
    }
    (out_dir / "MANIFEST.json").write_text(json.dumps(summary, indent=2), encoding="utf-8")
    (out_dir / "README.md").write_text(
        "# DISKLORDZ RAGE APOCALYPSE\n\n"
        "100% procedurally synthesized Rage, Opium, Hypertrap & Industrial Trap samples.\n"
        "808s are synthesized directly at verified musical root pitches with split-band clean sub preservation.\n",
        encoding="utf-8",
    )
    return summary


def main() -> None:
    ap = argparse.ArgumentParser(description=ENGINE)
    ap.add_argument("--tier", choices=("demo", "starter", "standard", "mega"), default="demo")
    ap.add_argument("--sr", type=int, choices=(44100, 48000, 96000), default=48000)
    ap.add_argument("--bits", type=int, choices=(16, 24), default=24)
    ap.add_argument("--seed", type=int, default=666808)
    ap.add_argument("--ceiling", type=float, default=-1.0)
    ap.add_argument("--out", default="DISKLORDZ_RAGE_APOCALYPSE_VOL_1")
    args = ap.parse_args()
    summary = build_apocalypse_pack(Path(args.out), args.sr, args.bits, args.seed, args.tier, args.ceiling)
    print(f"Finished {summary['one_shots']} one-shots and {summary['midi_grooves']} MIDI grooves in {args.out}")


if __name__ == "__main__":
    main()
