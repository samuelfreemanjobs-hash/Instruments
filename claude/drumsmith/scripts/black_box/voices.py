"""Diverse Black Box voice recipes — era-specific architectures per role.

Used when profile voice.diversify is true so kits are not one generator + knob walks.
recipe_set picks era personality: signature | hollow | trunk | digital.
"""

from __future__ import annotations

import json
import math
from pathlib import Path

import numpy as np

from scripts.black_box import layered as L
from scripts.memphis_oneshots.phonk_hat import generate_memphis_phonk_hat
from scripts.memphis_oneshots.phonk_kick import generate_memphis_phonk_kick
from scripts.memphis_oneshots.phonk_snare import generate_memphis_phonk_snare

RECIPE_SETS: dict[str, dict[str, tuple[str, ...]]] = {
    "signature": {
        "808": ("Thick", "Punch", "Crawl", "Dirty"),
        "KICK": ("Punch", "Knock", "Thump"),
        "SNR": ("Aggro", "Paper", "Crack", "Flam"),
        "CLAP": ("Dry", "Layer", "Gate"),
        "CH": ("Crunch", "Dust", "Thin", "Metal"),
        "OH": ("Crunch", "Dust", "Thin", "Metal"),
        "COW": ("Metal", "Mute", "Clang"),
        "TOM": ("Low", "Mid", "High", "Floor"),
        "PERC": ("Rim", "Clave", "Scrap", "Tick"),
    },
    "hollow": {
        "808": ("Hollow", "Haunt", "Fog", "Mud"),
        "KICK": ("Knock", "Thump", "Ghost"),
        "SNR": ("Paper", "Whisper", "Dust", "Rattle"),
        "CLAP": ("Dry", "Far", "Gate"),
        "CH": ("Tape", "Dust", "Air"),
        "OH": ("Tape", "Fog", "Air"),
        "COW": ("Eerie", "Mute", "Glass"),
        "TOM": ("Low", "Mid", "Floor"),
        "PERC": ("Scrap", "Tick", "Bone", "Rim"),
    },
    "trunk": {
        "808": ("Boom", "Trunk", "Crawl", "Dirty"),
        "KICK": ("Heavy", "Punch", "Thump"),
        "SNR": ("Crack", "Aggro", "Slap", "Flam"),
        "CLAP": ("Layer", "Gate", "Stack"),
        "CH": ("Tight", "Metal", "Crunch"),
        "OH": ("Tight", "Metal", "Crunch"),
        "COW": ("Metal", "Clang", "Mute"),
        "TOM": ("Low", "Floor", "Mid"),
        "PERC": ("Rim", "Hard", "Clave", "Tick"),
    },
    "digital": {
        "808": ("Tuned", "Clean", "Snap", "Wide"),
        "KICK": ("Digital", "Punch", "Click"),
        "SNR": ("Snappy", "Crack", "Thin", "Bright"),
        "CLAP": ("Bright", "Dry", "Gate"),
        "CH": ("Tight", "Bright", "Thin", "Metal"),
        "OH": ("Tight", "Bright", "Thin"),
        "COW": ("Metal", "Clang", "Clean"),
        "TOM": ("High", "Mid", "Low"),
        "PERC": ("Synth", "Tick", "Clave", "Rim"),
    },
}


def _norm(x: np.ndarray, ceiling_db: float = -0.3) -> np.ndarray:
    peak = float(np.max(np.abs(x))) or 1.0
    return ((x / peak) * (10.0 ** (ceiling_db / 20.0))).astype(np.float32)


def recipe_set_name(voice: dict) -> str:
    name = str(voice.get("recipe_set") or "signature")
    return name if name in RECIPE_SETS else "signature"


def recipes_for(kind: str, voice: dict) -> tuple[str, ...]:
    table = RECIPE_SETS[recipe_set_name(voice)]
    return table.get(kind) or RECIPE_SETS["signature"].get(kind, ("A",))


def _use_layered(voice: dict) -> bool:
    return bool(voice.get("layered", True))


def render_808(i: int, *, freq: float, eng: dict, voice: dict, seed: int) -> tuple[np.ndarray, str]:
    recipes = recipes_for("808", voice)
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
    mid_bark, sub_w = 0.22, 0.85

    # Shared recipe families + era aliases
    if tag in ("Thick", "Boom", "Tuned", "Clean"):
        dur, decay, click_amt, drive = base_dur, base_decay, click, clip + 1.5
        pitch_decay = min(pitch_decay, 0.016)
        if tag == "Clean":
            bits, lofi, mid_bark = max(bits, 14), 44100.0, 0.12
        if tag == "Boom":
            dur, decay, sub_w = base_dur * 1.15, base_decay * 1.2, 0.95
    elif tag in ("Punch", "Snap", "Heavy"):
        dur, decay, click_amt, drive = base_dur * 0.55, base_decay * 0.45, min(1.0, click + 0.25), clip + 2.5
        pitch_st, pitch_decay, mid_bark = pitch_st + 6, 0.012, 0.32
    elif tag in ("Crawl", "Haunt", "Fog", "Wide"):
        dur, decay, click_amt, drive = base_dur * 1.45, base_decay * 1.35, click * 0.55, clip
        pitch_decay, tape = 0.028, min(tape, 9000)
        if tag in ("Haunt", "Fog"):
            drive, click_amt, mid_bark = clip - 0.5, click * 0.35, 0.1
    elif tag in ("Dirty", "Mud", "Trunk"):
        dur, decay, click_amt, drive = base_dur * 0.9, base_decay * 0.85, click + 0.1, clip + 3.0
        lofi, bits, mid_bark = min(lofi, 18000), min(bits, 10), 0.28
        if tag == "Trunk":
            sub_w, dur = 1.0, base_dur * 1.1
    elif tag == "Hollow":
        dur, decay, click_amt, drive = base_dur * 1.2, base_decay * 1.1, click * 0.4, clip - 0.5
        tape, mid_bark = min(tape, 8500), 0.08
    else:
        dur, decay, click_amt, drive = base_dur, base_decay, click, clip

    kwargs = dict(
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
    if _use_layered(voice):
        buf = L.layered_808(**kwargs, mid_bark=mid_bark, sub_weight=sub_w)
    else:
        buf = generate_memphis_phonk_kick(**kwargs)
    return buf, tag


def render_kick(i: int, *, freq: float, eng: dict, voice: dict, seed: int) -> tuple[np.ndarray, str]:
    recipes = recipes_for("KICK", voice)
    tag = recipes[i % len(recipes)]
    pitch = float(eng.get("kick_pitch_st", 42))
    clip = float(eng.get("kick_clip_db", 4.5))
    if tag in ("Knock", "Ghost", "Click"):
        dur, decay, click, pitch = 0.18, 0.07, 0.55, min(pitch, 28)
    elif tag in ("Thump", "Heavy"):
        dur, decay, click, pitch = 0.32, 0.16, 0.25, pitch + 4
    elif tag == "Digital":
        dur, decay, click, pitch = 0.2, 0.1, 0.45, min(pitch, 30)
    else:
        dur, decay, click = 0.22, 0.10, 0.4
    kwargs = dict(
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
    if _use_layered(voice):
        buf = L.layered_808(**kwargs, mid_bark=0.18, sub_weight=0.7)
    else:
        buf = generate_memphis_phonk_kick(**kwargs)
    return buf, tag


def render_snare(i: int, *, membrane: float, eng: dict, voice: dict, seed: int) -> tuple[np.ndarray, str]:
    recipes = recipes_for("SNR", voice)
    tag = recipes[i % len(recipes)]
    base = int(voice.get("snare_pitch_base", 22))
    mod = max(1, int(voice.get("snare_pitch_mod", 10)))
    pitch = base + (i % mod)
    flam = float(voice.get("snare_flam_base_ms", 4.0)) + (i % 5) * float(voice.get("snare_flam_step_ms", 0.5))
    clip = float(eng.get("snare_clip_db", 5.5))
    mem_g, wire_g, body_g, noise = 0.75, 0.55, 0.35, 0.19

    if tag in ("Paper", "Whisper", "Dust", "Thin"):
        pitch, flam, noise, mem_g, wire_g = 16 + (i % 5), 6.0, 0.28, 0.45, 0.75
        clip = clip - 1.5
    elif tag in ("Crack", "Slap", "Snappy", "Bright"):
        pitch, flam, noise, body_g = 28 + (i % 6), 2.2, 0.09, 0.55
        clip = clip + 2.0
    elif tag in ("Flam", "Rattle", "Stack"):
        flam, noise = 8.0 + (i % 4), 0.16
    elif tag in ("Aggro", "Hard"):
        clip = clip + 1.0
        wire_g, body_g = 0.65, 0.4

    if _use_layered(voice):
        buf = L.layered_snare(
            membrane_root=membrane,
            pitch_mod_st=pitch,
            flam_delay_ms=flam,
            clip_drive_db=clip + (i % 4) * 0.25,
            seed=seed + i,
            membrane_gain=mem_g,
            wires_gain=wire_g,
            body_gain=body_g,
            noise_decay=noise,
            lofi_sr=float(eng.get("lofi_sr", 26040)),
            bit_depth=int(eng.get("bit_depth", 12)),
        )
    else:
        buf = generate_memphis_phonk_snare(
            membrane_root=membrane,
            pitch_mod_st=pitch,
            flam_delay_ms=flam,
            noise_decay=noise,
            clip_drive_db=clip + (i % 4) * 0.25,
            seed=seed + i,
        )
    return buf, tag


def render_clap(i: int, *, layered_default: bool, bright_default: bool, seed: int, voice: dict | None = None) -> tuple[np.ndarray, str]:
    recipes = recipes_for("CLAP", voice or {})
    tag = recipes[i % len(recipes)]
    rng = np.random.default_rng(seed + i)
    sr = 44100
    if tag in ("Layer", "Stack", "Far"):
        n, layered, bright = int(0.16 * sr), True, False
    elif tag == "Gate":
        n, layered, bright = int(0.07 * sr), False, True
    elif tag == "Bright":
        n, layered, bright = int(0.11 * sr), layered_default, True
    else:
        n, layered, bright = int(0.11 * sr), layered_default, bright_default
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


def render_hat(i: int, *, open_hat: bool, eng: dict, voice: dict, seed: int) -> tuple[np.ndarray, str]:
    kind = "OH" if open_hat else "CH"
    recipes = recipes_for(kind, voice)
    tag = recipes[i % len(recipes)]
    center = float(eng.get("hat_center_hz", 9000)) + (i % 10) * 180 - 400
    bits = int(eng.get("bit_depth", 12))
    lofi = float(eng.get("lofi_sr", 26040)) if voice.get("hat_use_lofi", True) else 44100.0
    metal = 1.0
    if open_hat:
        dur = float(voice.get("oh_duration", 0.2))
        decay = float(voice.get("oh_noise_decay", 0.07))
        if tag in ("Dust", "Fog", "Air"):
            dur, decay, lofi = dur * 1.3, decay * 1.4, min(lofi, 18000)
        elif tag in ("Thin", "Bright"):
            dur, decay, center = dur * 0.7, decay * 0.6, center + 1200
        elif tag in ("Metal", "Glass"):
            center, bits, metal = center + 800, min(bits, 10), 1.25
    else:
        decay = float(voice.get("hat_noise_decay", 0.022))
        clip = float(voice.get("hat_clip_db", 3.0))
        dur = 0.07
        if tag in ("Dust", "Tape", "Air"):
            decay, lofi, clip = 0.04, min(lofi, 16000), clip - 0.5
        elif tag in ("Thin", "Bright"):
            decay, center, dur = 0.014, center + 1500, 0.05
        elif tag in ("Metal", "Glass"):
            center, bits, metal = center + 1000, 9, 1.35
        else:
            clip = float(voice.get("hat_clip_db", 3.0))
        kwargs_clip = clip if not open_hat else 2.5 + (i % 3) * 0.4

    if open_hat:
        kwargs_clip = 2.5 + (i % 3) * 0.4
    else:
        kwargs_clip = float(voice.get("hat_clip_db", 3.0))
        if tag in ("Dust", "Tape", "Air"):
            kwargs_clip -= 0.5
        elif tag in ("Metal", "Glass"):
            kwargs_clip += 1.5

    if _use_layered(voice):
        buf = L.layered_hat(
            open_hat=open_hat,
            center_hz=center,
            noise_decay=decay if open_hat else decay + (i % 4) * 0.002,
            duration=dur if open_hat else dur,
            clip_drive_db=kwargs_clip,
            bit_depth=bits,
            lofi_sr=lofi,
            seed=seed + i,
            metal=metal,
        )
    else:
        buf = generate_memphis_phonk_hat(
            duration=dur if open_hat else dur,
            center_hz=center,
            seed=seed + i,
            noise_decay=(decay if open_hat else decay + (i % 4) * 0.002),
            clip_drive_db=kwargs_clip,
            bit_depth=bits,
            lofi_sr=lofi,
        )
    return buf, tag


def render_cow(i: int, eng: dict, voice: dict) -> tuple[np.ndarray, str]:
    recipes = recipes_for("COW", voice)
    tag = recipes[i % len(recipes)]
    pitch = 0.75 + (i % 14) * 0.04
    if voice.get("cow_metallic_fixed") is not None:
        metallic = float(voice["cow_metallic_fixed"])
    else:
        metallic = 1.0 + (i % 4) * 0.08
    mute = float(eng.get("cow_mute", 1.0))
    dur = 0.22
    if tag in ("Mute", "Eerie"):
        mute, dur, metallic = 0.55, 0.14, metallic * 0.85
    elif tag in ("Clang", "Glass"):
        metallic, dur = metallic * 1.25, 0.32
    elif tag == "Clean":
        mute = 1.0
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


def render_tom(i: int, voice: dict | None = None) -> tuple[np.ndarray, str]:
    recipes = recipes_for("TOM", voice or {})
    tag = recipes[i % len(recipes)]
    roots = {"Low": 78, "Mid": 110, "High": 155, "Floor": 62}
    root = roots.get(tag, 100) * (1.0 + (i % 3) * 0.03)
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
    recipes = recipes_for("PERC", voice)
    tag = recipes[i % len(recipes)]
    rng = np.random.default_rng(seed + i)
    sr = 44100
    if tag in ("Clave", "Synth"):
        dur = 0.09
        n = int(dur * sr)
        t = np.linspace(0.0, dur, n, endpoint=False)
        tone = np.sin(2 * math.pi * (1400 + (i % 5) * 70) * t) * np.exp(-t / 0.025)
        click = rng.uniform(-1, 1, n) * np.exp(-t / 0.004) * 0.35
        return _norm(tone + click), tag
    if tag in ("Scrap", "Bone", "Hard"):
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
    dur = 0.06
    n = int(dur * sr)
    t = np.linspace(0.0, dur, n, endpoint=False)
    click = rng.uniform(-1, 1, n) * np.exp(-t / 0.005)
    tone = np.sin(2 * math.pi * 1800 * t) * np.exp(-t / 0.015) * 0.55
    return _norm(click + tone), tag


def discover_loop_blueprints(root: Path) -> list[tuple[Path, int]]:
    """All converter-night loop JSONs with bpm + lanes (skip meta/MIDI sidecars)."""
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
