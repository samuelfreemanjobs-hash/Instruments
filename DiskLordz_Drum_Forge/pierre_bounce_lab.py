#!/usr/bin/env python3
"""DiskLordz Cloud Bounce Drum Engine (Woody Spinz 808, Razor Clap, Reverse Open-Hat Swell).

Implements the exact DSP recipes with real-time/export bug fixes:
- 145 Hz parametric peaking EQ before oversampled tanh saturation for woody Spinz knock
- 3-burst 9 ms flam clap with 380 Hz high-pass and 1850 Hz nasal presence peak
- Band-limited 6-oscillator (205.3..800 Hz) reverse open-hat swell using concave (t/attack)^2.4
  curve + 2.0 ms click-free choke ramp (preventing the 1-sample step click at t > totalDuration)
"""
from __future__ import annotations

import argparse
import json
from dataclasses import asdict, dataclass
from pathlib import Path

import numpy as np
import soundfile as sf
from scipy.signal import sosfilt

import forge_core_v2 as core

ENGINE = "DiskLordz CLOUD BOUNCE LAB v1"


@dataclass
class BounceParams:
    sub_freq: float = 48.0
    knock_freq: float = 165.0
    knock_decay_ms: float = 22.0
    sub_decay_ms: float = 360.0
    woody_mid_hz: float = 145.0
    woody_mid_q: float = 1.4
    woody_mid_gain: float = 1.6
    tape_saturation: float = 1.65

    clap_hp_hz: float = 380.0
    clap_peak_hz: float = 1850.0
    clap_peak_q: float = 2.0
    clap_peak_gain: float = 1.8
    clap_decay_ms: float = 78.0

    rev_attack_ms: float = 110.0
    rev_hold_ms: float = 15.0
    rev_choke_ms: float = 2.2
    hat_pitch_scale: float = 1.08


def peaking_eq(x: np.ndarray, sr: int, freq_hz: float, q: float, linear_gain: float) -> np.ndarray:
    """RBJ audio-EQ cookbook peaking biquad matching JUCE IIRCoefficients::makePeakFilter."""
    a_amp = float(np.sqrt(max(linear_gain, 0.05)))
    w0 = 2.0 * np.pi * float(np.clip(freq_hz, 20.0, sr * 0.45)) / sr
    alpha = np.sin(w0) / (2.0 * max(q, 0.1))
    cos_w0 = np.cos(w0)

    b0 = 1.0 + alpha * a_amp
    b1 = -2.0 * cos_w0
    b2 = 1.0 - alpha * a_amp
    a0 = 1.0 + alpha / a_amp
    a1 = -2.0 * cos_w0
    a2 = 1.0 - alpha / a_amp

    sos = np.array([[b0 / a0, b1 / a0, b2 / a0, 1.0, a1 / a0, a2 / a0]], dtype=np.float64)
    return sosfilt(sos, x)


def gen_woody_808(sr: int, p: BounceParams) -> tuple[np.ndarray, float]:
    knock_tau = p.knock_decay_ms * 0.001
    sub_tau = p.sub_decay_ms * 0.001
    duration = float(np.clip(sub_tau * 5.5, 0.55, 2.5))
    t = np.arange(int(sr * duration)) / sr

    freq = p.sub_freq + (p.knock_freq - p.sub_freq) * np.exp(-t / knock_tau)
    sine = core.osc(freq, sr)
    amp_env = (1.0 - np.exp(-t / 0.0005)) * np.exp(-t / sub_tau)

    shaped = peaking_eq(sine, sr, p.woody_mid_hz, p.woody_mid_q, p.woody_mid_gain)
    saturated = core.saturate_oversampled(shaped, drive=p.tape_saturation, mix=1.0, factor=4)
    return saturated * amp_env, p.sub_freq


def gen_razor_clap(sr: int, rng: np.random.Generator, p: BounceParams) -> tuple[np.ndarray, None]:
    tail_tau = p.clap_decay_ms * 0.001
    duration = float(np.clip(0.025 + tail_tau * 6.0, 0.22, 0.65))
    t = np.arange(int(sr * duration)) / sr

    env = np.zeros_like(t)
    spread_sec = 0.009
    for b in range(3):
        start = b * spread_sec
        decay = tail_tau if b == 2 else 0.006
        env += np.exp(-np.maximum(t - start, 0.0) / decay) * (t >= start)

    noise = rng.uniform(-1.0, 1.0, len(t)) * np.clip(env * 0.55, 0.0, 1.0)
    hp = core.filter_signal(noise, p.clap_hp_hz, "highpass", sr, 2)
    snappy = peaking_eq(hp, sr, p.clap_peak_hz, p.clap_peak_q, p.clap_peak_gain)
    return snappy, None


def gen_reverse_open_hat(sr: int, p: BounceParams) -> tuple[np.ndarray, None]:
    """Band-limited 6-oscillator reverse hat swell with click-free choke."""
    attack_s = p.rev_attack_ms * 0.001
    hold_s = p.rev_hold_ms * 0.001
    choke_s = max(0.0015, p.rev_choke_ms * 0.001)
    pad_tail_s = 0.008
    total_s = attack_s + hold_s + choke_s + pad_tail_s
    t = np.arange(int(sr * total_s)) / sr

    env = np.zeros_like(t)
    atk_mask = t < attack_s
    hold_mask = (t >= attack_s) & (t < attack_s + hold_s)
    choke_mask = (t >= attack_s + hold_s) & (t < attack_s + hold_s + choke_s)

    env[atk_mask] = (t[atk_mask] / max(attack_s, 1e-5)) ** 2.4
    env[hold_mask] = 1.0
    env[choke_mask] = 1.0 - (t[choke_mask] - (attack_s + hold_s)) / choke_s

    ratios = (205.3, 304.4, 369.6, 522.7, 540.0, 800.0)
    nyq_limit = sr * 0.45
    metallic = np.zeros_like(t)
    for f0 in ratios:
        base = f0 * p.hat_pitch_scale
        max_h = min(35, int(nyq_limit // max(base, 1.0)))
        for h in range(1, max_h + 1, 2):
            metallic += (4.0 / np.pi) * (1.0 / h) * np.sin(2.0 * np.pi * (base * h) * t) * 0.12

    filtered = core.filter_signal(metallic, 7200.0, "highpass", sr, 2)
    return filtered * env * 0.85, None


def build_bounce_kit(out_dir: Path, sr: int = 48000, variations: int = 4, seed: int = 404808) -> dict:
    out_dir.mkdir(parents=True, exist_ok=True)
    roots = (("F1", 43.6535), ("Fs1", 46.2493), ("G1", 48.9994), ("A1", 55.0))
    files = []

    for i in range(variations):
        rng = np.random.default_rng(seed + i * 29)
        note_tag, root_hz = roots[i % len(roots)]
        p = BounceParams(
            sub_freq=root_hz,
            knock_freq=float(rng.uniform(152.0, 178.0)),
            knock_decay_ms=float(rng.uniform(19.0, 25.0)),
            sub_decay_ms=float(rng.uniform(320.0, 410.0)),
            woody_mid_gain=float(rng.uniform(1.45, 1.85)),
            tape_saturation=float(rng.uniform(1.5, 1.85)),
            clap_peak_hz=float(rng.uniform(1750.0, 1980.0)),
            clap_decay_ms=float(rng.uniform(68.0, 90.0)),
            rev_attack_ms=float(rng.uniform(95.0, 135.0)),
            hat_pitch_scale=float(rng.uniform(1.02, 1.14)),
        )

        b808, _ = gen_woody_808(sr, p)
        clap, _ = gen_razor_clap(sr, rng, p)
        rhat, _ = gen_reverse_open_hat(sr, p)

        specs = (
            ("808", f"DL_BOUNCE_Woody808_{note_tag}_V{i + 1:02d}.wav", b808),
            ("clap", f"DL_BOUNCE_RazorClap_V{i + 1:02d}.wav", clap),
            ("reverse_hat", f"DL_BOUNCE_RevOpenHat_V{i + 1:02d}.wav", rhat),
        )
        for folder, fname, sig in specs:
            mastered = core.master(sig, sr, -1.0)
            dest = out_dir / folder / fname
            dest.parent.mkdir(parents=True, exist_ok=True)
            sf.write(dest, mastered, sr, subtype="PCM_24")
            files.append({"file": f"{folder}/{fname}", "params": asdict(p)})

    manifest = {"engine": ENGINE, "sample_rate": sr, "seed": seed, "count": len(files), "files": files}
    (out_dir / "manifest.json").write_text(json.dumps(manifest, indent=2), encoding="utf-8")
    return manifest


def main() -> None:
    ap = argparse.ArgumentParser(description=ENGINE)
    ap.add_argument("--variations", type=int, default=4)
    ap.add_argument("--sr", type=int, choices=(44100, 48000, 96000), default=48000)
    ap.add_argument("--seed", type=int, default=404808)
    ap.add_argument("--out", default="DiskLordz_Cloud_Bounce_Kit")
    args = ap.parse_args()
    manifest = build_bounce_kit(Path(args.out), args.sr, args.variations, args.seed)
    print(f"Exported {manifest['count']} WAV files to {args.out}")


if __name__ == "__main__":
    main()
