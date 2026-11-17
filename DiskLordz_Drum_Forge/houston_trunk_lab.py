#!/usr/bin/env python3
"""DiskLordz Houston Trunk & 12-Bit Vintage Hardware Sampler Drum Factory.

Implements:
- VintageSamplerStage: 26.04 kHz zero-order-hold DAC clock + 12-bit quantization
  + independent stereo 11.5 kHz (Q=0.707) analog reconstruction low-pass filter
- HoustonWarm808Generator: sine + 12% 2nd harmonic, soft-knee tube saturation
  driven / (1 + |0.65 * driven|) + transformer even-order warmth (DC-blocked at 24 Hz HPF + 260 Hz LPF),
  3 ms smooth subwoofer-safe attack ramp
- Dedicated 540/800 Hz band-passed 808 Cowbell generator for Pads 15-16 (fixing the C++ snippet bug
  where cowbells were accidentally routed through the 6.8 kHz high-passed hat generator)
- Complete 16-pad Houston Trunk Kit matrix export + ZIP archive
"""
from __future__ import annotations

import argparse
import json
import zipfile
from dataclasses import asdict, dataclass
from pathlib import Path

import numpy as np
import soundfile as sf

import brick_squad_lab as brick
import forge_core_v2 as core

ENGINE = "DiskLordz HOUSTON TRUNK LAB v1"


@dataclass(frozen=True)
class TrunkPadPreset:
    pad: int
    file_name: str
    engine_type: str  # warm808 | kick | snare | clap | hat | cowbell
    freq_or_pitch: float
    decay_sec: float
    drive: float
    sp1200_mix: float
    extra: float = 0.0  # punchRatio for 808/kick, clapSpreadMs for snare/clap


HOUSTON_16_PAD_MATRIX: tuple[TrunkPadPreset, ...] = (
    TrunkPadPreset(1, "01_808_Screw_Slab_C1.wav", "warm808", 32.7032, 2.40, 2.2, 0.25, 2.1),
    TrunkPadPreset(2, "02_808_MikeDean_Moog_E1.wav", "warm808", 41.2034, 2.10, 2.8, 0.15, 2.1),
    TrunkPadPreset(3, "03_808_UGK_Trunk_F1.wav", "warm808", 43.6535, 1.95, 2.5, 0.30, 2.1),
    TrunkPadPreset(4, "04_808_ZRo_Sub_G1.wav", "warm808", 48.9994, 1.80, 2.1, 0.20, 2.1),
    TrunkPadPreset(5, "05_Kick_MPC3000_Thump.wav", "kick", 55.00, 0.22, 2.6, 0.65, 3.2),
    TrunkPadPreset(6, "06_Kick_SP1200_Round.wav", "kick", 62.00, 0.18, 2.9, 0.85, 2.8),
    TrunkPadPreset(7, "07_Kick_808_Tight.wav", "kick", 52.00, 0.26, 2.2, 0.45, 2.5),
    TrunkPadPreset(8, "08_Snare_Screwed_Low.wav", "snare", 145.00, 0.32, 1.7, 0.75, 11.0),
    TrunkPadPreset(9, "09_Snare_Linn_Fat.wav", "snare", 162.00, 0.28, 1.9, 0.80, 11.0),
    TrunkPadPreset(10, "10_Clap_R8_Wide.wav", "clap", 175.00, 0.35, 1.6, 0.70, 16.0),
    TrunkPadPreset(11, "11_Snare_Clap_Swisha.wav", "clap", 155.00, 0.30, 2.0, 0.85, 14.0),
    TrunkPadPreset(12, "12_Hat_Closed_SP12.wav", "hat", 0.88, 0.065, 1.3, 0.90, 0.0),
    TrunkPadPreset(13, "13_Hat_Closed_Tight.wav", "hat", 0.94, 0.048, 1.2, 0.80, 0.0),
    TrunkPadPreset(14, "14_Hat_Open_808_Wash.wav", "hat", 0.85, 0.480, 1.4, 0.85, 1.0),
    TrunkPadPreset(15, "15_Perc_808_Cowbell_Low.wav", "cowbell", 0.65, 0.240, 1.8, 0.75, 0.0),
    TrunkPadPreset(16, "16_Perc_808_Cowbell_High.wav", "cowbell", 0.78, 0.210, 1.8, 0.75, 0.0),
)


def vintage_sampler_stage(
    x: np.ndarray,
    sr: int,
    wet_mix: float = 1.0,
    target_sr: float = 26040.0,
    bit_depth: int = 12,
    recon_cutoff_hz: float = 11500.0,
) -> np.ndarray:
    """Stateless per-render 26.04 kHz ZOH + 12-bit quantizer + 11.5 kHz reconstruction LPF."""
    wet = float(np.clip(wet_mix, 0.0, 1.0))
    if wet <= 0.0:
        return x.copy()

    dry = np.asarray(x, dtype=np.float64)
    step = max(1.0, sr / float(target_sr))
    quant_levels = float(2 ** (bit_depth - 1))

    sample_idx = np.floor(np.arange(len(dry)) / step).astype(np.int64)
    hold_positions = np.minimum(len(dry) - 1, (sample_idx * step).astype(np.int64))
    held = dry[hold_positions]
    quantized = np.round(held * quant_levels) / quant_levels

    cutoff = min(recon_cutoff_hz, sr * 0.45)
    filtered = core.filter_signal(quantized, cutoff, "lowpass", sr, 2)
    return dry + wet * (filtered - dry)


def gen_houston_warm_808(
    sr: int,
    base_freq: float = 41.2034,
    warmth_drive: float = 2.4,
    decay_sec: float = 2.1,
    punch_ratio: float = 2.1,
) -> np.ndarray:
    """Warm tube/transformer 808 with 3 ms subwoofer-safe attack, 24 Hz HPF, and 260 Hz LPF."""
    num_samples = int((decay_sec + 0.35) * sr)
    t = np.arange(num_samples) / sr

    pitch_offset = base_freq * (punch_ratio - 1.0) * np.exp(-t / 0.024)
    freq = base_freq + pitch_offset
    two_pi_phase = 2.0 * np.pi * np.cumsum(freq) / sr

    core_osc = np.sin(two_pi_phase) + 0.12 * np.sin(two_pi_phase * 2.0)
    driven = core_osc * warmth_drive
    warm_sat = driven / (1.0 + np.abs(driven * 0.65))
    warm_sat = warm_sat + 0.08 * (warm_sat * warm_sat)
    warm_sat = warm_sat - np.mean(warm_sat)

    attack_samples = max(1, int(0.003 * sr))
    attack_gain = np.ones_like(t)
    attack_gain[:attack_samples] = np.linspace(0.0, 1.0, attack_samples, endpoint=False)
    amp_env = np.exp(-t / max(decay_sec, 0.04))

    shaped = warm_sat * amp_env * attack_gain
    hp = core.filter_signal(shaped, 24.0, "highpass", sr, 2)
    lp_cutoff = 420.0 if decay_sec < 0.35 else 260.0
    lp = core.filter_signal(hp, lp_cutoff, "lowpass", sr, 2)
    mono = np.clip(lp * 1.1, -0.94, 0.94)
    return np.column_stack((mono, mono))


def gen_808_bandpass_cowbell(
    sr: int, pitch_scale: float = 0.65, decay_sec: float = 0.24, drive: float = 1.8
) -> np.ndarray:
    """Dedicated 808 dual-oscillator (540/800 Hz * scale) band-passed cowbell for Pads 15-16."""
    t = np.arange(int((decay_sec + 0.22) * sr)) / sr
    f1 = 540.0 * (pitch_scale / 0.65)
    f2 = 800.0 * (pitch_scale / 0.65)
    nyq_limit = sr * 0.45

    sig = np.zeros_like(t)
    for f0, weight in ((f1, 0.74), (f2, 0.42)):
        max_h = min(19, int(nyq_limit // max(f0, 1.0)))
        for h in range(1, max_h + 1, 2):
            sig += weight * (4.0 / np.pi) * (1.0 / h) * np.sin(2.0 * np.pi * (f0 * h) * t)

    bp = core.filter_signal(core.filter_signal(sig, 360.0, "highpass", sr, 2), 3400.0, "lowpass", sr, 2)
    attack_samples = max(1, int(0.0008 * sr))
    attack = np.ones_like(t)
    attack[:attack_samples] = np.linspace(0.0, 1.0, attack_samples, endpoint=False)
    env = (0.65 * np.exp(-t / 0.018) + 0.35 * np.exp(-t / max(decay_sec, 0.05))) * attack
    mono = core.saturate_oversampled(bp * env, drive=drive, mix=0.65, factor=4)
    return np.column_stack((mono, mono))


def render_trunk_pad(preset: TrunkPadPreset, sr: int = 48000, seed: int = 713808) -> np.ndarray:
    rng = np.random.default_rng(seed + preset.pad * 97)
    if preset.engine_type in ("warm808", "kick"):
        buf = gen_houston_warm_808(
            sr,
            base_freq=preset.freq_or_pitch,
            warmth_drive=preset.drive,
            decay_sec=preset.decay_sec,
            punch_ratio=preset.extra,
        )
    elif preset.engine_type in ("snare", "clap"):
        bp = brick.BrickParams(
            snare_tune_hz=preset.freq_or_pitch,
            clap_spread_ms=preset.extra,
            snare_decay_sec=preset.decay_sec,
            snap_drive=preset.drive,
            high_shelf_db=3.5,
        )
        buf, _ = brick.gen_snare_clap_layer(sr, rng, bp)
    elif preset.engine_type == "hat":
        bp = brick.BrickParams(
            hat_pitch_mult=preset.freq_or_pitch,
            hat_decay_sec=preset.decay_sec,
            open_hat_decay_sec=preset.decay_sec,
            hat_grit_drive=preset.drive,
        )
        mono, _ = brick.gen_909_metallic_hat(sr, rng, bp, is_open=(preset.extra > 0.5))
        buf = np.column_stack((mono, mono))
    else:
        buf = gen_808_bandpass_cowbell(sr, pitch_scale=preset.freq_or_pitch, decay_sec=preset.decay_sec, drive=preset.drive)

    gritty = vintage_sampler_stage(buf, sr, wet_mix=preset.sp1200_mix)
    trimmed = brick.trim_below_db(gritty, sr, threshold_db=-70.0)
    return core.master(trimmed, sr, ceiling_db=-0.8)


def export_houston_trunk_kit(
    out_dir: Path, sr: int = 48000, seed: int = 713808, make_zip: bool = True
) -> dict:
    out_dir.mkdir(parents=True, exist_ok=True)
    rows = []
    written: list[Path] = []

    for preset in HOUSTON_16_PAD_MATRIX:
        audio = render_trunk_pad(preset, sr=sr, seed=seed)
        dest = out_dir / preset.file_name
        sf.write(dest, audio, sr, subtype="PCM_24")
        written.append(dest)
        rows.append(
            {
                **asdict(preset),
                "frames": int(len(audio)),
                "true_peak_dbtp": round(core.true_peak_db(audio), 3),
            }
        )

    manifest_path = out_dir / "manifest.json"
    manifest = {"engine": ENGINE, "sample_rate": sr, "seed": seed, "count": len(rows), "pads": rows}
    manifest_path.write_text(json.dumps(manifest, indent=2), encoding="utf-8")

    if make_zip:
        zip_path = out_dir / "DiskLordz_Houston_Trunk_16Pad_Pack.zip"
        with zipfile.ZipFile(zip_path, "w", zipfile.ZIP_DEFLATED) as zf:
            for wav in written:
                zf.write(wav, arcname=wav.name)
            zf.write(manifest_path, arcname="manifest.json")
        manifest["zip"] = zip_path.name
    return manifest


def main() -> None:
    ap = argparse.ArgumentParser(description=ENGINE)
    ap.add_argument("--sr", type=int, choices=(44100, 48000, 96000), default=48000)
    ap.add_argument("--seed", type=int, default=713808)
    ap.add_argument("--out", default="DiskLordz_Houston_Trunk_Kit")
    args = ap.parse_args()
    manifest = export_houston_trunk_kit(Path(args.out), args.sr, args.seed)
    print(f"Exported {manifest['count']} Houston Trunk WAV pads and {manifest.get('zip', '')} to {args.out}")


if __name__ == "__main__":
    main()
