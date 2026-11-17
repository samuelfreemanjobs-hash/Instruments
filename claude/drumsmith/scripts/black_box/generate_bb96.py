#!/usr/bin/env python3
"""Generate BLACK BOX 96 — The Original Machine (240 PCM16 oneshots + loops)."""

from __future__ import annotations

import argparse
import json
import math
import sys
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[2]
for p in (str(ROOT), str(ROOT / "scripts")):
    if p not in sys.path:
        sys.path.insert(0, p)

from dsp_math import loop_samples  # noqa: E402
from scripts.memphis_oneshots.phonk_hat import generate_memphis_phonk_hat  # noqa: E402
from scripts.memphis_oneshots.phonk_kick import (  # noqa: E402
    generate_memphis_phonk_kick,
    write_wav_16bit_mono,
)
from scripts.memphis_oneshots.phonk_snare import generate_memphis_phonk_snare  # noqa: E402
from scripts.memphis_oneshots.render_memphis_loop import render_memphis_loop  # noqa: E402

PROFILE = ROOT / "kits/black-box-collection/profiles/bb96.json"
OUT_DEFAULT = ROOT / "kits/black-box-96/masters"

KICK_KEYS = [32.70, 36.71, 41.20, 43.65, 46.25, 49.00, 55.00, 38.89]
SNR_HZ = [180, 196, 208, 220, 233, 247, 262, 185, 200, 215]


def _norm(x: np.ndarray, ceiling_db: float = -0.1) -> np.ndarray:
    peak = float(np.max(np.abs(x))) or 1.0
    return ((x / peak) * (10.0 ** (ceiling_db / 20.0))).astype(np.float32)


def _cowbell(pitch: float, duration: float = 0.22, metallic: float = 1.0) -> np.ndarray:
    sr = 44100
    n = int(round(sr * duration))
    t = np.linspace(0.0, duration, n, endpoint=False)
    f1, f2 = 540.0 * pitch * metallic, 800.0 * pitch * metallic
    sq = 0.5 * np.sign(np.sin(2 * np.pi * f1 * t)) + 0.5 * np.sign(np.sin(2 * np.pi * f2 * t))
    # metallic ring partial
    ring = 0.15 * np.sin(2 * np.pi * f1 * 2.7 * t) * np.exp(-t / 0.04)
    env = np.exp(-t / 0.09)
    return _norm(sq * env + ring)


def _clap(seed: int) -> np.ndarray:
    rng = np.random.default_rng(seed)
    sr = 44100
    n = int(0.11 * sr)
    t = np.linspace(0.0, 0.11, n, endpoint=False)
    noise = rng.uniform(-1, 1, n)
    amp = np.zeros_like(t)
    for off, g in ((0.0, 0.45), (0.011, 0.75), (0.024, 1.0)):
        i = int(off * sr)
        amp[i:] += g * np.exp(-(t[i:] - t[i]) / 0.028)
    y = np.diff(noise * amp, prepend=0.0)
    return _norm(y)


def _tom(root: float, seed: int) -> np.ndarray:
    sr = 44100
    dur = 0.28
    n = int(round(sr * dur))
    t = np.linspace(0.0, dur, n, endpoint=False)
    pitch_env = np.exp(-t / 0.035)
    freq = root * (2.0 ** (18.0 * pitch_env / 12.0))
    phase = 2.0 * math.pi * np.cumsum(freq) / sr
    osc = 0.7 * np.sin(phase) + 0.3 * (2 / math.pi) * np.arcsin(np.sin(phase))
    amp = np.exp(-t / 0.12)
    return _norm(osc * amp)


def _perc(seed: int) -> np.ndarray:
    rng = np.random.default_rng(seed)
    sr = 44100
    n = int(0.06 * sr)
    t = np.linspace(0.0, 0.06, n, endpoint=False)
    click = rng.uniform(-1, 1, n) * np.exp(-t / 0.008)
    tone = np.sin(2 * math.pi * 1200 * t) * np.exp(-t / 0.02) * 0.4
    return _norm(click + tone)


def _oh(center: float, seed: int) -> np.ndarray:
    return generate_memphis_phonk_hat(
        duration=0.22,
        center_hz=center,
        noise_decay=0.08,
        seed=seed,
        clip_drive_db=3.0,
    )


def generate_all(out_dir: Path, profile: dict) -> list[dict]:
    if out_dir.exists():
        import shutil

        shutil.rmtree(out_dir)
    inv = profile["inventory"]
    eng = profile["engine"]
    rows: list[dict] = []

    def save(folder: str, name: str, buf: np.ndarray, kind: str) -> None:
        path = out_dir / folder / name
        write_wav_16bit_mono(path, buf)
        rows.append({"kind": kind, "path": str(path.relative_to(out_dir)), "name": name})

    # 808s
    for i in range(inv["808"]):
        freq = KICK_KEYS[i % len(KICK_KEYS)]
        buf = generate_memphis_phonk_kick(
            root_freq=freq,
            duration=0.35,
            pitch_mod_depth=eng["kick_pitch_st"],
            clip_drive_db=eng["kick_clip_db"] + (i % 5) * 0.2,
            lofi_sr=eng["lofi_sr"],
            bit_depth=eng["bit_depth"],
            tape_lpf_hz=eng["tape_lpf_hz"],
        )
        save("01_808_Subs", f"BB96_808_Thick_F{i%7}_{i+1:02d}.wav", buf, "808")

    # Punch kicks
    for i in range(inv["KICK"]):
        freq = KICK_KEYS[(i + 3) % len(KICK_KEYS)]
        buf = generate_memphis_phonk_kick(
            root_freq=freq,
            duration=0.22,
            pitch_mod_depth=40 + (i % 8),
            clip_drive_db=4.0 + (i % 4) * 0.3,
            lofi_sr=eng["lofi_sr"],
            bit_depth=eng["bit_depth"],
        )
        save("02_Kicks", f"BB96_KICK_Punch_NA_{i+1:02d}.wav", buf, "KICK")

    # Snares
    for i in range(inv["SNR"]):
        buf = generate_memphis_phonk_snare(
            membrane_root=SNR_HZ[i % len(SNR_HZ)],
            pitch_mod_st=22 + (i % 10),
            flam_delay_ms=3.0 + (i % 5) * 0.5,
            clip_drive_db=eng["snare_clip_db"] + (i % 4) * 0.25,
            seed=9600 + i,
        )
        save("03_Snares", f"BB96_SNR_Aggro_NA_{i+1:02d}.wav", buf, "SNR")

    # Claps
    for i in range(inv["CLAP"]):
        save("04_Claps", f"BB96_CLAP_Dry_NA_{i+1:02d}.wav", _clap(9700 + i), "CLAP")

    # Closed hats
    for i in range(inv["CH"]):
        hz = eng["hat_center_hz"] + (i % 10) * 180 - 400
        buf = generate_memphis_phonk_hat(center_hz=hz, seed=9800 + i, noise_decay=0.022 + (i % 5) * 0.002)
        save("05_Hats_Closed", f"BB96_CH_Crunch_NA_{i+1:02d}.wav", buf, "CH")

    # Open hats
    for i in range(inv["OH"]):
        hz = eng["hat_center_hz"] + (i % 6) * 200
        save("06_Hats_Open", f"BB96_OH_Crunch_NA_{i+1:02d}.wav", _oh(hz, 9900 + i), "OH")

    # Cowbells
    for i in range(inv["COW"]):
        pitch = 0.85 + (i % 12) * 0.04
        save("07_Cowbells", f"BB96_COW_Metal_NA_{i+1:02d}.wav", _cowbell(pitch, metallic=1.0 + (i % 3) * 0.05), "COW")

    # Toms
    tom_roots = [90, 110, 130, 150, 170, 95, 120, 140, 160, 180]
    for i in range(inv["TOM"]):
        save("08_Toms", f"BB96_TOM_Analog_NA_{i+1:02d}.wav", _tom(tom_roots[i % len(tom_roots)], 10000 + i), "TOM")

    # Perc
    for i in range(inv["PERC"]):
        save("09_Percussion", f"BB96_PERC_Rim_NA_{i+1:02d}.wav", _perc(11000 + i), "PERC")

    # Loops from archetypes
    blueprints = [
        (ROOT / "kits/converter-night/loops/cnv-dr660-94-a.json", 94),
        (ROOT / "kits/converter-night/loops/cnv-arch-horror.json", 94),
        (ROOT / "kits/converter-night/loops/cnv-arch-bitcrush.json", 94),
        (ROOT / "kits/converter-night/loops/cnv-arch-bounce.json", 134),
        (ROOT / "kits/converter-night/loops/cnv-dr660-134-crawl.json", 134),
        (ROOT / "kits/converter-night/loops/cnv-rhythm-94.json", 94),
    ]
    for i in range(inv["LOOP"]):
        bp_path, bpm = blueprints[i % len(blueprints)]
        bp = json.loads(bp_path.read_text(encoding="utf-8"))
        audio = render_memphis_loop(bp, seed=960 + i)
        # trim/pad to exact loop length
        want = loop_samples(int(bp["bars"]), float(bp["bpm"]), 44100)
        if len(audio) > want:
            audio = audio[:want]
        elif len(audio) < want:
            audio = np.pad(audio, (0, want - len(audio)))
        save("10_Loops", f"BB96_LOOP_{bpm}bpm_{i+1:02d}.wav", audio, "LOOP")

    manifest = {
        "sku": "BB96",
        "title": "BLACK BOX 96",
        "identity": "The Original Machine",
        "release_ready": False,
        "sample_count": len(rows),
        "delivery": {"sample_rate": 44100, "bit_depth": 16, "channels": 1},
        "samples": rows,
    }
    (out_dir / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    return rows


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out", type=Path, default=OUT_DEFAULT)
    parser.add_argument("--profile", type=Path, default=PROFILE)
    args = parser.parse_args()
    profile = json.loads(args.profile.read_text(encoding="utf-8"))
    rows = generate_all(args.out, profile)
    print(f"BB96 generated {len(rows)} samples → {args.out}")
    by = {}
    for r in rows:
        by[r["kind"]] = by.get(r["kind"], 0) + 1
    for k, v in sorted(by.items()):
        print(f"  {k}: {v}")


if __name__ == "__main__":
    main()
