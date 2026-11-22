#!/usr/bin/env python3
"""DiskLordz Louisiana Expansion v2 — Baton Rouge Trap & New Orleans Bounce Factory.

100% procedural synthesis (zero sampled vocal/drum recordings):
1. LouisianaKitSynth voices:
   - Baton Rouge 808: +20 st exponential pitch drop (14 ms tau), 4 ms click-free attack,
     2nd-harmonic boost + 4x oversampled cubic/tanh soft clipper + 215 Hz phone-speaker presence
   - Plucked BooBass: band-limited saw + octave-down sine sub through resonant 1600 -> 240 Hz (65 ms) lowpass sweep
   - NOLA Dual-Mode Rimshot: 465 Hz & 1720 Hz resonant modes + 2.5 ms transient snap
   - Baton Rouge Dry Crack Snare: 195 Hz swept shell + 1100 Hz high-passed wire burst
2. NolaChantSynth:
   - Glottal band-limited saw + breath burst with +3 st 40 ms pitch scoop through 3 parallel Q=6.5
     vocal-tract formant bandpass filters (Hey_EH 530/1840/2480 Hz, Oh_OW 570/840/2410 Hz, Uh_AH 730/1090/2440 Hz)
3. TriggermanPatternGen:
   - 960 TPQ syncopated bounce kick, 1.75 / 2.25 / 3.75 rolling NOLA rims, and 1/32 machine-gun velocity-ramped hat rolls
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
from scipy.signal import butter, resample_poly, sosfilt

import brick_squad_lab as brick
import forge_core_v2 as core
import pierre_bounce_lab as bounce

ENGINE = "DiskLordz LOUISIANA BOUNCE & BATON ROUGE LAB v2"
TPQ = 960

FORMANT_VOWELS: dict[str, tuple[tuple[float, float, float], tuple[float, float, float]]] = {
    "Hey_EH": ((530.0, 1840.0, 2480.0), (1.00, 0.75, 0.45)),
    "Oh_OW": ((570.0, 840.0, 2410.0), (1.00, 0.80, 0.35)),
    "Uh_AH": ((730.0, 1090.0, 2440.0), (1.00, 0.70, 0.40)),
}


@dataclass(frozen=True)
class LouisianaPadPreset:
    pad: int
    file_name: str
    kind: str  # br_808 | boo_bass | nola_rim | crack_snare | br_clap_snare | chant_eh | chant_ow | chant_ah | kick | shaker | tom | triangle | hat_closed | hat_open
    freq_hz: float
    decay_sec: float
    drive: float
    extra: float = 0.0


LOUISIANA_16_PAD_MATRIX: tuple[LouisianaPadPreset, ...] = (
    LouisianaPadPreset(1, "01_BR_808_Trench_F1.wav", "br_808", 43.6535, 1.40, 3.4, 0.0),
    LouisianaPadPreset(2, "02_BR_808_PhoneCut_G1.wav", "br_808", 48.9994, 1.35, 3.6, 0.0),
    LouisianaPadPreset(3, "03_BR_BooBass_Pluck_F2.wav", "boo_bass", 87.3071, 0.55, 1.8, 0.0),
    LouisianaPadPreset(4, "04_BR_BooBass_Growl_A2.wav", "boo_bass", 110.0000, 0.55, 2.0, 0.0),
    LouisianaPadPreset(5, "05_BR_Kick_PunchTight.wav", "kick", 58.0, 0.16, 3.1, 240.0),
    LouisianaPadPreset(6, "06_NOLA_Kick_FastBounce.wav", "kick", 64.0, 0.11, 2.5, 210.0),
    LouisianaPadPreset(7, "07_BR_CrackSnare_Dry.wav", "crack_snare", 195.0, 0.28, 1.6, 0.0),
    LouisianaPadPreset(8, "08_BR_SnareClap_SecondHit.wav", "br_clap_snare", 196.0, 0.21, 2.4, 11.0),
    LouisianaPadPreset(9, "09_NOLA_Rimshot_465_1720.wav", "nola_rim", 465.0, 0.18, 1.9, 1720.0),
    LouisianaPadPreset(10, "10_NOLA_Chant_Hey_EH.wav", "chant_eh", 185.0, 0.24, 4.2, 0.0),
    LouisianaPadPreset(11, "11_NOLA_Chant_Oh_OW.wav", "chant_ow", 172.0, 0.26, 4.2, 0.0),
    LouisianaPadPreset(12, "12_NOLA_Chant_Uh_AH.wav", "chant_ah", 196.0, 0.22, 4.2, 0.0),
    LouisianaPadPreset(13, "13_NOLA_Shaker_Roll.wav", "shaker", 5200.0, 0.085, 1.3, 0.0),
    LouisianaPadPreset(14, "14_NOLA_Tom_Turnaround.wav", "tom", 138.0, 0.20, 1.8, 0.0),
    LouisianaPadPreset(15, "15_LA_Hat_Closed_Triggerman.wav", "hat_closed", 1.05, 0.042, 1.5, 0.0),
    LouisianaPadPreset(16, "16_LA_Hat_Open_Accent.wav", "hat_open", 0.98, 0.24, 1.5, 0.0),
)


@dataclass(frozen=True)
class StepEvent:
    beat_offset: float
    midi_note: int
    velocity: float
    length_beats: float
    channel: int = 9


def _oversampled_cubic_tanh(x: np.ndarray, drive: float = 3.2) -> np.ndarray:
    up = resample_poly(x, 4, 1, axis=0)
    driven = np.clip(up * drive, -1.5, 1.5)
    shaped = np.tanh(driven - 0.15 * (driven ** 3))
    down = resample_poly(shaped, 1, 4, axis=0)[: len(x)]
    if len(down) < len(x):
        pad_shape = ((0, len(x) - len(down)), (0, 0)) if x.ndim == 2 else (0, len(x) - len(down))
        down = np.pad(down, pad_shape)
    return down - np.mean(down, axis=0)


def render_baton_rouge_808(sr: int, root_hz: float = 43.6535, duration_sec: float = 1.4, drive: float = 3.4) -> np.ndarray:
    """LouisianaKitSynth::renderBatonRouge808 with 4x oversampled cubic/tanh clip + 215 Hz phone-cut boost."""
    t = np.arange(int(sr * duration_sec)) / sr
    pitch_env = 1.0 + 2.17 * np.exp(-t / 0.014)  # +20 semitones punch
    freq = root_hz * pitch_env
    phase = 2.0 * np.pi * np.cumsum(freq) / sr

    attack = np.minimum(1.0, t / 0.004)
    amp_env = attack * np.exp(-t / 0.42)
    shaped = (np.sin(phase) + 0.18 * np.sin(2.0 * phase)) * amp_env

    mono = _oversampled_cubic_tanh(shaped, drive=drive)
    mono = bounce.peaking_eq(mono, sr, 215.0, 1.05, brick.db_to_gain(4.2))
    mono = core.filter_signal(mono, 26.0, "highpass", sr, 2)
    return np.column_stack((mono, mono))


def render_boo_bass_pluck(sr: int, root_hz: float = 87.3071, duration_sec: float = 0.55, drive: float = 1.8) -> np.ndarray:
    """Band-limited saw + octave-down sine sub through resonant 1600 -> 240 Hz (65 ms) swept low-pass."""
    n = int(sr * duration_sec)
    t = np.arange(n) / sr
    amp_env = np.minimum(1.0, t / 0.002) * np.exp(-t / 0.16)

    # Band-limited sawtooth (additive harmonics < 0.45 * sr) + octave-down sine sub
    nyq_limit = sr * 0.45
    max_h = min(32, int(nyq_limit // max(root_hz, 1.0)))
    saw = np.zeros_like(t)
    for h in range(1, max_h + 1):
        saw += ((-1.0) ** (h + 1)) * (2.0 / np.pi) * (1.0 / h) * np.sin(2.0 * np.pi * (root_hz * h) * t)
    sub = np.sin(2.0 * np.pi * (root_hz * 0.5) * t)
    exciter = (saw * 0.65 + sub * 0.55) * amp_env

    # Time-varying TPT SVF low-pass (cutoff 1600 -> 240 Hz over 65 ms, resonance Q = 1.8)
    cutoffs = 240.0 + 1360.0 * np.exp(-t / 0.065)
    r_damp = 1.0 / (2.0 * 1.8)
    ic1 = 0.0
    ic2 = 0.0
    filtered = np.zeros(n, dtype=np.float64)
    for i in range(n):
        g = math.tan(math.pi * min(float(cutoffs[i]), sr * 0.42) / sr) if False else np.tan(np.pi * min(float(cutoffs[i]), sr * 0.42) / sr)
        a1 = 1.0 / (1.0 + g * (g + 2.0 * r_damp))
        a2 = g * a1
        a3 = g * a2
        v3 = float(exciter[i]) - ic2
        v1 = a1 * ic1 + a2 * v3
        v2 = ic2 + a2 * ic1 + a3 * v3
        ic1 = 2.0 * v1 - ic1
        ic2 = 2.0 * v2 - ic2
        filtered[i] = v2

    mono = core.saturate_oversampled(filtered, drive=drive, mix=1.0, factor=4)
    return np.column_stack((mono, mono))


def render_nola_rimshot(sr: int, rng: np.random.Generator, f1: float = 465.0, f2: float = 1720.0, duration_sec: float = 0.18, drive: float = 1.9) -> np.ndarray:
    t = np.arange(int(sr * duration_sec)) / sr
    body = np.sin(2.0 * np.pi * f1 * t) * np.exp(-t / 0.018) + 0.7 * np.sin(2.0 * np.pi * f2 * t) * np.exp(-t / 0.009)
    transient = rng.uniform(-1.0, 1.0, len(t)) * np.exp(-t / 0.0025)
    mono = core.saturate_oversampled((body * 0.75 + transient * 0.45) * drive, drive=1.0, mix=1.0, factor=4)
    return np.column_stack((mono, mono))


def render_crack_snare(sr: int, rng: np.random.Generator, shell_hz: float = 195.0, duration_sec: float = 0.28, drive: float = 1.6) -> np.ndarray:
    t = np.arange(int(sr * duration_sec)) / sr
    shell_freq = shell_hz * (1.0 + 0.8 * np.exp(-t / 0.012))
    shell = core.osc(shell_freq, sr) * np.exp(-t / 0.045)
    wire = core.filter_signal(rng.uniform(-1.0, 1.0, len(t)) * np.exp(-t / 0.075), 1100.0, "highpass", sr, 2)
    mono = core.saturate_oversampled((shell * 0.85 + wire * 0.95) * drive, drive=1.0, mix=1.0, factor=4)
    return np.column_stack((mono, mono))


def render_bounce_chant(sr: int, rng: np.random.Generator, vowel: str = "Hey_EH", base_pitch_hz: float = 185.0, duration_sec: float = 0.24, drive: float = 4.2) -> np.ndarray:
    """NolaChantSynth: pitch-scooped band-limited glottal saw + breath -> 3 parallel Q=6.5 formant bandpass filters."""
    t = np.arange(int(sr * duration_sec)) / sr
    scoop = 1.0 + 0.18 * np.sin(np.minimum(1.0, t / 0.04) * np.pi) - 0.12 * (t / duration_sec)
    current_hz = base_pitch_hz * scoop
    phase = 2.0 * np.pi * np.cumsum(current_hz) / sr

    # Band-limited glottal sawtooth (18 harmonics comfortably below Nyquist)
    glottal_saw = np.zeros_like(t)
    for h in range(1, 19):
        glottal_saw += ((-1.0) ** (h + 1)) * (2.0 / np.pi) * (1.0 / h) * np.sin(h * phase)

    attack = np.minimum(1.0, t / 0.005)
    release = np.where(t > duration_sec - 0.035, np.maximum(0.0, (duration_sec - t) / 0.035), 1.0)
    env = attack * release

    breath = rng.uniform(-1.0, 1.0, len(t)) * 0.12
    glottal = (glottal_saw + breath) * env

    freqs, gains = FORMANT_VOWELS[vowel]
    vocal_sum = np.zeros_like(t)
    q = 6.5
    for f_center, g_weight in zip(freqs, gains):
        bw = f_center / q
        lo = max(60.0, f_center - bw * 0.5)
        hi = min(sr * 0.44, f_center + bw * 0.5)
        sos = butter(2, [lo, hi], btype="bandpass", fs=sr, output="sos")
        vocal_sum += sosfilt(sos, glottal) * g_weight

    mono = core.saturate_oversampled(vocal_sum, drive=drive, mix=1.0, factor=4)
    return np.column_stack((mono * 0.97, mono * 1.03))


def generate_triggerman_bar(swing_amount: float = 0.15) -> list[StepEvent]:
    """TriggermanPatternGen::generateTriggermanBar (1 bar, 4.0 quarter-note beats, 960 TPQ ready)."""
    events: list[StepEvent] = [
        StepEvent(0.00, 36, 1.00, 0.25),
        StepEvent(0.75, 36, 0.90, 0.25),
        StepEvent(1.50, 36, 0.95, 0.25),
        StepEvent(2.50, 36, 0.92, 0.25),
        StepEvent(3.25, 36, 0.88, 0.25),
        StepEvent(1.00, 38, 1.00, 0.25),
        StepEvent(1.75, 37, 0.85, 0.12),
        StepEvent(2.25, 37, 0.90, 0.12),
        StepEvent(3.00, 38, 1.00, 0.25),
        StepEvent(3.75, 37, 0.82, 0.12),
    ]
    for step16 in range(16):
        beat_pos = step16 * 0.25 + (swing_amount * 0.06 if step16 % 2 == 1 else 0.0)
        if step16 in (7, 14, 15):
            for sub in range(2):
                ramp_vel = 0.55 + 0.40 * sub
                events.append(StepEvent(beat_pos + sub * 0.125, 42, ramp_vel, 0.10))
        else:
            accent_vel = 0.85 if step16 % 2 == 0 else 0.62
            events.append(StepEvent(beat_pos, 42, accent_vel, 0.15))
    return events


def build_baton_rouge_2bar(bounce_sync_ms: float = 16.0, bpm: float = 162.0, root_midi: int = 29) -> list[StepEvent]:
    shift_beats = (float(np.clip(bounce_sync_ms, 0.0, 35.0)) / 1000.0) * (bpm / 60.0)
    events: list[StepEvent] = [
        StepEvent(2.0, 38, 0.95, 0.20, 9),
        StepEvent(6.0, 38, 0.98, 0.20, 9),
        StepEvent(6.0, 39, 0.92, 0.20, 9),
    ]
    for st, pitch in ((0, root_midi), (3, root_midi), (6, root_midi + 5), (11, root_midi + 3), (16, root_midi), (19, root_midi + 5), (22, root_midi + 7), (27, root_midi + 3)):
        beat = st * 0.25
        events.append(StepEvent(beat, 36, 0.94, 0.20, 9))
        events.append(StepEvent(beat, pitch, 0.95, 0.70, 0))
        events.append(StepEvent(beat + shift_beats * 0.5, pitch + 12, 0.82, 0.45, 1))
    for st in range(32):
        beat = st * 0.25 + (shift_beats if st % 2 == 1 else 0.0)
        events.append(StepEvent(beat, 42, 0.84 if st % 4 == 0 else 0.64, 0.12, 9))
    return events


def _vlq(value: int) -> bytes:
    buf = bytearray([value & 0x7F])
    value >>= 7
    while value:
        buf.append((value & 0x7F) | 0x80)
        value >>= 7
    return bytes(reversed(buf))


def export_step_events_to_midi(events: list[StepEvent], path: Path, bpm: float = 98.0) -> None:
    tempo = int(60_000_000 / bpm)
    msgs = []
    for ev in events:
        start_tick = max(0, int(round(ev.beat_offset * TPQ)))
        end_tick = max(start_tick + 1, int(round((ev.beat_offset + ev.length_beats) * TPQ)))
        vel_int = int(np.clip(round(ev.velocity * 127.0), 1, 127))
        msgs.append((start_tick, 1, 0x90 | (ev.channel & 0x0F), ev.midi_note & 0x7F, vel_int))
        msgs.append((end_tick, 0, 0x80 | (ev.channel & 0x0F), ev.midi_note & 0x7F, 0))
    msgs.sort()
    track = bytearray(b"\x00\xff\x51\x03" + struct.pack(">I", tempo)[1:])
    last = 0
    for tick, _prio, status, note, vel in msgs:
        track += _vlq(tick - last) + bytes((status, note, vel))
        last = tick
    track += b"\x00\xff\x2f\x00"
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(b"MThd" + struct.pack(">IHHH", 6, 0, 1, TPQ) + b"MTrk" + struct.pack(">I", len(track)) + track)


def render_louisiana_pad(preset: LouisianaPadPreset, sr: int = 48000, seed: int = 504225) -> np.ndarray:
    rng = np.random.default_rng(seed + preset.pad * 71)
    if preset.kind == "br_808":
        raw = render_baton_rouge_808(sr, preset.freq_hz, preset.decay_sec, preset.drive)
    elif preset.kind == "boo_bass":
        raw = render_boo_bass_pluck(sr, preset.freq_hz, preset.decay_sec, preset.drive)
    elif preset.kind == "nola_rim":
        raw = render_nola_rimshot(sr, rng, preset.freq_hz, preset.extra, preset.decay_sec, preset.drive)
    elif preset.kind == "crack_snare":
        raw = render_crack_snare(sr, rng, preset.freq_hz, preset.decay_sec, preset.drive)
    elif preset.kind == "br_clap_snare":
        bp = brick.BrickParams(
            snare_tune_hz=preset.freq_hz,
            clap_spread_ms=preset.extra,
            snare_decay_sec=preset.decay_sec,
            snap_drive=preset.drive,
            high_shelf_db=4.8,
        )
        raw, _ = brick.gen_snare_clap_layer(sr, rng, bp)
    elif preset.kind in ("chant_eh", "chant_ow", "chant_ah"):
        vowel = {"chant_eh": "Hey_EH", "chant_ow": "Oh_OW", "chant_ah": "Uh_AH"}[preset.kind]
        raw = render_bounce_chant(sr, rng, vowel, preset.freq_hz, preset.decay_sec, preset.drive)
    elif preset.kind == "kick":
        t = np.arange(int(0.36 * sr)) / sr
        freq = preset.freq_hz + (preset.extra - preset.freq_hz) * np.exp(-t / 0.013)
        body = core.osc(freq, sr) * (1.0 - np.exp(-t / 0.0003)) * np.exp(-t / preset.decay_sec)
        click = core.filter_signal(rng.uniform(-1.0, 1.0, len(t)), 2100.0, "highpass", sr, 2) * np.exp(-t / 0.0025) * 0.28
        mono = _oversampled_cubic_tanh(body + click, drive=preset.drive)
        raw = np.column_stack((mono, mono))
    elif preset.kind == "shaker":
        t = np.arange(int(0.22 * sr)) / sr
        env = np.sin(np.pi * np.clip(t / preset.decay_sec, 0.0, 1.0)) ** 1.6 * (t <= preset.decay_sec)
        grain = 0.75 + 0.25 * np.sin(2.0 * np.pi * 68.0 * t)
        ch_list = [
            core.filter_signal(core.filter_signal(rng.standard_normal(len(t)), 3400.0, "highpass", sr, 3), min(9500.0, sr * 0.44), "lowpass", sr, 3) * env * grain
            for _ in range(2)
        ]
        raw = np.column_stack(ch_list)
    elif preset.kind == "tom":
        t = np.arange(int(0.45 * sr)) / sr
        freq = preset.freq_hz * 0.62 + (preset.freq_hz * 0.75) * np.exp(-t / 0.045)
        mono = _oversampled_cubic_tanh(core.osc(freq, sr) * (1.0 - np.exp(-t / 0.0005)) * np.exp(-t / preset.decay_sec), drive=preset.drive)
        raw = np.column_stack((mono, mono))
    else:
        bp = brick.BrickParams(
            hat_pitch_mult=preset.freq_hz,
            hat_decay_sec=preset.decay_sec,
            open_hat_decay_sec=preset.decay_sec,
            hat_grit_drive=preset.drive,
        )
        mono, _ = brick.gen_909_metallic_hat(sr, rng, bp, is_open=(preset.kind == "hat_open"))
        raw = np.column_stack((mono, mono))

    trimmed = brick.trim_below_db(raw, sr, threshold_db=-70.0)
    return core.master(trimmed, sr, ceiling_db=-0.8)


def export_louisiana_kit(
    out_dir: Path, sr: int = 48000, seed: int = 504225, swing: float = 0.15, bounce_sync_ms: float = 18.0, make_zip: bool = True
) -> dict:
    out_dir.mkdir(parents=True, exist_ok=True)
    rows = []
    written: list[Path] = []

    for preset in LOUISIANA_16_PAD_MATRIX:
        audio = render_louisiana_pad(preset, sr=sr, seed=seed)
        dest = out_dir / preset.file_name
        sf.write(dest, audio, sr, subtype="PCM_24")
        written.append(dest)
        rows.append({**asdict(preset), "frames": int(len(audio)), "true_peak_dbtp": round(core.true_peak_db(audio), 3)})

    triggerman_mid = out_dir / "17_NOLA_Triggerman_98BPM.mid"
    br_mid = out_dir / "18_BatonRouge_Trench_162BPM.mid"
    export_step_events_to_midi(generate_triggerman_bar(swing), triggerman_mid, bpm=98.0)
    export_step_events_to_midi(build_baton_rouge_2bar(bounce_sync_ms, 162.0), br_mid, bpm=162.0)
    written.extend([triggerman_mid, br_mid])

    manifest_path = out_dir / "manifest.json"
    manifest = {
        "engine": ENGINE,
        "sample_rate": sr,
        "tpq": TPQ,
        "seed": seed,
        "count": len(rows),
        "midi": [triggerman_mid.name, br_mid.name],
        "pads": rows,
    }
    manifest_path.write_text(json.dumps(manifest, indent=2), encoding="utf-8")

    if make_zip:
        zip_path = out_dir / "DiskLordz_Louisiana_Bounce_BatonRouge_Pack.zip"
        with zipfile.ZipFile(zip_path, "w", zipfile.ZIP_DEFLATED) as zf:
            for item in written:
                zf.write(item, arcname=item.name)
            zf.write(manifest_path, arcname="manifest.json")
        manifest["zip"] = zip_path.name
    return manifest


def main() -> None:
    ap = argparse.ArgumentParser(description=ENGINE)
    ap.add_argument("--sr", type=int, choices=(44100, 48000, 96000), default=48000)
    ap.add_argument("--seed", type=int, default=504225)
    ap.add_argument("--swing", type=float, default=0.15)
    ap.add_argument("--bounce-ms", type=float, default=18.0)
    ap.add_argument("--out", default="DiskLordz_Louisiana_Kit")
    args = ap.parse_args()
    manifest = export_louisiana_kit(Path(args.out), args.sr, args.seed, args.swing, args.bounce_ms)
    print(f"Exported {manifest['count']} Louisiana pads + {len(manifest['midi'])} MIDI grooves to {args.out}")


if __name__ == "__main__":
    main()
