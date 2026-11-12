#!/usr/bin/env python3
"""Spectral + envelope metrics for Trap Lord headless WAV QA."""

from __future__ import annotations

import csv
import math
import struct
import wave
from dataclasses import dataclass
from pathlib import Path

import numpy as np

SAMPLE_RATE_DEFAULT = 44100

CROSS_CATEGORY_CORR_MAX = 0.96

# Factory / audition audible gate: peak must exceed this linear level (~-30 dBFS).
AUDITION_PEAK_MIN_DBFS = -30.0
AUDITION_PEAK_MIN_LINEAR = 10.0 ** (AUDITION_PEAK_MIN_DBFS / 20.0)
MIDI_C3 = 48
MIDI_C4 = 60
AUDIBLE_CHECK_NOTES_DEFAULT = (MIDI_C3, MIDI_C4)
AUDIBLE_CHECK_NOTES_BY_CATEGORY: dict[str, tuple[int, ...]] = {
    "Bass": (36, 48),
    "Pads": (48, 60),
    "Swells": (48, 60),
}

CATEGORY_SPECTRAL_RULES: dict[str, dict[str, float]] = {
    "Leads": {"sub30_max": 0.12, "fund_min": 120.0, "fund_max": 5200.0, "centroid_min": 220.0},
    "Church Keys/EP": {"sub30_max": 0.09, "bright5k_max": 0.38, "centroid_min": 180.0},
    "Bells": {"centroid_min": 650.0, "sub30_max": 0.12, "fund_min": 400.0},
    "Hits/FX": {"centroid_min": 350.0, "sub30_max": 0.22},
    "Bass": {"sub30_min": 0.04, "centroid_max": 2200.0},
    "Pads": {"sub30_max": 0.35, "centroid_min": 80.0},
    "Swells": {"sub30_max": 0.35, "centroid_min": 80.0},
}

MPC_PROGRAM_PEAK_MIN = 0.84  # program loudest note ~ -1 dBFS after offline normalize
MPC_PROGRAM_PEAK_MAX = 0.95
MPC_NOTE_LEVEL_SPREAD_DB_MAX = 12.0


def read_wav_stereo(path: Path) -> tuple[np.ndarray, np.ndarray, int]:
    path = Path(path)
    with wave.open(str(path), "rb") as wf:
        ch = wf.getnchannels()
        sw = wf.getsampwidth()
        sr = wf.getframerate()
        n = wf.getnframes()
        raw = wf.readframes(n)

    def decode_frame(off: int) -> list[float]:
        frame: list[float] = []
        for c in range(ch):
            base = off + c * sw
            if sw == 3:
                b = raw[base : base + 3]
                v = struct.unpack("<i", b + b"\x00")[0]
                if v >= 1 << 23:
                    v -= 1 << 24
                frame.append(v / float(1 << 23))
            elif sw == 2:
                v = struct.unpack("<h", raw[base : base + 2])[0]
                frame.append(v / 32768.0)
            else:
                v = struct.unpack("<f", raw[base : base + 4])[0]
                frame.append(float(v))
        return frame

    step = ch * sw
    left: list[float] = []
    right: list[float] = []
    for i in range(0, len(raw), step):
        frame = decode_frame(i)
        if ch == 1:
            left.append(frame[0])
            right.append(frame[0])
        else:
            left.append(frame[0])
            right.append(frame[1] if len(frame) > 1 else frame[0])
    return np.array(left, dtype=np.float64), np.array(right, dtype=np.float64), sr


def read_wav_mono(path: Path) -> tuple[np.ndarray, int]:
    path = Path(path)
    with wave.open(str(path), "rb") as wf:
        ch = wf.getnchannels()
        sw = wf.getsampwidth()
        sr = wf.getframerate()
        n = wf.getnframes()
        raw = wf.readframes(n)

    if sw == 3:
        samples = []
        step = ch * 3
        for i in range(0, len(raw), step):
            frame = []
            for c in range(ch):
                off = i + c * 3
                b = raw[off : off + 3]
                v = struct.unpack("<i", b + b"\x00")[0]
                if v >= 1 << 23:
                    v -= 1 << 24
                frame.append(v / float(1 << 23))
            samples.append(float(np.mean(frame)))
        x = np.array(samples, dtype=np.float64)
    elif sw == 2:
        ints = np.frombuffer(raw, dtype=np.int16).astype(np.float64)
        if ch > 1:
            ints = ints.reshape(-1, ch).mean(axis=1)
        x = ints / 32768.0
    elif sw == 4:
        floats = np.frombuffer(raw, dtype="<f4")
        if ch > 1:
            floats = floats.reshape(-1, ch).mean(axis=1)
        x = floats.astype(np.float64)
    else:
        raise ValueError(f"Unsupported sample width {sw} in {path}")

    return x, sr


@dataclass
class AudioMetrics:
    path: str
    peak: float
    rms: float
    dc_offset: float
    tail_rms_db: float
    centroid_hz: float
    fundamental_hz: float
    fundamental_ratio: float
    sub30_energy_ratio: float = 0.0
    bright5k_energy_ratio: float = 0.0
    category: str = ""


def envelope_rms(x: np.ndarray, win: int) -> np.ndarray:
    if win < 8:
        win = 8
    kernel = np.ones(win, dtype=np.float64) / win
    return np.sqrt(np.convolve(x * x, kernel, mode="same"))


def spectral_features(x: np.ndarray, sr: int) -> tuple[float, float, float]:
    if len(x) < 512:
        return 0.0, 0.0, 0.0
    w = np.hanning(len(x))
    spec = np.abs(np.fft.rfft(x * w))
    freqs = np.fft.rfftfreq(len(x), 1.0 / sr)
    power = spec * spec
    total = float(np.sum(power))
    if total <= 1e-20:
        return 0.0, 0.0, 0.0
    centroid = float(np.sum(freqs * power) / total)
    band = (freqs >= 90.0) & (freqs <= 1200.0)
    if not np.any(band):
        band = freqs > 1.0
    band_spec = spec.copy()
    band_spec[~band] = 0.0
    peak_bin = int(np.argmax(band_spec))
    fundamental = float(freqs[peak_bin]) if peak_bin < len(freqs) else 0.0
    fundamental_ratio = float(spec[peak_bin] / max(np.max(spec), 1e-12))
    return centroid, fundamental, fundamental_ratio


def energy_ratio_above_hz(x: np.ndarray, sr: int, hz: float) -> float:
    w = np.hanning(len(x))
    spec = np.abs(np.fft.rfft(x * w))
    freqs = np.fft.rfftfreq(len(x), 1.0 / sr)
    power = spec * spec
    total = float(np.sum(power)) + 1e-20
    return float(np.sum(power[freqs >= hz])) / total


def analyze_file(path: Path, category: str = "", tail_fraction: float = 0.12) -> AudioMetrics:
    x, sr = read_wav_mono(path)
    if not np.all(np.isfinite(x)):
        raise ValueError(f"non-finite samples in {path}")

    peak = float(np.max(np.abs(x)))
    rms = float(np.sqrt(np.mean(x * x)))
    dc = float(np.mean(x))
    tail_start = int(len(x) * (1.0 - tail_fraction))
    tail = x[tail_start:]
    tail_rms = float(np.sqrt(np.mean(tail * tail)) if len(tail) else 0.0)
    tail_rms_db = 20.0 * math.log10(max(tail_rms, 1e-12))

    centroid, fundamental, fund_ratio = spectral_features(x, sr)

    w = np.hanning(len(x))
    spec = np.abs(np.fft.rfft(x * w))
    freqs = np.fft.rfftfreq(len(x), 1.0 / sr)
    power = spec * spec
    total_p = float(np.sum(power)) + 1e-20
    sub30 = float(np.sum(power[freqs < 30.0]))
    sub30_ratio = sub30 / total_p
    bright5k = energy_ratio_above_hz(x, sr, 5000.0)

    return AudioMetrics(
        path=path.name,
        peak=peak,
        rms=rms,
        dc_offset=dc,
        tail_rms_db=tail_rms_db,
        centroid_hz=centroid,
        fundamental_hz=fundamental,
        fundamental_ratio=fund_ratio,
        sub30_energy_ratio=sub30_ratio,
        bright5k_energy_ratio=bright5k,
        category=category,
    )


def category_spectral_passes(metrics: AudioMetrics, category: str) -> tuple[bool, str]:
    rules = CATEGORY_SPECTRAL_RULES.get(category)
    if not rules:
        return True, ""
    if "sub30_max" in rules and metrics.sub30_energy_ratio > rules["sub30_max"]:
        return False, f"sub30 {metrics.sub30_energy_ratio:.3f} > {rules['sub30_max']}"
    if "sub30_min" in rules and metrics.sub30_energy_ratio < rules["sub30_min"]:
        return False, f"sub30 {metrics.sub30_energy_ratio:.3f} < {rules['sub30_min']}"
    if "bright5k_max" in rules and metrics.bright5k_energy_ratio > rules["bright5k_max"]:
        return False, f"bright5k {metrics.bright5k_energy_ratio:.3f} > {rules['bright5k_max']}"
    if "fund_min" in rules and 0 < metrics.fundamental_hz < rules["fund_min"]:
        return False, f"fundamental {metrics.fundamental_hz:.1f} < {rules['fund_min']}"
    if "fund_max" in rules and metrics.fundamental_hz > rules["fund_max"]:
        return False, f"fundamental {metrics.fundamental_hz:.1f} > {rules['fund_max']}"
    if "centroid_min" in rules and metrics.centroid_hz < rules["centroid_min"]:
        return False, f"centroid {metrics.centroid_hz:.1f} < {rules['centroid_min']}"
    if "centroid_max" in rules and metrics.centroid_hz > rules["centroid_max"]:
        return False, f"centroid {metrics.centroid_hz:.1f} > {rules['centroid_max']}"
    return True, ""


def mpc_note_peak_levels(program_folder: Path) -> dict[int, float]:
    """Peak abs per MIDI note (max L/R) for MPC program WAVs."""
    import re

    note_re = re.compile(r"DL_N(\d{3})_L\d+\.WAV", re.I)
    levels: dict[int, float] = {}
    wavs = sorted(
        {p for p in program_folder.rglob("*") if p.is_file() and p.suffix.lower() == ".wav"}
    )
    for wav in wavs:
        m = note_re.match(wav.name)
        if not m:
            continue
        note = int(m.group(1))
        left, right, _ = read_wav_stereo(wav)
        levels[note] = max(float(np.max(np.abs(left))), float(np.max(np.abs(right))))
    return levels


def mpc_program_level_spread_db(program_folder: Path) -> tuple[float, dict[int, float]]:
    levels = mpc_note_peak_levels(program_folder)
    if not levels:
        return 999.0, levels
    top = max(levels.values())
    if top < 1e-6:
        return 999.0, levels
    worst_db = 0.0
    for peak in levels.values():
        if peak < 1e-8:
            db = -120.0
        else:
            db = 20.0 * math.log10(peak / top)
        worst_db = min(worst_db, db)
    return float(-worst_db), levels


def mpc_program_levels_ok(
    program_folder: Path, max_spread_db: float = MPC_NOTE_LEVEL_SPREAD_DB_MAX
) -> tuple[bool, float, str]:
    spread, levels = mpc_program_level_spread_db(program_folder)
    ok = spread <= max_spread_db
    worst_note = -1
    if levels:
        top = max(levels.values())
        for n, p in levels.items():
            if p < 1e-8 or 20.0 * math.log10(p / top) < -max_spread_db:
                worst_note = n
                break
    msg = f"spread {spread:.1f} dB worst_note={worst_note}"
    return ok, spread, msg


def mpc_program_peak_ok(program_folder: Path) -> tuple[bool, float, int]:
    """Return (ok, worst_peak, bits) for MPC program WAVs (expect 24-bit, ~-1 dBFS peak)."""
    wavs = sorted(
        {p for p in program_folder.rglob("*") if p.is_file() and p.suffix.lower() == ".wav"}
    )
    if not wavs:
        return False, 0.0, 0
    worst = 0.0
    bits = 0
    for wav in wavs:
        with wave.open(str(wav), "rb") as wf:
            bits = wf.getsampwidth()
            if wf.getsampwidth() != 3:
                return False, 0.0, wf.getsampwidth()
        left, right, _ = read_wav_stereo(wav)
        peak = max(float(np.max(np.abs(left))), float(np.max(np.abs(right))))
        worst = max(worst, peak)
    ok = MPC_PROGRAM_PEAK_MIN <= worst <= MPC_PROGRAM_PEAK_MAX
    return ok, worst, bits


def assert_wav_format(path: Path, sample_rate: int = 44100, sample_width_bytes: int = 3) -> None:
    path = Path(path)
    with wave.open(str(path), "rb") as wf:
        if wf.getframerate() != sample_rate:
            raise AssertionError(f"{path.name}: sample rate {wf.getframerate()} != {sample_rate}")
        if wf.getsampwidth() != sample_width_bytes:
            raise AssertionError(f"{path.name}: width {wf.getsampwidth()} != {sample_width_bytes}")


def estimate_attack_release_sec(x: np.ndarray, sr: int) -> tuple[float, float]:
    env = envelope_rms(x, max(8, sr // 120))
    peak = float(np.max(env)) + 1e-12
    attack_ix = int(np.argmax(env >= 0.9 * peak) if np.any(env >= 0.9 * peak) else np.argmax(env))
    attack_s = attack_ix / sr
    tail = env[int(len(env) * 0.75) :]
    release_ix = 0
    for i, v in enumerate(tail):
        if v < peak * 0.12:
            release_ix = i
            break
    release_s = max(0.05, release_ix / sr)
    return attack_s, release_s


def brightness_ratio(x: np.ndarray, sr: int) -> float:
    w = np.hanning(len(x))
    spec = np.abs(np.fft.rfft(x * w))
    freqs = np.fft.rfftfreq(len(x), 1.0 / sr)
    power = spec * spec
    total = float(np.sum(power)) + 1e-20
    bright = float(np.sum(power[freqs >= 2000.0]))
    return bright / total


def stereo_width_lr(left: np.ndarray, right: np.ndarray) -> float:
    mid = (left + right) * 0.5
    side = (left - right) * 0.5
    mid_e = float(np.mean(mid * mid)) + 1e-12
    side_e = float(np.mean(side * side))
    return side_e / (mid_e + side_e)


def vibrato_mod_index(x: np.ndarray, sr: int) -> float:
    """Rough AM energy in 3–7 Hz band (proxy for vibrato / movement)."""
    if len(x) < sr:
        return 0.0
    env = envelope_rms(x, max(8, sr // 200))
    env = env - np.mean(env)
    if len(env) < 64:
        return 0.0
    spec = np.abs(np.fft.rfft(env * np.hanning(len(env))))
    freqs = np.fft.rfftfreq(len(env), 1.0 / sr)
    band = (freqs >= 3.0) & (freqs <= 7.0)
    return float(np.sum(spec[band])) / (float(np.sum(spec)) + 1e-12)


def centroid_trajectory(x: np.ndarray, sr: int, frames: int = 48) -> np.ndarray:
    hop = max(sr // 40, 256)
    out: list[float] = []
    for start in range(0, len(x) - hop, hop):
        chunk = x[start : start + hop * 3]
        c, _, _ = spectral_features(chunk, sr)
        out.append(c)
        if len(out) >= frames:
            break
    arr = np.array(out, dtype=np.float64)
    if arr.size == 0:
        return np.zeros(frames, dtype=np.float64)
    arr = arr[:frames]
    if arr.size < frames:
        arr = np.pad(arr, (0, frames - arr.size))
    denom = float(np.max(arr)) + 1e-9
    return arr / denom


def log_band_energies(x: np.ndarray, sr: int, bands: int = 32) -> np.ndarray:
    w = np.hanning(len(x))
    spec = np.abs(np.fft.rfft(x * w))
    freqs = np.fft.rfftfreq(len(x), 1.0 / sr)
    edges = np.logspace(np.log10(40.0), np.log10(min(16000.0, sr * 0.45)), num=bands + 1)
    out = np.zeros(bands, dtype=np.float64)
    for i in range(bands):
        mask = (freqs >= edges[i]) & (freqs < edges[i + 1])
        out[i] = float(np.sum(spec[mask] ** 2))
    out = np.log1p(out)
    return out / (np.linalg.norm(out) + 1e-12)


def peak_passes_audible_gate(peak: float, min_linear: float = AUDITION_PEAK_MIN_LINEAR) -> bool:
    return peak >= min_linear


def audible_notes_for_category(category: str) -> tuple[int, ...]:
    return AUDIBLE_CHECK_NOTES_BY_CATEGORY.get(category, AUDIBLE_CHECK_NOTES_DEFAULT)


def preset_audible_at_notes(
    cli: Path,
    root: Path,
    preset_index: int,
    scratch_dir: Path,
    notes: tuple[int, ...] | None = None,
    category: str = "",
) -> tuple[bool, float, str]:
    """Render held notes (default C3/C4) and require peak above AUDITION_PEAK_MIN_LINEAR."""
    import subprocess
    import tempfile

    scratch_dir.mkdir(parents=True, exist_ok=True)
    note_list = notes or audible_notes_for_category(category)
    best = 0.0
    for note in note_list:
        wav = scratch_dir / f"aud_{preset_index:04d}_n{note}.wav"
        subprocess.check_call(
            [str(cli), "test-note", str(note), str(wav), str(preset_index)],
            cwd=root,
        )
        pk = analyze_file(wav).peak
        best = max(best, pk)
    if not peak_passes_audible_gate(best):
        db = 20.0 * math.log10(max(best, 1e-12))
        return False, best, f"note peak {db:.1f} dBFS < {AUDITION_PEAK_MIN_DBFS:.0f} dBFS"
    return True, best, "ok"


def category_feature_vector(path: Path) -> np.ndarray:
    """Features for within-category diversity gate (category phrase renders)."""
    left, right, sr = read_wav_stereo(path)
    mono = (left + right) * 0.5
    cent = centroid_trajectory(mono, sr, frames=32)
    cent_mean = float(np.mean(cent))
    cent_std = float(np.std(cent))

    attack, release = estimate_attack_release_sec(mono, sr)
    timing = np.array(
        [
            attack,
            release,
            brightness_ratio(mono, sr),
            stereo_width_lr(left, right),
            vibrato_mod_index(mono, sr),
            cent_mean,
            cent_std,
        ]
    )

    bands = log_band_energies(mono, sr, bands=40)
    vec = np.concatenate([bands, cent, timing])
    vec = vec / (np.linalg.norm(vec) + 1e-12)
    return vec


def preset_param_vector(p: dict) -> np.ndarray:
    def f(k: str, default: float = 0.0) -> float:
        v = p.get(k, default)
        if isinstance(v, bool):
            return 1.0 if v else 0.0
        return float(v)

    raw = np.array(
        [
            f("engine") / 24.0,
            f("filterType") / 5.0,
            f("cutoff") / 16000.0,
            f("detune") / 24.0,
            f("drive") / 4.5,
            f("resonance"),
            f("attack") / 3.0,
            f("release") / 3.0,
            f("macroDarkness"),
            f("macroDirt"),
            f("macroMovement"),
            f("macroSpace"),
            f("swellDepth"),
            f("chord") / 5.0,
            1.0 if p.get("arp") else 0.0,
            f("reverb"),
            f("delay"),
            f("pulseWidth"),
            f("vibratoDepth"),
        ],
        dtype=np.float64,
    )
    return raw / (np.linalg.norm(raw) + 1e-12)


def gate_feature_vector(wav_path: Path, preset: dict | None = None) -> np.ndarray:
    """Within-category gate: audio-only on category phrase renders (corr ≤ 0.92)."""
    del preset
    return category_feature_vector(wav_path)


def feature_correlation(a: Path | np.ndarray, b: Path | np.ndarray) -> float:
    if isinstance(a, Path):
        a = category_feature_vector(a)
    if isinstance(b, Path):
        b = category_feature_vector(b)
    return float(np.dot(a, b))


def feature_vector(x: np.ndarray, sr: int) -> np.ndarray:
    env = envelope_rms(x, max(1, sr // 100))
    env = env[:: max(1, len(env) // 256)]
    w = np.hanning(len(x))
    spec = np.abs(np.fft.rfft(x * w))
    spec = spec[:512]
    vec = np.concatenate([spec, env])
    vec = vec / (np.linalg.norm(vec) + 1e-12)
    return vec


def correlation(a: Path, b: Path) -> float:
    xa, sra = read_wav_mono(a)
    xb, srb = read_wav_mono(b)
    if sra != srb:
        raise ValueError("sample rate mismatch")
    n = min(len(xa), len(xb))
    fa = feature_vector(xa[:n], sra)
    fb = feature_vector(xb[:n], srb)
    return float(np.dot(fa, fb))


def write_metrics_csv(rows: list[AudioMetrics], out: Path) -> None:
    out = Path(out)
    with out.open("w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        w.writerow(
            [
                "file",
                "category",
                "peak",
                "rms",
                "dc_offset",
                "tail_rms_dbfs",
                "centroid_hz",
                "fundamental_hz",
                "fundamental_ratio",
                "sub30_energy_ratio",
                "bright5k_energy_ratio",
            ]
        )
        for m in rows:
            w.writerow(
                [
                    m.path,
                    m.category,
                    f"{m.peak:.6f}",
                    f"{m.rms:.6f}",
                    f"{m.dc_offset:.6f}",
                    f"{m.tail_rms_db:.2f}",
                    f"{m.centroid_hz:.1f}",
                    f"{m.fundamental_hz:.1f}",
                    f"{m.fundamental_ratio:.4f}",
                    f"{m.sub30_energy_ratio:.4f}",
                    f"{m.bright5k_energy_ratio:.4f}",
                ]
            )
