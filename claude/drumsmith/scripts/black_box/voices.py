"""Diverse Black Box voice recipes — multiple architectures per role.

Used when profile voice.diversify is true so kits are not one generator + knob walks.
"""

from __future__ import annotations

import math
from pathlib import Path

import numpy as np

from scripts.memphis_oneshots.phonk_hat import generate_memphis_phonk_hat
from scripts.memphis_oneshots.phonk_kick import generate_memphis_phonk_kick
from scripts.memphis_oneshots.phonk_snare import generate_memphis_phonk_snare


def _norm(x: np.ndarray, ceiling_db: float = -0.3) -> np.ndarray:
    peak = float(np.max(np.abs(x))) or 1.0
    return ((x / peak) * (10.0 ** (ceiling_db / 20.0))).astype(np.float32)


def render_808(i: int, *, freq: float, eng: dict, voice: dict, seed: int) -> tuple[np.ndarray, str]:
    """Return (buffer, tag). Recipes: Thick / Punch / Crawl / Dirty."""
    recipes = ("Thick", "Punch", "Crawl", "Dirty")
    tag = recipes[i % len(recipes)]
    base_dur = float(eng.get("808_duration", eng.get("kick_duration", 1.2)))
    base_decay = float(eng.get("amp_decay_808", eng.get("amp_decay_kick", 0.55)))
    pitch_st = float(eng.get("kick_pitch_st", 48))
    pitch_decay = float(voice.get("kick_pitch_decay", 0.018))
    clip = float(eng.get("kick_clip_db", 5.0))
    lofi = float(eng.get("lofi_sr", 26040))
    bits = int(eng.get("bit_depth", 12))
    tape = float(eng.get("tape_lpf_hz", 11800))
    click = float(voice.get("808_click", 0.4))

    if tag == "Thick":
        dur, decay, click_amt, drive = base_dur, base_decay, click, clip + 1.5
        pitch_decay = min(pitch_decay, 0.016)
    elif tag == "Punch":
        dur, decay, click_amt, drive = base_dur * 0.55, base_decay * 0.45, min(1.0, click + 0.25), clip + 2.5
        pitch_st = pitch_st + 6
        pitch_decay = 0.012
    elif tag == "Crawl":
        dur, decay, click_amt, drive = base_dur * 1.45, base_decay * 1.35, click * 0.55, clip
        pitch_decay = 0.028
        tape = min(tape, 9000)
    else:  # Dirty
        dur, decay, click_amt, drive = base_dur * 0.9, base_decay * 0.85, click + 0.1, clip + 3.0
        lofi, bits = min(lofi, 18000), min(bits, 10)

    buf = generate_memphis_phonk_kick(
        root_freq=freq,
        duration=dur,
        pitch_mod_depth=pitch_st,
        pitch_decay=pitch_decay,
        amp_decay=decay,
        click_amount=click_amt,
        clip_drive_db=drive,
        lofi_sr=lofi,
        bit_depth=bits,
        tape_lpf_hz=tape,
        seed=seed + i,
    )
    return buf, tag


def render_kick(i: int, *, freq: float, eng: dict, voice: dict, seed: int) -> tuple[np.ndarray, str]:
    recipes = ("Punch", "Knock", "Thump")
    tag = recipes[i % len(recipes)]
    pitch = float(eng.get("kick_pitch_st", 42))
    clip = float(eng.get("kick_clip_db", 4.5))
    if tag == "Knock":
        dur, decay, click, pitch = 0.18, 0.07, 0.55, min(pitch, 28)
    elif tag == "Thump":
        dur, decay, click, pitch = 0.32, 0.16, 0.25, pitch + 4
    else:
        dur, decay, click = 0.22, 0.10, 0.4
    buf = generate_memphis_phonk_kick(
        root_freq=freq,
        duration=dur,
        pitch_mod_depth=pitch + (i % 6),
        pitch_decay=float(voice.get("kick_pitch_decay", 0.02)),
        amp_decay=decay,
        click_amount=click,
        clip_drive_db=clip + (i % 4) * 0.3,
        lofi_sr=float(eng.get("lofi_sr", 26040)),
        bit_depth=int(eng.get("bit_depth", 12)),
        tape_lpf_hz=float(eng.get("tape_lpf_hz", 12000)),
        seed=seed + i,
    )
    return buf, tag


def render_snare(i: int, *, membrane: float, eng: dict, voice: dict, seed: int) -> tuple[np.ndarray, str]:
    recipes = ("Aggro", "Paper", "Crack", "Flam")
    tag = recipes[i % len(recipes)]
    base = int(voice.get("snare_pitch_base", 22))
    mod = max(1, int(voice.get("snare_pitch_mod", 10)))
    pitch = base + (i % mod)
    flam = float(voice.get("snare_flam_base_ms", 4.0)) + (i % 5) * float(voice.get("snare_flam_step_ms", 0.5))
    clip = float(eng.get("snare_clip_db", 5.5))
    kwargs: dict = {
        "membrane_root": membrane,
        "pitch_mod_st": pitch,
        "flam_delay_ms": flam,
        "clip_drive_db": clip + (i % 4) * 0.25,
        "seed": seed + i,
    }
    if tag == "Paper":
        kwargs.update(pitch_mod_st=16 + (i % 5), flam_delay_ms=6.0, noise_decay=0.28, clip_drive_db=clip - 1.5)
    elif tag == "Crack":
        kwargs.update(pitch_mod_st=28 + (i % 6), flam_delay_ms=2.2, noise_decay=0.09, clip_drive_db=clip + 2.0)
    elif tag == "Flam":
        kwargs.update(flam_delay_ms=8.0 + (i % 4), noise_decay=0.16, clip_drive_db=clip + 0.5)
    # Aggro = defaults with base pitch walk
    return generate_memphis_phonk_snare(**kwargs), tag


def render_clap(i: int, *, layered_default: bool, bright_default: bool, seed: int) -> tuple[np.ndarray, str]:
    recipes = ("Dry", "Layer", "Gate")
    tag = recipes[i % len(recipes)]
    rng = np.random.default_rng(seed + i)
    sr = 44100
    if tag == "Layer":
        n = int(0.16 * sr)
        layered, bright = True, False
    elif tag == "Gate":
        n = int(0.07 * sr)
        layered, bright = False, True
    else:
        n = int(0.11 * sr)
        layered, bright = layered_default, bright_default
    t = np.linspace(0.0, n / sr, n, endpoint=False)
    noise = rng.uniform(-1, 1, n)
    amp = np.zeros_like(t)
    hits = ((0.0, 0.45), (0.011, 0.75), (0.024, 1.0))
    if layered:
        hits = ((0.0, 0.35), (0.008, 0.55), (0.016, 0.8), (0.028, 1.0), (0.04, 0.6))
    for off, g in hits:
        idx = int(off * sr)
        if idx < n:
            amp[idx:] += g * np.exp(-(t[idx:] - t[idx]) / 0.028)
    y = np.diff(noise * amp, prepend=0.0)
    if bright:
        y = np.diff(y, prepend=0.0)
    if tag == "Gate":
        y = y * (t < 0.045)
    return _norm(y), tag


def render_hat(
    i: int,
    *,
    open_hat: bool,
    eng: dict,
    voice: dict,
    seed: int,
) -> tuple[np.ndarray, str]:
    recipes = ("Crunch", "Dust", "Thin", "Metal")
    tag = recipes[i % len(recipes)]
    center = float(eng.get("hat_center_hz", 9000)) + (i % 10) * 180 - 400
    bits = int(eng.get("bit_depth", 12))
    lofi = float(eng.get("lofi_sr", 26040)) if voice.get("hat_use_lofi", True) else 44100.0
    if open_hat:
        dur = float(voice.get("oh_duration", 0.2))
        decay = float(voice.get("oh_noise_decay", 0.07))
        if tag == "Dust":
            dur, decay, lofi = dur * 1.3, decay * 1.4, min(lofi, 18000)
        elif tag == "Thin":
            dur, decay, center = dur * 0.7, decay * 0.6, center + 1200
        elif tag == "Metal":
            center, bits = center + 800, min(bits, 10)
        buf = generate_memphis_phonk_hat(
            duration=dur,
            center_hz=center,
            noise_decay=decay,
            seed=seed + i,
            clip_drive_db=2.5 + (i % 3) * 0.4,
            bit_depth=bits,
            lofi_sr=lofi,
        )
    else:
        decay = float(voice.get("hat_noise_decay", 0.022))
        clip = float(voice.get("hat_clip_db", 3.0))
        dur = 0.07
        if tag == "Dust":
            decay, lofi, clip = 0.04, min(lofi, 16000), clip - 0.5
        elif tag == "Thin":
            decay, center, dur = 0.014, center + 1500, 0.05
        elif tag == "Metal":
            center, bits, clip = center + 1000, 9, clip + 1.5
        buf = generate_memphis_phonk_hat(
            duration=dur,
            center_hz=center,
            seed=seed + i,
            noise_decay=decay + (i % 4) * 0.002,
            clip_drive_db=clip,
            bit_depth=bits,
            lofi_sr=lofi,
        )
    return buf, tag


def render_cow(i: int, eng: dict, voice: dict) -> tuple[np.ndarray, str]:
    recipes = ("Metal", "Mute", "Clang")
    tag = recipes[i % len(recipes)]
    pitch = 0.75 + (i % 14) * 0.04
    if voice.get("cow_metallic_fixed") is not None:
        metallic = float(voice["cow_metallic_fixed"])
    else:
        metallic = 1.0 + (i % 4) * 0.08
    mute = float(eng.get("cow_mute", 1.0))
    dur = 0.22
    if tag == "Mute":
        mute, dur, metallic = 0.55, 0.14, metallic * 0.85
    elif tag == "Clang":
        metallic, dur = metallic * 1.25, 0.32
    sr = 44100
    n = int(round(sr * dur))
    t = np.linspace(0.0, dur, n, endpoint=False)
    f1 = float(eng.get("cow_f1", 540)) * pitch * metallic
    f2 = float(eng.get("cow_f2", 800)) * pitch * metallic
    sq = 0.5 * np.sign(np.sin(2 * np.pi * f1 * t)) + 0.5 * np.sign(np.sin(2 * np.pi * f2 * t))
    ring = 0.15 * mute * np.sin(2 * np.pi * f1 * 2.7 * t) * np.exp(-t / 0.04)
    decay = 0.14 if mute < 0.85 else 0.09
    env = np.exp(-t / decay) * mute
    return _norm(sq * env + ring), tag


def render_tom(i: int) -> tuple[np.ndarray, str]:
    recipes = ("Low", "Mid", "High", "Floor")
    tag = recipes[i % len(recipes)]
    roots = {"Low": 78, "Mid": 110, "High": 155, "Floor": 62}
    root = roots[tag] * (1.0 + (i % 3) * 0.03)
    sr, dur = 44100, 0.22 if tag == "High" else 0.34
    n = int(round(sr * dur))
    t = np.linspace(0.0, dur, n, endpoint=False)
    pitch_env = np.exp(-t / 0.035)
    freq = root * (2.0 ** (18.0 * pitch_env / 12.0))
    phase = 2.0 * math.pi * np.cumsum(freq) / sr
    osc = 0.7 * np.sin(phase) + 0.3 * (2 / math.pi) * np.arcsin(np.sin(phase))
    decay = 0.08 if tag == "High" else 0.14
    return _norm(osc * np.exp(-t / decay)), tag


def render_perc(i: int, *, seed: int, voice: dict) -> tuple[np.ndarray, str]:
    recipes = ("Rim", "Clave", "Scrap", "Tick")
    tag = recipes[i % len(recipes)]
    rng = np.random.default_rng(seed + i)
    sr = 44100
    if tag == "Clave":
        dur = 0.09
        n = int(dur * sr)
        t = np.linspace(0.0, dur, n, endpoint=False)
        tone = np.sin(2 * math.pi * (1400 + (i % 5) * 70) * t) * np.exp(-t / 0.025)
        click = rng.uniform(-1, 1, n) * np.exp(-t / 0.004) * 0.35
        return _norm(tone + click), tag
    if tag == "Scrap":
        dur = 0.11
        n = int(dur * sr)
        t = np.linspace(0.0, dur, n, endpoint=False)
        noise = rng.uniform(-1, 1, n)
        noise = np.round(noise * 48) / 48
        return _norm(noise * np.exp(-t / 0.035)), tag
    if tag == "Tick":
        dur = 0.045
        n = int(dur * sr)
        t = np.linspace(0.0, dur, n, endpoint=False)
        return _norm(rng.uniform(-1, 1, n) * np.exp(-t / 0.006)), tag
    # Rim
    dur = 0.06
    n = int(dur * sr)
    t = np.linspace(0.0, dur, n, endpoint=False)
    click = rng.uniform(-1, 1, n) * np.exp(-t / 0.005)
    tone = np.sin(2 * math.pi * 1800 * t) * np.exp(-t / 0.015) * 0.55
    return _norm(click + tone), tag


def discover_loop_blueprints(root: Path) -> list[tuple[Path, int]]:
    """All converter-night loop JSONs with bpm + lanes (skip meta/MIDI sidecars)."""
    import json

    loop_dir = root / "kits/converter-night/loops"
    found: list[tuple[Path, int]] = []
    if not loop_dir.is_dir():
        return found
    for path in sorted(loop_dir.glob("*.json")):
        if path.name.startswith("render-") or path.name.endswith("-meta.json"):
            continue
        try:
            bp = json.loads(path.read_text(encoding="utf-8"))
        except Exception:
            continue
        if "bpm" not in bp or "lanes" not in bp or "bars" not in bp:
            continue
        bpm = int(round(float(bp["bpm"])))
        found.append((path, bpm))
    return found
