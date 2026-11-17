#!/usr/bin/env python3
"""Headless 2-bar Memphis loop renderer — synthesizes oneshots onto a blueprint grid.

P2 twin for OfflineAudio: no browser required. Mixes KICK/SNARE/HAT (and a simple
cowbell) from MEMPHIS engines onto drumsmith-loop-blueprint-v1 JSON → PCM16 WAV.
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS = ROOT / "scripts"
for p in (str(ROOT), str(SCRIPTS)):
    if p not in sys.path:
        sys.path.insert(0, p)

from dsp_math import loop_samples  # noqa: E402
from scripts.memphis_oneshots.phonk_hat import generate_memphis_phonk_hat  # noqa: E402
from scripts.memphis_oneshots.phonk_kick import (  # noqa: E402
    generate_memphis_phonk_kick,
    write_wav_16bit_mono,
)
from scripts.memphis_oneshots.phonk_snare import generate_memphis_phonk_snare  # noqa: E402


def _cowbell(sample_rate: int = 44100, duration: float = 0.18, pitch: float = 1.0) -> np.ndarray:
    """Minimal dual-square 808-ish cowbell for loop beds."""
    n = int(round(sample_rate * duration))
    t = np.linspace(0.0, duration, n, endpoint=False)
    f1, f2 = 540.0 * pitch, 800.0 * pitch
    sq = 0.55 * np.sign(np.sin(2 * np.pi * f1 * t)) + 0.45 * np.sign(np.sin(2 * np.pi * f2 * t))
    env = np.exp(-t / 0.07)
    out = sq * env
    peak = float(np.max(np.abs(out))) or 1.0
    return (out / peak * 0.7).astype(np.float32)


def _voice_buffers(seed: int = 0) -> dict[str, np.ndarray]:
    # Vary body per seed so looping the same blueprint does not clone the same kit
    kick_root = 43.65 * (2.0 ** (((seed % 7) - 3) / 12.0))
    snare_root = 200.0 + (seed % 9) * 6.0
    hat_hz = 8600.0 + (seed % 11) * 180.0
    return {
        "KICK": generate_memphis_phonk_kick(
            root_freq=kick_root,
            duration=0.22,
            amp_decay=0.11,
            click_amount=0.35,
            clip_drive_db=4.5 + (seed % 4) * 0.3,
            seed=1000 + seed,
        ),
        "SNARE": generate_memphis_phonk_snare(membrane_root=snare_root, seed=660 + seed),
        "HAT": generate_memphis_phonk_hat(center_hz=hat_hz, seed=808 + seed),
        "COWBELL": _cowbell(pitch=0.9 + (seed % 5) * 0.05),
        "808": generate_memphis_phonk_kick(
            root_freq=kick_root,
            duration=1.4,
            pitch_mod_depth=42.0 + (seed % 8),
            amp_decay=0.65,
            click_amount=0.45,
            clip_drive_db=5.0,
            seed=2000 + seed,
        ),
    }


def render_memphis_loop(
    blueprint: dict,
    *,
    ceiling_dbfs: float = -0.1,
    seed: int = 0,
) -> np.ndarray:
    """Return float32 mono mix for the blueprint."""
    bpm = float(blueprint["bpm"])
    bars = int(blueprint["bars"])
    sr = int(blueprint.get("sample_rate", 44100))
    grid = int(blueprint["grid_subdivisions_per_bar"])
    samples_bar = int(blueprint.get("samples_per_bar") or loop_samples(1, bpm, sr))
    file_len = loop_samples(bars, bpm, sr)
    mix_len = max(samples_bar * bars, file_len)
    step_len = samples_bar / float(grid)

    voices = _voice_buffers(seed=seed)
    buf = np.zeros(mix_len, dtype=np.float64)
    lanes = blueprint.get("lanes") or {}
    for kind, events in lanes.items():
        oneshot = voices.get(kind)
        if oneshot is None or not events:
            continue
        for ev in events:
            step = int(ev["step"])
            vel = float(ev.get("vel", 100)) / 127.0
            start = int(round(step * step_len))
            if start >= mix_len:
                continue
            end = min(mix_len, start + len(oneshot))
            sl = end - start
            buf[start:end] += oneshot[:sl].astype(np.float64) * vel

    peak = float(np.max(np.abs(buf))) or 1.0
    ceiling = 10.0 ** (ceiling_dbfs / 20.0)
    out = (buf / peak) * ceiling
    return out[:file_len].astype(np.float32)


def main() -> None:
    parser = argparse.ArgumentParser(description="Render a Memphis kit loop from blueprint JSON")
    parser.add_argument(
        "--blueprint",
        type=Path,
        default=ROOT / "kits/converter-night/loops/cnv-dr660-94-a.json",
    )
    parser.add_argument("--out", type=Path, default=Path("/tmp/cnv_memphis_loop_94.wav"))
    parser.add_argument("--seed", type=int, default=0)
    args = parser.parse_args()

    bp = json.loads(args.blueprint.read_text(encoding="utf-8"))
    audio = render_memphis_loop(bp, seed=args.seed)
    write_wav_16bit_mono(args.out, audio, sample_rate=int(bp.get("sample_rate", 44100)))
    print(
        f"wrote {args.out} ({len(audio)} samples, bpm={bp.get('bpm')}, bars={bp.get('bars')})"
    )


if __name__ == "__main__":
    main()
