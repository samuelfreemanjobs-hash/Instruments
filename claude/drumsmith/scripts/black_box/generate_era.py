#!/usr/bin/env python3
"""Shared BLACK BOX era generator — profile-driven oneshots + loops."""

from __future__ import annotations

import argparse
import json
import math
import shutil
import sys
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[2]
for p in (str(ROOT), str(ROOT / "scripts")):
    if p not in sys.path:
        sys.path.insert(0, p)

from dsp_math import loop_samples  # noqa: E402
from scripts.black_box.seeds import sku_seed  # noqa: E402
from scripts.memphis_oneshots.phonk_hat import generate_memphis_phonk_hat  # noqa: E402
from scripts.memphis_oneshots.phonk_kick import (  # noqa: E402
    generate_memphis_phonk_kick,
    write_wav_16bit_mono,
)
from scripts.memphis_oneshots.phonk_snare import generate_memphis_phonk_snare  # noqa: E402
from scripts.memphis_oneshots.render_memphis_loop import render_memphis_loop  # noqa: E402

PROFILES = ROOT / "kits/black-box-collection/profiles"
KICK_KEYS = [32.70, 36.71, 41.20, 43.65, 46.25, 49.00, 55.00, 38.89]
SNR_HZ = [180, 196, 208, 220, 233, 247, 262, 185, 200, 215]

BLUEPRINTS = [
    (ROOT / "kits/converter-night/loops/cnv-dr660-94-a.json", 94),
    (ROOT / "kits/converter-night/loops/cnv-arch-horror.json", 94),
    (ROOT / "kits/converter-night/loops/cnv-arch-bitcrush.json", 94),
    (ROOT / "kits/converter-night/loops/cnv-arch-bounce.json", 134),
    (ROOT / "kits/converter-night/loops/cnv-dr660-134-crawl.json", 134),
    (ROOT / "kits/converter-night/loops/cnv-rhythm-94.json", 94),
]


def _norm(x: np.ndarray, ceiling_db: float = -0.1) -> np.ndarray:
    peak = float(np.max(np.abs(x))) or 1.0
    return ((x / peak) * (10.0 ** (ceiling_db / 20.0))).astype(np.float32)


def _bias_hz(hz: float, st: float) -> float:
    return hz * (2.0 ** (st / 12.0))


def _cowbell(pitch: float, eng: dict, duration: float = 0.22, metallic: float = 1.0) -> np.ndarray:
    sr = 44100
    n = int(round(sr * duration))
    t = np.linspace(0.0, duration, n, endpoint=False)
    f1 = float(eng.get("cow_f1", 540)) * pitch * metallic
    f2 = float(eng.get("cow_f2", 800)) * pitch * metallic
    mute = float(eng.get("cow_mute", 1.0))
    sq = 0.5 * np.sign(np.sin(2 * np.pi * f1 * t)) + 0.5 * np.sign(np.sin(2 * np.pi * f2 * t))
    ring = 0.15 * mute * np.sin(2 * np.pi * f1 * 2.7 * t) * np.exp(-t / 0.04)
    decay = 0.12 if mute < 0.85 else 0.09
    env = np.exp(-t / decay) * mute
    return _norm(sq * env + ring)


def _clap(seed: int, layered: bool = False, bright: bool = False) -> np.ndarray:
    rng = np.random.default_rng(seed)
    sr = 44100
    n = int(0.14 * sr) if layered else int(0.11 * sr)
    t = np.linspace(0.0, n / sr, n, endpoint=False)
    noise = rng.uniform(-1, 1, n)
    amp = np.zeros_like(t)
    hits = ((0.0, 0.45), (0.011, 0.75), (0.024, 1.0))
    if layered:
        hits = ((0.0, 0.35), (0.008, 0.55), (0.016, 0.8), (0.028, 1.0), (0.04, 0.6))
    for off, g in hits:
        i = int(off * sr)
        if i < n:
            amp[i:] += g * np.exp(-(t[i:] - t[i]) / 0.028)
    y = np.diff(noise * amp, prepend=0.0)
    if bright:
        y = np.diff(y, prepend=0.0)
    return _norm(y)


def _tom(root: float) -> np.ndarray:
    sr = 44100
    dur = 0.28
    n = int(round(sr * dur))
    t = np.linspace(0.0, dur, n, endpoint=False)
    pitch_env = np.exp(-t / 0.035)
    freq = root * (2.0 ** (18.0 * pitch_env / 12.0))
    phase = 2.0 * math.pi * np.cumsum(freq) / sr
    osc = 0.7 * np.sin(phase) + 0.3 * (2 / math.pi) * np.arcsin(np.sin(phase))
    return _norm(osc * np.exp(-t / 0.12))


def _perc(seed: int, grimy: bool = False, synth: bool = False, rim: bool = False) -> np.ndarray:
    rng = np.random.default_rng(seed)
    sr = 44100
    dur = 0.08 if grimy else 0.06
    n = int(dur * sr)
    t = np.linspace(0.0, dur, n, endpoint=False)
    if rim:
        click = rng.uniform(-1, 1, n) * np.exp(-t / 0.005)
        tone = np.sin(2 * math.pi * 1800 * t) * np.exp(-t / 0.015) * 0.55
        return _norm(click + tone)
    if synth:
        tone = np.sin(2 * math.pi * (900 + (seed % 7) * 80) * t) * np.exp(-t / 0.03)
        click = rng.uniform(-1, 1, n) * np.exp(-t / 0.01) * 0.25
        return _norm(tone + click)
    click = rng.uniform(-1, 1, n) * np.exp(-t / 0.008)
    tone = np.sin(2 * math.pi * (1100 + (seed % 5) * 90) * t) * np.exp(-t / 0.02) * 0.4
    if grimy:
        click = np.round(click * 64) / 64
    return _norm(click + tone)


def load_profile(sku_or_path: str | Path) -> dict:
    path = Path(sku_or_path)
    if not path.exists():
        key = str(sku_or_path).lower().replace("black-box-", "bb").replace(" ", "")
        aliases = {
            "bb93": "bb93.json",
            "93": "bb93.json",
            "bb96": "bb96.json",
            "96": "bb96.json",
            "bb98": "bb98.json",
            "98": "bb98.json",
            "bb00": "bb2000.json",
            "bb2000": "bb2000.json",
            "2000": "bb2000.json",
            "bbdc": "dirty_circuits.json",
            "dirty": "dirty_circuits.json",
            "dirty_circuits": "dirty_circuits.json",
        }
        path = PROFILES / aliases.get(key, f"{key}.json")
    return json.loads(path.read_text(encoding="utf-8"))


def kit_masters_dir(profile: dict) -> Path:
    sku = profile["sku"].lower()
    folder = {
        "bb93": "black-box-93",
        "bb96": "black-box-96",
        "bb98": "black-box-98",
        "bb00": "black-box-2000",
    }.get(sku, f"black-box-{sku}")
    return ROOT / "kits" / folder / "masters"


def _voice(profile: dict) -> dict:
    """Era personality lives in profile['voice'] — no char== branches in the loop."""
    defaults = {
        "kick_pitch_decay": 0.022,
        "kick_duration_min": None,
        "kick_extra_bias_st": 0.0,
        "kick_pitch_cap": None,
        "808_tuning": "key",
        "snare_pitch_base": 22,
        "snare_pitch_mod": 10,
        "snare_flam_base_ms": 4.0,
        "snare_flam_step_ms": 0.5,
        "clap_layered": False,
        "clap_bright": False,
        "hat_noise_decay": 0.022,
        "hat_clip_db": 3.0,
        "hat_use_lofi": True,
        "oh_duration": 0.2,
        "oh_noise_decay": 0.07,
        "cow_metallic_fixed": None,
        "perc_grimy": False,
        "perc_synth": False,
        "perc_rim_every": 0,
        "loop_bpm_alt": None,
    }
    merged = dict(defaults)
    merged.update(profile.get("voice") or {})
    return merged


def generate_all(out_dir: Path, profile: dict) -> list[dict]:
    if out_dir.exists():
        shutil.rmtree(out_dir)
    inv = profile["inventory"]
    eng = profile["engine"]
    folders = profile["folder_map"]
    tags = profile.get("name_tags", {})
    prefix = profile["prefix"]
    char = profile.get("character", "signature")
    voice = _voice(profile)
    bias = float(eng.get("root_bias_st", 0.0))
    base = sku_seed(profile["sku"])
    rows: list[dict] = []

    def save(kind: str, name: str, buf: np.ndarray) -> None:
        folder = folders[kind]
        path = out_dir / folder / name
        write_wav_16bit_mono(path, buf)
        rows.append({"kind": kind, "path": str(path.relative_to(out_dir)), "name": name})

    def fname(kind: str, i: int, note: str = "NA") -> str:
        tag = tags.get(kind, kind)
        return f"{prefix}_{kind}_{tag}_{note}_{i+1:02d}.wav".replace("__", "_")

    # 808s
    for i in range(inv.get("808", 0)):
        if voice["808_tuning"] == "chromatic":
            freq = _bias_hz(32.70 * (2.0 ** ((i % 12) / 12.0)), 0)
            note = f"K{i % 12}"
        else:
            freq = _bias_hz(KICK_KEYS[i % len(KICK_KEYS)], bias)
            note = f"F{i % 7}"
        dur = float(eng.get("kick_duration", 0.35))
        if voice["kick_duration_min"] is not None:
            dur = max(dur, float(voice["kick_duration_min"]))
        buf = generate_memphis_phonk_kick(
            root_freq=freq,
            duration=dur,
            pitch_mod_depth=float(eng["kick_pitch_st"]),
            pitch_decay=float(voice["kick_pitch_decay"]),
            clip_drive_db=float(eng["kick_clip_db"]) + (i % 5) * 0.15,
            lofi_sr=float(eng["lofi_sr"]),
            bit_depth=int(eng["bit_depth"]),
            tape_lpf_hz=float(eng["tape_lpf_hz"]),
        )
        save("808", fname("808", i, note), buf)

    # Kicks (non-808)
    for i in range(inv.get("KICK", 0)):
        freq = _bias_hz(KICK_KEYS[(i + 2) % len(KICK_KEYS)], bias + float(voice["kick_extra_bias_st"]))
        pitch = float(eng["kick_pitch_st"])
        if voice["kick_pitch_cap"] is not None:
            pitch = min(pitch, float(voice["kick_pitch_cap"]))
        buf = generate_memphis_phonk_kick(
            root_freq=freq,
            duration=float(eng.get("kick_duration", 0.22)),
            pitch_mod_depth=pitch + (i % 6),
            clip_drive_db=float(eng["kick_clip_db"]) + (i % 4) * 0.25,
            lofi_sr=float(eng["lofi_sr"]),
            bit_depth=int(eng["bit_depth"]),
            tape_lpf_hz=float(eng["tape_lpf_hz"]),
        )
        save("KICK", fname("KICK", i), buf)

    # Snares
    for i in range(inv.get("SNR", 0)):
        mem = SNR_HZ[i % len(SNR_HZ)]
        mod = max(1, int(voice["snare_pitch_mod"]))
        pitch_st = int(voice["snare_pitch_base"]) + (i % mod)
        flam = float(voice["snare_flam_base_ms"]) + (i % 5) * float(voice["snare_flam_step_ms"])
        buf = generate_memphis_phonk_snare(
            membrane_root=mem,
            pitch_mod_st=pitch_st,
            flam_delay_ms=flam,
            clip_drive_db=float(eng["snare_clip_db"]) + (i % 4) * 0.2,
            seed=sku_seed(profile["sku"], 9300 + i),
        )
        save("SNR", fname("SNR", i), buf)

    # Claps
    for i in range(inv.get("CLAP", 0)):
        buf = _clap(
            9400 + i,
            layered=bool(voice["clap_layered"]),
            bright=bool(voice["clap_bright"]),
        )
        save("CLAP", fname("CLAP", i), buf)

    # Hats
    for i in range(inv.get("CH", 0)):
        hz = float(eng.get("hat_center_hz", 9000)) + (i % 10) * 150 - 300
        lofi = float(eng["lofi_sr"]) if voice["hat_use_lofi"] else 44100.0
        buf = generate_memphis_phonk_hat(
            center_hz=hz,
            seed=9600 + i,
            noise_decay=float(voice["hat_noise_decay"]) + (i % 4) * 0.002,
            clip_drive_db=float(voice["hat_clip_db"]),
            bit_depth=int(eng["bit_depth"]),
            lofi_sr=lofi,
        )
        save("CH", fname("CH", i), buf)

    for i in range(inv.get("OH", 0)):
        hz = float(eng.get("hat_center_hz", 9000)) + (i % 6) * 180
        buf = generate_memphis_phonk_hat(
            duration=float(voice["oh_duration"]),
            center_hz=hz,
            noise_decay=float(voice["oh_noise_decay"]),
            seed=9700 + i,
            clip_drive_db=2.5,
            bit_depth=int(eng["bit_depth"]),
            lofi_sr=float(eng["lofi_sr"]),
        )
        save("OH", fname("OH", i), buf)

    # Cowbells
    for i in range(inv.get("COW", 0)):
        pitch = 0.8 + (i % 12) * 0.035
        if voice["cow_metallic_fixed"] is not None:
            metallic = float(voice["cow_metallic_fixed"])
        else:
            metallic = 1.0 + (i % 3) * 0.05
        save("COW", fname("COW", i), _cowbell(pitch, eng, metallic=metallic))

    # Toms
    tom_roots = [90, 110, 130, 150, 170, 95, 120, 140, 160, 180]
    for i in range(inv.get("TOM", 0)):
        save("TOM", fname("TOM", i), _tom(tom_roots[i % len(tom_roots)]))

    # Rimshots
    for i in range(inv.get("RIM", 0)):
        save("RIM", fname("RIM", i), _perc(9800 + i, rim=True))

    # Perc
    rim_every = int(voice["perc_rim_every"] or 0)
    for i in range(inv.get("PERC", 0)):
        buf = _perc(
            9900 + i,
            grimy=bool(voice["perc_grimy"]),
            synth=bool(voice["perc_synth"]),
            rim=(rim_every > 0 and i % rim_every == 0),
        )
        save("PERC", fname("PERC", i), buf)

    # Loops
    for i in range(inv.get("LOOP", 0)):
        bp_path, bpm = BLUEPRINTS[i % len(BLUEPRINTS)]
        alt = voice.get("loop_bpm_alt")
        if alt:
            bpm = int(alt[i % len(alt)])
        bp = json.loads(bp_path.read_text(encoding="utf-8"))
        use_bpm = float(bp.get("bpm", bpm))
        audio = render_memphis_loop(bp, seed=sku_seed(profile["sku"], 800 + i))
        want = loop_samples(int(bp["bars"]), use_bpm, 44100)
        if len(audio) > want:
            audio = audio[:want]
        elif len(audio) < want:
            audio = np.pad(audio, (0, want - len(audio)))
        name = f"{prefix}_LOOP_{int(use_bpm)}bpm_{i+1:02d}.wav"
        save("LOOP", name, audio)

    total = len(rows)
    lo, hi = profile.get("target_samples", [0, 10_000])
    manifest = {
        "sku": profile["sku"],
        "title": profile["title"],
        "identity": profile["identity"],
        "character": char,
        "release_ready": False,
        "sample_count": total,
        "target_samples": [lo, hi],
        "in_band": lo <= total <= hi,
        "seed_base": base,
        "delivery": {"sample_rate": 44100, "bit_depth": 16, "channels": 1},
        "samples": rows,
    }
    (out_dir / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    if not manifest["in_band"]:
        raise RuntimeError(f"{profile['sku']} count {total} outside {lo}-{hi}")
    return rows


def main() -> None:
    parser = argparse.ArgumentParser(description="Generate a Black Box era kit from profile JSON")
    parser.add_argument("era", help="bb93|bb96|bb98|bb2000 or path to profile JSON")
    parser.add_argument("--out", type=Path, default=None)
    args = parser.parse_args()
    profile = load_profile(args.era)
    out = args.out or kit_masters_dir(profile)
    rows = generate_all(out, profile)
    print(f"{profile['sku']} generated {len(rows)} samples → {out}")
    by: dict[str, int] = {}
    for r in rows:
        by[r["kind"]] = by.get(r["kind"], 0) + 1
    for k, v in sorted(by.items()):
        print(f"  {k}: {v}")


if __name__ == "__main__":
    main()
