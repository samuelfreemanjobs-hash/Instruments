#!/usr/bin/env python3
"""DiskLordz Heavy ATL / 2010s Orchestral Trap Drum Engine.

Implements the exact DSP voice chain with deterministic seeding, 4x oversampling,
independent L/R stereo filter states, -70 dBFS silence trimming, and ZIP export:
- Brick 808: 3.5x (~2 octave) 35 ms pitch drop -> sine/triangle -> 4x oversampled
  asymmetric tanh -> 28 Hz sub-rumble HPF
- Beater Hard Kick: 300 -> 58 Hz sine sweep (18 ms) + 5 ms band-passed noise beater -> 4x hard clip
- Layered Snare & Clap: dual body (tuneHz + 1.78x shell overtone) + 3 staggered bursts (11 ms) +
  stereo noise -> independent L/R 120 Hz HPF -> 380 Hz (-4.5 dB) mid-scoop -> 4.5 kHz (+5.5 dB) snap shelf
- 909 Metallic Hat: 6 band-limited non-harmonic pulse oscillators (205.3, 304.4, 369.6, 522.7, 540.5, 800.0 Hz)
  with deterministic staggered start phases + 15% air noise -> 1.5 ms attack ramp -> 6.8 kHz HPF -> 10.5 kHz (+4 dB) air peak
"""
from __future__ import annotations

import argparse
import json
import zipfile
from dataclasses import asdict, dataclass
from pathlib import Path

import numpy as np
import soundfile as sf
from scipy.signal import resample_poly, sosfilt

import forge_core_v2 as core
import pierre_bounce_lab as bounce

ENGINE = "DiskLordz HEAVY ATL BRICK LAB v2"


@dataclass
class BrickParams:
    base_freq: float = 48.0
    drive: float = 4.5
    decay_sec: float = 1.4
    tri_blend: float = 0.16

    kick_start_hz: float = 300.0
    kick_end_hz: float = 58.0
    kick_sweep_ms: float = 18.0
    kick_decay_ms: float = 165.0
    kick_click_ms: float = 5.0

    snare_tune_hz: float = 185.0
    clap_spread_ms: float = 11.0
    snare_decay_sec: float = 0.24
    snap_drive: float = 2.2
    mid_scoop_db: float = -4.5
    high_shelf_db: float = 5.5

    hat_pitch_mult: float = 1.0
    hat_decay_sec: float = 0.055
    open_hat_decay_sec: float = 0.35
    hat_grit_drive: float = 1.6
    hat_air_db: float = 4.0


def db_to_gain(db: float) -> float:
    return float(10.0 ** (db / 20.0))


def high_shelf_eq(x: np.ndarray, sr: int, freq_hz: float, q: float, linear_gain: float) -> np.ndarray:
    """RBJ high-shelf biquad applied independently per channel (axis=0)."""
    a_amp = float(np.sqrt(max(linear_gain, 0.05)))
    w0 = 2.0 * np.pi * float(np.clip(freq_hz, 40.0, sr * 0.45)) / sr
    cos_w0 = np.cos(w0)
    sin_w0 = np.sin(w0)
    alpha = sin_w0 / (2.0 * max(q, 0.1))
    two_sqrt_a_alpha = 2.0 * np.sqrt(a_amp) * alpha

    b0 = a_amp * ((a_amp + 1.0) + (a_amp - 1.0) * cos_w0 + two_sqrt_a_alpha)
    b1 = -2.0 * a_amp * ((a_amp - 1.0) + (a_amp + 1.0) * cos_w0)
    b2 = a_amp * ((a_amp + 1.0) + (a_amp - 1.0) * cos_w0 - two_sqrt_a_alpha)
    a0 = (a_amp + 1.0) - (a_amp - 1.0) * cos_w0 + two_sqrt_a_alpha
    a1 = 2.0 * ((a_amp - 1.0) - (a_amp + 1.0) * cos_w0)
    a2 = (a_amp + 1.0) - (a_amp - 1.0) * cos_w0 - two_sqrt_a_alpha

    sos = np.array([[b0 / a0, b1 / a0, b2 / a0, 1.0, a1 / a0, a2 / a0]], dtype=np.float64)
    return sosfilt(sos, x, axis=0)


def peaking_eq_stereo(x: np.ndarray, sr: int, freq_hz: float, q: float, linear_gain: float) -> np.ndarray:
    """Independent per-channel peaking EQ (no L/R state bleeding)."""
    if x.ndim == 1:
        return bounce.peaking_eq(x, sr, freq_hz, q, linear_gain)
    left = bounce.peaking_eq(x[:, 0], sr, freq_hz, q, linear_gain)
    right = bounce.peaking_eq(x[:, 1], sr, freq_hz, q, linear_gain)
    return np.column_stack((left, right))


def trim_below_db(x: np.ndarray, sr: int, threshold_db: float = -70.0, release_ms: float = 12.0) -> np.ndarray:
    env = np.max(np.abs(x), axis=1) if x.ndim == 2 else np.abs(x)
    idx = np.flatnonzero(env > db_to_gain(threshold_db))
    if not len(idx):
        return x
    end = min(len(x), int(idx[-1]) + 1 + int(sr * release_ms * 0.001))
    return core.fade(x[:end], sr, fadein_ms=0.1, fadeout_ms=4.0)


def _oversampled_asymmetric_brick(x: np.ndarray, drive: float, post_clip: float = 0.95) -> np.ndarray:
    up = resample_poly(x, 4, 1, axis=0)
    driven = up * drive
    sat = np.where(driven >= 0.0, np.tanh(driven), np.tanh(driven * 0.85) * 0.95)
    clipped = np.clip(sat * 1.15, -post_clip, post_clip)
    down = resample_poly(clipped, 1, 4, axis=0)[: len(x)]
    if len(down) < len(x):
        pad_shape = ((0, len(x) - len(down)), (0, 0)) if x.ndim == 2 else (0, len(x) - len(down))
        down = np.pad(down, pad_shape)
    return down - np.mean(down, axis=0)


def gen_brick_808(sr: int, p: BrickParams) -> tuple[np.ndarray, float]:
    duration = float(np.clip(max(1.5, p.decay_sec * 1.9), 1.2, 3.4))
    t = np.arange(int(sr * duration)) / sr

    pitch_start = p.base_freq * 3.5
    freq = p.base_freq + (pitch_start - p.base_freq) * np.exp(-t / 0.035)
    phase = 2.0 * np.pi * np.cumsum(freq) / sr

    sine = np.sin(phase)
    tri = (2.0 / np.pi) * np.arcsin(np.sin(phase))
    raw_osc = (1.0 - p.tri_blend) * sine + p.tri_blend * tri

    amp_env = (1.0 - np.exp(-t / 0.0006)) * np.exp(-t / p.decay_sec)
    shaped = _oversampled_asymmetric_brick(raw_osc, drive=p.drive, post_clip=0.96) * amp_env
    cleaned = core.filter_signal(shaped, 28.0, "highpass", sr, 2)
    return trim_below_db(cleaned, sr), p.base_freq


def gen_beater_kick(sr: int, rng: np.random.Generator, p: BrickParams) -> tuple[np.ndarray, None]:
    t = np.arange(int(sr * 0.45)) / sr
    freq = p.kick_end_hz + (p.kick_start_hz - p.kick_end_hz) * np.exp(-t / (p.kick_sweep_ms * 0.001))
    body = core.osc(freq, sr) * (1.0 - np.exp(-t / 0.0003)) * np.exp(-t / (p.kick_decay_ms * 0.001))

    noise = rng.uniform(-1.0, 1.0, len(t))
    beater_bp = core.filter_signal(core.filter_signal(noise, 1400.0, "highpass", sr, 2), 5200.0, "lowpass", sr, 2)
    beater = beater_bp * np.exp(-t / (p.kick_click_ms * 0.001)) * 0.34
    return trim_below_db(_oversampled_asymmetric_brick(body + beater, drive=2.6), sr), None


def gen_snare_clap_layer(sr: int, rng: np.random.Generator, p: BrickParams) -> tuple[np.ndarray, None]:
    """True stereo L/R EQ chain: 120 Hz HPF -> 380 Hz (-4.5 dB) mid-scoop -> 4.5 kHz (+5.5 dB) snap shelf."""
    t = np.arange(int(sr * 0.60)) / sr
    pitch_offset = (p.snare_tune_hz * 1.4) * np.exp(-t / 0.014)
    f1 = p.snare_tune_hz + pitch_offset
    f2 = (p.snare_tune_hz * 1.78) + (pitch_offset * 1.2)

    osc1 = core.osc(f1, sr)
    osc2 = core.osc(f2, sr) * 0.55
    body = (osc1 + osc2) * np.exp(-t / 0.065)

    clap_env = np.zeros_like(t)
    spread_s = p.clap_spread_ms * 0.001
    for tap in range(3):
        start = tap * spread_s
        clap_env = np.where(t >= start, np.maximum(clap_env, np.exp(-np.maximum(t - start, 0.0) / 0.008)), clap_env)
    tail_env = 0.85 * np.exp(-t / p.snare_decay_sec)
    noise_env = (clap_env * 0.95) + (tail_env * 0.65)

    white_l = rng.uniform(-1.0, 1.0, len(t)) * noise_env
    white_r = rng.uniform(-1.0, 1.0, len(t)) * noise_env
    stereo = np.column_stack((body + white_l, body + white_r))

    # Independent per-channel filtering (no shared IIR state bleeding across L/R)
    hp = core.filter_signal(stereo, 120.0, "highpass", sr, 2)
    scooped = peaking_eq_stereo(hp, sr, 380.0, 1.4, db_to_gain(p.mid_scoop_db))
    snapped = high_shelf_eq(scooped, sr, 4500.0, 0.8, db_to_gain(p.high_shelf_db))
    saturated = _oversampled_asymmetric_brick(snapped, drive=p.snap_drive, post_clip=0.95)
    return trim_below_db(saturated, sr), None


def gen_909_metallic_hat(
    sr: int, rng: np.random.Generator, p: BrickParams, is_open: bool = False
) -> tuple[np.ndarray, None]:
    """6 band-limited TR-909 oscillators + 15% air noise -> 6.8 kHz HPF -> 10.5 kHz (+4 dB) air peak."""
    decay_s = p.open_hat_decay_sec if is_open else p.hat_decay_sec
    duration = max(0.40 if not is_open else 0.95, decay_s * 5.5)
    t = np.arange(int(sr * duration)) / sr

    base_freqs = (205.3, 304.4, 369.6, 522.7, 540.5, 800.0)
    init_phases = (0.0, 0.15, 0.33, 0.52, 0.71, 0.89)
    nyq_limit = sr * 0.45
    metallic = np.zeros_like(t)
    for f0, ph0 in zip(base_freqs, init_phases):
        freq = f0 * p.hat_pitch_mult
        max_h = min(35, int(nyq_limit // max(freq, 1.0)))
        for h in range(1, max_h + 1, 2):
            metallic += (4.0 / np.pi) * (1.0 / h) * np.sin(2.0 * np.pi * (freq * h * t + h * ph0))
    metallic /= 6.0

    air_noise = rng.uniform(-1.0, 1.0, len(t)) * 0.15
    raw_hat = metallic + air_noise

    attack_samples = max(1, int(0.0015 * sr))
    attack_gain = np.ones_like(t)
    attack_gain[:attack_samples] = np.linspace(0.0, 1.0, attack_samples, endpoint=False)
    env_sig = raw_hat * np.exp(-t / decay_s) * attack_gain

    hp = core.filter_signal(env_sig, 6800.0, "highpass", sr, 2)
    peaked = bounce.peaking_eq(hp, sr, 10500.0, 1.2, db_to_gain(p.hat_air_db))
    shaped = core.saturate_oversampled(peaked, drive=p.hat_grit_drive, mix=1.0, factor=4)
    return trim_below_db(np.clip(shaped, -0.95, 0.95), sr), None


def build_brick_kit(
    out_dir: Path, sr: int = 48000, variations: int = 4, seed: int = 1017808, make_zip: bool = True
) -> dict:
    out_dir.mkdir(parents=True, exist_ok=True)
    roots = (("E1", 41.2034), ("F1", 43.6535), ("G1", 48.9994), ("Gs1", 51.9131))
    rows = []
    written_paths: list[Path] = []

    for i in range(variations):
        rng = np.random.default_rng(seed + i * 41)
        note_tag, root_hz = roots[i % len(roots)]
        p = BrickParams(
            base_freq=root_hz,
            drive=float(rng.uniform(3.9, 5.2)),
            decay_sec=float(rng.uniform(1.2, 1.6)),
            kick_start_hz=float(rng.uniform(275.0, 325.0)),
            snare_tune_hz=float(rng.uniform(176.0, 198.0)),
            high_shelf_db=float(rng.uniform(4.5, 6.5)),
            hat_decay_sec=float(rng.uniform(0.045, 0.072)),
            hat_pitch_mult=float(rng.uniform(0.96, 1.06)),
        )
        b808, _ = gen_brick_808(sr, p)
        kick, _ = gen_beater_kick(sr, rng, p)
        snare_clap, _ = gen_snare_clap_layer(sr, rng, p)
        chat, _ = gen_909_metallic_hat(sr, rng, p, is_open=False)
        ohat, _ = gen_909_metallic_hat(sr, rng, p, is_open=True)

        items = (
            ("808", f"DL_BRICK_808_{note_tag}_V{i + 1:02d}.wav", b808),
            ("kick", f"DL_BRICK_BeaterKick_V{i + 1:02d}.wav", kick),
            ("snare_clap", f"DL_BRICK_SnareClap_V{i + 1:02d}.wav", snare_clap),
            ("closed_hat", f"DL_BRICK_909ClosedHat_V{i + 1:02d}.wav", chat),
            ("open_hat", f"DL_BRICK_909OpenHat_V{i + 1:02d}.wav", ohat),
        )
        for folder, fname, sig in items:
            mastered = core.master(sig, sr, -1.0)
            dest = out_dir / folder / fname
            dest.parent.mkdir(parents=True, exist_ok=True)
            sf.write(dest, mastered, sr, subtype="PCM_24")
            written_paths.append(dest)
            rows.append({"file": f"{folder}/{fname}", "channels": 1 if mastered.ndim == 1 else 2, "params": asdict(p)})

    manifest_path = out_dir / "manifest.json"
    manifest = {"engine": ENGINE, "sample_rate": sr, "seed": seed, "count": len(rows), "files": rows}
    manifest_path.write_text(json.dumps(manifest, indent=2), encoding="utf-8")

    if make_zip:
        zip_path = out_dir / "DiskLordz_Heavy_ATL_Brick_Pack.zip"
        with zipfile.ZipFile(zip_path, "w", zipfile.ZIP_DEFLATED) as zf:
            for wav_path in written_paths:
                zf.write(wav_path, arcname=wav_path.relative_to(out_dir).as_posix())
            zf.write(manifest_path, arcname="manifest.json")
        manifest["zip"] = zip_path.name
    return manifest


def main() -> None:
    ap = argparse.ArgumentParser(description=ENGINE)
    ap.add_argument("--variations", type=int, default=4)
    ap.add_argument("--sr", type=int, choices=(44100, 48000, 96000), default=48000)
    ap.add_argument("--seed", type=int, default=1017808)
    ap.add_argument("--out", default="DiskLordz_Heavy_ATL_Brick_Kit")
    args = ap.parse_args()
    manifest = build_brick_kit(Path(args.out), args.sr, args.variations, args.seed)
    print(f"Exported {manifest['count']} WAV files and {manifest.get('zip', '')} to {args.out}")


if __name__ == "__main__":
    main()
