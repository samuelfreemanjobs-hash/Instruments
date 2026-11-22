#!/usr/bin/env python3
"""DiskLordz Chicago Drill 16-Pad & Groove Factory (ChicagoDrillDSP v2).

Implements the exact 16-pad South Side matrix with two critical DSP fixes over the raw snippet:
1. Buffer length extends to >= 4.5 * decaySec (+ -70 dBFS silence trim & micro-fade) instead of
   (decaySec + 0.15) * sr, which would truncate an exp(-t/decaySec) tail at -9.5 dBFS and click.
2. Per-render filter reset + 4x oversampled symmetric tanh + band-limited 6-oscillator crash core
   (218, 319, 384, 548, 572, 845 Hz) through independent stereo 3.6 kHz HPF & 8.8 kHz (+4.5 dB) shimmer.
"""
from __future__ import annotations

import argparse
import json
import struct
import zipfile
from dataclasses import asdict, dataclass
from pathlib import Path

import numpy as np
import soundfile as sf
from scipy.signal import lfilter, resample_poly

import brick_squad_lab as brick
import forge_core_v2 as core
import pierre_bounce_lab as bounce

ENGINE = "DiskLordz CHICAGO DRILL LAB v2"
TPQ = 480


@dataclass(frozen=True)
class DrillPadPreset:
    pad: int
    file_name: str
    engine_type: int  # 0 = Drill808/Kick, 1 = DrySnareClap, 2 = ClosedHat, 3 = Crash/OpenHat
    freq_or_pitch: float
    decay_sec: float
    drive_or_width: float
    extra_param: float  # glideSemitones for engine 0, clapSpreadMs for engine 1
    glide_start_ratio: float = 0.48


CHICAGO_16_PAD_MATRIX: tuple[DrillPadPreset, ...] = (
    DrillPadPreset(1, "01_808_Chop_ElChopo_F1.wav", 0, 43.6535, 1.55, 3.4, 0.0, 0.50),
    DrillPadPreset(2, "02_808_Spinz_SouthSide_G1.wav", 0, 48.9994, 1.40, 3.1, 0.0, 0.50),
    DrillPadPreset(3, "03_808_Durk_OctaveSlide_G1.wav", 0, 48.9994, 1.35, 3.3, 12.0, 0.50),
    DrillPadPreset(4, "04_808_Kenn_FifthSlide_E1.wav", 0, 41.2034, 1.50, 3.6, 7.0, 0.45),
    DrillPadPreset(5, "05_Kick_Goat_Punch.wav", 0, 64.00, 0.17, 4.2, -12.0, 0.05),
    DrillPadPreset(6, "06_Kick_YoungChop_Thump.wav", 0, 58.00, 0.20, 3.9, -10.0, 0.05),
    DrillPadPreset(7, "07_Kick_Luger_Clip.wav", 0, 68.00, 0.15, 4.5, -14.0, 0.04),
    DrillPadPreset(8, "08_Snare_Chop_DryCrack.wav", 1, 198.0, 0.18, 2.4, 6.5),
    DrillPadPreset(9, "09_Snare_GBE_Sharp.wav", 1, 215.0, 0.16, 2.6, 5.0),
    DrillPadPreset(10, "10_Clap_SouthSide_Tight.wav", 1, 188.0, 0.21, 2.1, 9.0),
    DrillPadPreset(11, "11_Snare_Roll_Ghost.wav", 1, 225.0, 0.12, 1.9, 4.0),
    DrillPadPreset(12, "12_Hat_Closed_TwoStep.wav", 2, 1.06, 0.042, 1.6, 0.0),
    DrillPadPreset(13, "13_Hat_Closed_Triplet.wav", 2, 1.14, 0.035, 1.7, 0.0),
    DrillPadPreset(14, "14_Hat_Open_Bounce1.wav", 3, 1.18, 0.260, 0.25, 0.0),
    DrillPadPreset(15, "15_Hat_Open_Bounce2.wav", 3, 0.98, 0.320, 0.35, 0.0),
    DrillPadPreset(16, "16_Crash_Downbeat_Drill.wav", 3, 1.00, 1.050, 0.55, 0.0),
)


def _oversampled_symmetric_clip(x: np.ndarray, drive: float, post_gain: float = 1.15, ceiling: float = 0.96) -> np.ndarray:
    up = resample_poly(x, 4, 1, axis=0)
    sat = np.tanh(up * drive) * post_gain
    clipped = np.clip(sat, -ceiling, ceiling)
    down = resample_poly(clipped, 1, 4, axis=0)[: len(x)]
    if len(down) < len(x):
        pad_shape = ((0, len(x) - len(down)), (0, 0)) if x.ndim == 2 else (0, len(x) - len(down))
        down = np.pad(down, pad_shape)
    return down - np.mean(down, axis=0)


def gen_chicago_drill_808(
    sr: int,
    base_freq: float = 49.0,
    drive: float = 3.2,
    decay_sec: float = 1.45,
    glide_semitones: float = 0.0,
    glide_start_ratio: float = 0.50,
) -> np.ndarray:
    """ChicagoDrill808Generator: 2.8x 26 ms beater drop, 35 ms smooth portamento glide, 28 Hz HPF + 140 Hz (+3 dB) peak."""
    # Musical note span uses (decay_sec + 0.15) as reference for glide_start_ratio, while extending render tail cleanly
    musical_samples = int((decay_sec + 0.15) * sr)
    total_samples = int(max(musical_samples * 1.65, decay_sec * 3.2 * sr))
    t = np.arange(total_samples) / sr

    punch_offset = (base_freq * 1.8) * np.exp(-t / 0.026)
    glide_start_sample = int(musical_samples * float(np.clip(glide_start_ratio, 0.0, 0.95)))
    target_freq = base_freq * (2.0 ** (glide_semitones / 12.0))

    base_curve = np.full(total_samples, base_freq, dtype=np.float64)
    if glide_semitones != 0.0 and glide_start_sample < total_samples:
        smooth = 1.0 - np.exp(-1.0 / (0.035 * sr))
        step_target = np.full(total_samples - glide_start_sample, target_freq - base_freq, dtype=np.float64)
        glided = lfilter([smooth], [1.0, -(1.0 - smooth)], step_target)
        base_curve[glide_start_sample:] = base_freq + glided

    freq = base_curve + punch_offset
    phase = np.mod(np.cumsum(freq) / sr, 1.0)
    two_pi_phase = phase * (2.0 * np.pi)

    sine_osc = np.sin(two_pi_phase)
    tri_osc = 2.0 * np.abs(2.0 * (phase - np.floor(phase + 0.5))) - 1.0
    core_sig = 0.88 * sine_osc + 0.12 * tri_osc

    attack_samples = max(1, int(0.002 * sr))
    attack_gain = np.ones(total_samples, dtype=np.float64)
    attack_gain[:attack_samples] = np.linspace(0.0, 1.0, attack_samples, endpoint=False)
    amp_env = np.exp(-t / max(decay_sec, 0.03)) * attack_gain

    saturated = _oversampled_symmetric_clip(core_sig, drive=drive, post_gain=1.0, ceiling=0.99) * amp_env
    hp = core.filter_signal(saturated, 28.0, "highpass", sr, 2)
    peaked = bounce.peaking_eq(hp, sr, 140.0, 1.1, brick.db_to_gain(3.0))
    mono = np.clip(peaked * 1.15, -0.96, 0.96)
    return np.column_stack((mono, mono))


def gen_chicago_crash(
    sr: int,
    rng: np.random.Generator,
    pitch_mult: float = 1.0,
    decay_sec: float = 0.95,
    stereo_width: float = 0.45,
) -> np.ndarray:
    """ChicagoCrashGenerator: 6-oscillator (218..845 Hz) metallic core + stereo noise -> 3.6 kHz HPF -> 8.8 kHz (+4.5 dB)."""
    total_samples = int(max(0.45, decay_sec * 4.5) * sr)
    t = np.arange(total_samples) / sr

    freqs = (218.0, 319.0, 384.0, 548.0, 572.0, 845.0)
    phases = (0.0, 0.19, 0.41, 0.63, 0.77, 0.91)
    nyq_limit = sr * 0.45
    metallic = np.zeros_like(t)
    for f0, ph0 in zip(freqs, phases):
        freq = f0 * pitch_mult
        max_h = min(31, int(nyq_limit // max(freq, 1.0)))
        for h in range(1, max_h + 1, 2):
            metallic += (4.0 / np.pi) * (1.0 / h) * np.sin(2.0 * np.pi * (freq * h * t + h * ph0))
    metallic /= 6.0

    noise_scale = 0.45 + stereo_width * 0.2
    noise_l = rng.uniform(-1.0, 1.0, total_samples) * noise_scale
    noise_r = rng.uniform(-1.0, 1.0, total_samples) * noise_scale

    attack_samples = max(1, int(0.004 * sr))
    attack_gain = np.ones_like(t)
    attack_gain[:attack_samples] = np.linspace(0.0, 1.0, attack_samples, endpoint=False)
    env = np.exp(-t / max(decay_sec, 0.04)) * attack_gain

    raw_stereo = np.column_stack(((metallic * 0.55 + noise_l) * env, (metallic * 0.55 + noise_r) * env))
    hp = core.filter_signal(raw_stereo, 3600.0, "highpass", sr, 2)
    shimmer = brick.peaking_eq_stereo(hp, sr, 8800.0, 1.1, brick.db_to_gain(4.5))
    return _oversampled_symmetric_clip(shimmer, drive=1.4, post_gain=1.0, ceiling=0.95)


def render_drill_pad(preset: DrillPadPreset, sr: int = 48000, seed: int = 312808) -> np.ndarray:
    rng = np.random.default_rng(seed + preset.pad * 83)
    if preset.engine_type == 0:
        raw = gen_chicago_drill_808(
            sr,
            base_freq=preset.freq_or_pitch,
            drive=preset.drive_or_width,
            decay_sec=preset.decay_sec,
            glide_semitones=preset.extra_param,
            glide_start_ratio=preset.glide_start_ratio,
        )
    elif preset.engine_type == 1:
        bp = brick.BrickParams(
            snare_tune_hz=preset.freq_or_pitch,
            clap_spread_ms=preset.extra_param,
            snare_decay_sec=preset.decay_sec,
            snap_drive=preset.drive_or_width,
            high_shelf_db=4.5,
        )
        raw, _ = brick.gen_snare_clap_layer(sr, rng, bp)
        # Emphasize 3.2 kHz dry Chop crack
        raw = brick.peaking_eq_stereo(raw, sr, 3200.0, 1.3, brick.db_to_gain(3.0))
    elif preset.engine_type == 2:
        bp = brick.BrickParams(
            hat_pitch_mult=preset.freq_or_pitch,
            hat_decay_sec=preset.decay_sec,
            hat_grit_drive=preset.drive_or_width,
        )
        mono, _ = brick.gen_909_metallic_hat(sr, rng, bp, is_open=False)
        raw = np.column_stack((mono, mono))
    else:
        raw = gen_chicago_crash(
            sr,
            rng,
            pitch_mult=preset.freq_or_pitch,
            decay_sec=preset.decay_sec,
            stereo_width=preset.drive_or_width,
        )

    trimmed = brick.trim_below_db(raw, sr, threshold_db=-70.0)
    return core.master(trimmed, sr, ceiling_db=-0.8)


def _vlq(value: int) -> bytes:
    buf = bytearray([value & 0x7F])
    value >>= 7
    while value:
        buf.append((value & 0x7F) | 0x80)
        value >>= 7
    return bytes(reversed(buf))


def build_chicago_drill_groove(root_midi: int = 31) -> list[tuple[int, int, int, int, int]]:
    step_ticks = TPQ // 4
    events: list[tuple[int, int, int, int, int]] = [(0, step_ticks * 8, 49, 118, 9)]
    for st in (8, 24):
        events.append((st * step_ticks, 75, 38, 120, 9))
        events.append((st * step_ticks, 70, 39, 110, 9))
    for r_idx, st in enumerate((29, 30, 31)):
        events.append((st * step_ticks, 55, 38, 68 + r_idx * 14, 9))
    for st in (0, 3, 6, 10, 16, 19, 22, 26):
        events.append((st * step_ticks, 70, 36, 122 if st in (0, 16) else 108, 9))
    for st, pitch, dur in ((0, root_midi, 6), (6, root_midi, 4), (10, root_midi - 2, 6), (16, root_midi, 6), (22, root_midi + 12, 4), (26, root_midi, 6)):
        events.append((st * step_ticks, step_ticks * dur, pitch, 118, 0))
    triplet_span = 2 * step_ticks
    for st in range(0, 32, 2):
        if st in (12, 28):
            for tr in range(3):
                events.append((st * step_ticks + (triplet_span * tr) // 3, 45, 42, 86 + tr * 10, 9))
        else:
            events.append((st * step_ticks, 48, 42, 104 if st % 4 == 0 else 82, 9))
    events.extend([(6 * step_ticks, 110, 46, 98, 9), (14 * step_ticks, 110, 44, 92, 9), (22 * step_ticks, 110, 46, 100, 9)])
    return events


def write_midi(path: Path, events: list[tuple[int, int, int, int, int]], bpm: float = 134.0) -> None:
    tempo = int(60_000_000 / bpm)
    msgs = []
    for start, dur, note, vel, ch in events:
        msgs.append((max(0, int(start)), 1, 0x90 | (ch & 0x0F), note & 0x7F, vel & 0x7F))
        msgs.append((max(0, int(start)) + max(1, int(dur)), 0, 0x80 | (ch & 0x0F), note & 0x7F, 0))
    msgs.sort()
    track = bytearray(b"\x00\xff\x51\x03" + struct.pack(">I", tempo)[1:])
    last = 0
    for tick, _prio, status, note, vel in msgs:
        track += _vlq(tick - last) + bytes((status, note, vel))
        last = tick
    track += b"\x00\xff\x2f\x00"
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(b"MThd" + struct.pack(">IHHH", 6, 0, 1, TPQ) + b"MTrk" + struct.pack(">I", len(track)) + track)


def export_chicago_drill_kit(
    out_dir: Path, sr: int = 48000, seed: int = 312808, bpm: float = 134.0, make_zip: bool = True
) -> dict:
    out_dir.mkdir(parents=True, exist_ok=True)
    rows = []
    written: list[Path] = []

    for preset in CHICAGO_16_PAD_MATRIX:
        audio = render_drill_pad(preset, sr=sr, seed=seed)
        dest = out_dir / preset.file_name
        sf.write(dest, audio, sr, subtype="PCM_24")
        written.append(dest)
        rows.append({**asdict(preset), "frames": int(len(audio)), "true_peak_dbtp": round(core.true_peak_db(audio), 3)})

    midi_path = out_dir / "17_SouthSide_Drill_134BPM.mid"
    write_midi(midi_path, build_chicago_drill_groove(root_midi=31), bpm=bpm)
    written.append(midi_path)

    manifest_path = out_dir / "manifest.json"
    manifest = {
        "engine": ENGINE,
        "sample_rate": sr,
        "bpm": bpm,
        "seed": seed,
        "count": len(rows),
        "midi": midi_path.name,
        "pads": rows,
    }
    manifest_path.write_text(json.dumps(manifest, indent=2), encoding="utf-8")

    if make_zip:
        zip_path = out_dir / "DiskLordz_Chicago_Drill_16Pad_Pack.zip"
        with zipfile.ZipFile(zip_path, "w", zipfile.ZIP_DEFLATED) as zf:
            for item in written:
                zf.write(item, arcname=item.name)
            zf.write(manifest_path, arcname="manifest.json")
        manifest["zip"] = zip_path.name
    return manifest


def main() -> None:
    ap = argparse.ArgumentParser(description=ENGINE)
    ap.add_argument("--sr", type=int, choices=(44100, 48000, 96000), default=48000)
    ap.add_argument("--bpm", type=float, default=134.0)
    ap.add_argument("--seed", type=int, default=312808)
    ap.add_argument("--out", default="DiskLordz_Chicago_Drill_Kit")
    args = ap.parse_args()
    manifest = export_chicago_drill_kit(Path(args.out), args.sr, args.seed, args.bpm)
    print(f"Exported {manifest['count']} Chicago Drill pads + {manifest['midi']} to {args.out}")


if __name__ == "__main__":
    main()
