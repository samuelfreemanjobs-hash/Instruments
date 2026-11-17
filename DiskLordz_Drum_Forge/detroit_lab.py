#!/usr/bin/env python3
"""DiskLordz Detroit Trap Drum Kit Lab.

Original synthesis and bounce patterns for Detroit street-rap drums.
This is not an Atlanta-trap or Memphis-phonk preset, and it does not use
recordings from other kit makers.
"""
from __future__ import annotations

import argparse
import json
import struct
from pathlib import Path

import numpy as np
import soundfile as sf

import forge_core_v2 as core

ENGINE = "DiskLordz DETROIT DRUM LAB"
SAMPLE_RATE_CHOICES = (44100, 48000, 96000)
NOTES = ("C1", "D1", "F1", "G1")
DRUM_CHANNEL = 9
BASS_CHANNEL = 0
DRUM_NOTES = {
    "kick": 36,
    "rim": 37,
    "snare": 38,
    "clap": 39,
    "closed_hat": 42,
    "open_hat": 46,
    "perc": 54,
    "knock": 56,
    "laser": 49,
    "whistle": 71,
}
FULL_COUNTS = {
    "808": 16,
    "kick": 14,
    "snare": 14,
    "clap": 16,
    "closed_hat": 24,
    "open_hat": 16,
    "rim": 16,
    "perc": 14,
    "laser": 7,
    "whistle": 7,
    "knock": 6,
}
DEMO_COUNTS = {name: 1 for name in FULL_COUNTS}
TPQ = 480


def _t(sr: int, seconds: float) -> np.ndarray:
    return np.arange(int(sr * seconds)) / sr


def gen_808(sr: int, rng: np.random.Generator, midi: int = 24):
    """Short Detroit 808: hard knock, tight sub, grit on the mids, stable tail."""
    root = core.note_to_hz(midi)
    length = float(rng.uniform(0.42, 0.68))
    t = _t(sr, length)
    knock = rng.uniform(180, 240) * np.exp(-t / rng.uniform(0.0045, 0.007))
    sag = rng.uniform(1.5, 3.0) * np.exp(-t / 0.05)
    pitch = root + knock + sag
    amp = core.envelope(t, 0.00035, length * 0.72, 1.35)
    fund = core.osc(pitch, sr) * amp
    sub = core.filter_signal(fund, 90, "lowpass", sr, 4)
    mid = core.filter_signal(fund, 180, "highpass", sr, 3)
    mid = core.saturate_oversampled(mid, rng.uniform(4.5, 6.5), mix=0.9)
    click = core.filter_signal(rng.standard_normal(len(t)), 1800, "highpass", sr, 2)
    click *= np.exp(-t / 0.0014) * 0.16
    return sub + 0.55 * mid + click, root


def gen_kick(sr: int, rng: np.random.Generator):
    t = _t(sr, 0.16)
    sweep = rng.uniform(140, 190) * np.exp(-t / 0.008)
    body = core.osc(58 + sweep, sr) * core.envelope(t, 0.0002, 0.055, 0.7)
    click = core.filter_signal(rng.standard_normal(len(t)), 2200, "highpass", sr, 2)
    click *= np.exp(-t / 0.0012) * 0.28
    leaned = body + click + 0.22 * np.maximum(body + click, 0)
    return core.saturate_oversampled(leaned, 2.4, mix=0.7), None


def gen_snare(sr: int, rng: np.random.Generator):
    t = _t(sr, 0.18)
    tune = rng.uniform(210, 250)
    body = core.osc(tune + 80 * np.exp(-t / 0.008), sr) * np.exp(-t / 0.03) * 0.55
    noise = core.filter_signal(rng.standard_normal(len(t)), 2200, "highpass", sr, 3)
    snap = noise * np.exp(-t / 0.02) * 0.45
    return body + snap, None


def gen_clap(sr: int, rng: np.random.Generator):
    t = _t(sr, 0.22)
    noise = core.filter_signal(rng.standard_normal(len(t)), 1400, "highpass", sr, 2)
    noise = core.filter_signal(noise, 6500, "lowpass", sr, 2)
    bursts = np.zeros_like(t)
    for index, start in enumerate((0.0, 0.008, 0.016)):
        tau = 0.012 if index < 2 else 0.04
        bursts += np.exp(-np.maximum(t - start, 0) / tau) * (t >= start)
    return noise * bursts * 0.7, None


def gen_rim(sr: int, rng: np.random.Generator):
    t = _t(sr, 0.09)
    f1 = rng.uniform(290, 340)
    f2 = f1 * rng.uniform(2.15, 2.45)
    wood = np.sin(2 * np.pi * f1 * t) + 0.55 * np.sin(2 * np.pi * f2 * t)
    stick = core.filter_signal(rng.standard_normal(len(t)), 2500, "highpass", sr, 2)
    stick *= np.exp(-t / 0.0015) * 0.35
    return (wood * 0.45 + stick) * np.exp(-t / 0.028), None


def gen_hat(sr: int, rng: np.random.Generator, open_hat: bool = False):
    seconds = 0.2 if open_hat else 0.045
    t = _t(sr, seconds)
    base = min(rng.uniform(3000, 3600), sr * 0.2)
    metal = np.zeros(len(t))
    for index, ratio in enumerate((1.0, 1.41, 1.87, 2.43, 3.19, 4.02)):
        freq = min(base * ratio, sr * 0.46)
        metal += (0.58 ** index) * np.sin(2 * np.pi * freq * t)
    noise = core.filter_signal(rng.standard_normal(len(t)), min(base * 1.3, sr * 0.45), "highpass", sr, 3)
    decay = 0.16 if open_hat else 0.012
    return (noise * 0.35 + metal * 0.55) * core.envelope(t, 0.0001, decay, 1.4), None


def gen_perc(sr: int, rng: np.random.Generator):
    t = _t(sr, 0.12)
    f = rng.uniform(420, 680) * np.exp(-t / 0.02) + 180
    tone = core.osc(f, sr) * np.exp(-t / 0.035)
    click = core.filter_signal(rng.standard_normal(len(t)), 3000, "highpass", sr, 2)
    click *= np.exp(-t / 0.004) * 0.2
    return tone * 0.7 + click, None


def gen_laser(sr: int, rng: np.random.Generator):
    t = _t(sr, rng.uniform(0.18, 0.32))
    start = rng.uniform(2400, 4200)
    pitch = start * np.exp(-t / rng.uniform(0.05, 0.09)) + 160
    return core.osc(pitch, sr) * np.exp(-t / 0.09) * 0.7, None


def gen_whistle(sr: int, rng: np.random.Generator):
    t = _t(sr, 0.28)
    center = rng.uniform(1800, 2600)
    vibrato = 1 + 0.012 * np.sin(2 * np.pi * rng.uniform(4.5, 6.5) * t)
    tone = np.sin(2 * np.pi * np.cumsum(center * vibrato) / sr)
    return core.filter_signal(tone, center * 0.7, "highpass", sr, 2) * np.exp(-t / 0.12), None


def gen_knock(sr: int, rng: np.random.Generator):
    t = _t(sr, 0.07)
    cluster = np.zeros(len(t))
    for _ in range(4):
        freq = rng.uniform(700, 1800)
        cluster += np.sin(2 * np.pi * freq * t) / 4
    return cluster * np.exp(-t / 0.012), None


VOICES = {
    "808": gen_808,
    "kick": gen_kick,
    "snare": gen_snare,
    "clap": gen_clap,
    "rim": gen_rim,
    "closed_hat": lambda sr, rng: gen_hat(sr, rng, False),
    "open_hat": lambda sr, rng: gen_hat(sr, rng, True),
    "perc": gen_perc,
    "laser": gen_laser,
    "whistle": gen_whistle,
    "knock": gen_knock,
}


def render_voice(name: str, sr: int, rng: np.random.Generator, midi: int = 24):
    if name == "808":
        audio, root = gen_808(sr, rng, midi)
    else:
        audio, root = VOICES[name](sr, rng)
    audio = core.master(audio, sr, -1.0)
    return audio, root


def _vlq(value: int) -> bytes:
    buffer = bytearray([value & 0x7F])
    value >>= 7
    while value:
        buffer.append((value & 0x7F) | 0x80)
        value >>= 7
    return bytes(reversed(buffer))


def bounce_events(rng: np.random.Generator, steps: int, swing: float, root_midi: int):
    """16th-note Detroit bounce with swing, holes, and a bar-to-bar answer."""
    step_ticks = TPQ // 4
    events = []
    for index in range(steps):
        bar = index // 16
        pos = index % 16
        swing_ticks = int(swing * step_ticks * 0.5) if index % 2 else 0
        tick = index * step_ticks + swing_ticks
        if pos == 0 or (pos == 7 and bar % 2 == 0) or (pos == 10 and bar % 2 == 1):
            chance = 1.0 if pos == 0 else 0.82
            if rng.random() < chance:
                events.append((max(0, tick - 6), 70, "kick", 112 if pos == 0 else 88))
        if pos in (4, 12) and not (bar % 2 == 1 and pos == 12 and rng.random() < 0.35):
            voice = "clap" if bar % 2 == 0 else "rim"
            events.append((tick, 55, voice, 108 if pos == 4 else 84))
        if not (bar % 2 == 1 and pos >= 14):
            if rng.random() < 0.9:
                hat = "open_hat" if pos == 14 and bar % 2 == 0 else "closed_hat"
                velocity = 104 if pos % 4 == 0 else 68
                events.append((tick + int(rng.integers(-3, 4)), 28, hat, velocity))
        if pos == 0:
            events.append((tick, step_ticks * 4, "808", 114))
        if pos == 11 and bar % 2 == 0 and rng.random() < 0.75:
            events.append((tick, step_ticks * 2, "808_answer", 96))
    noted = []
    for start, duration, name, velocity in events:
        if name == "808":
            noted.append((start, duration, root_midi, velocity, BASS_CHANNEL))
        elif name == "808_answer":
            noted.append((start, duration, root_midi + 7, velocity, BASS_CHANNEL))
        else:
            noted.append((start, duration, DRUM_NOTES[name], velocity, DRUM_CHANNEL))
    noted.sort(key=lambda item: (item[0], item[2]))
    return noted


def write_midi(path: Path, events, bpm: float = 142.0) -> None:
    tempo = int(60_000_000 / bpm)
    track = bytearray()
    track += b"\x00\xff\x51\x03" + struct.pack(">I", tempo)[1:]
    messages = []
    for start, duration, note, velocity, channel in events:
        start = max(0, int(start))
        messages.append((start, 1, 0x90 | channel, note & 0x7F, velocity & 0x7F))
        messages.append((start + max(1, int(duration)), 0, 0x80 | channel, note & 0x7F, 0))
    messages.sort()
    last = 0
    for tick, _priority, status, note, velocity in messages:
        track += _vlq(tick - last) + bytes((status, note, velocity))
        last = tick
    track += b"\x00\xff\x2f\x00"
    header = b"MThd" + struct.pack(">IHHH", 6, 0, 1, TPQ)
    body = b"MTrk" + struct.pack(">I", len(track)) + track
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(header + body)


def read_note_ons(path: Path) -> list[tuple[int, int]]:
    data = path.read_bytes()
    if data[:4] != b"MThd":
        raise ValueError("not a MIDI file")
    _, _, _, tpq = struct.unpack(">IHHH", data[4:14])
    if tpq != TPQ:
        raise ValueError("unexpected tick resolution")
    position = 14
    if data[position:position + 4] != b"MTrk":
        raise ValueError("missing track")
    length = struct.unpack(">I", data[position + 4:position + 8])[0]
    blob = data[position + 8:position + 8 + length]
    cursor = 0
    notes = []

    def take_vlq() -> int:
        nonlocal cursor
        value = 0
        while True:
            byte = blob[cursor]
            cursor += 1
            value = (value << 7) | (byte & 0x7F)
            if not byte & 0x80:
                return value

    while cursor < len(blob):
        take_vlq()
        status = blob[cursor]
        cursor += 1
        if status == 0xFF:
            cursor += 1
            meta_len = take_vlq()
            cursor += meta_len
            continue
        if status & 0xF0 in (0x80, 0x90):
            note = blob[cursor]
            velocity = blob[cursor + 1]
            cursor += 2
            if status & 0xF0 == 0x90 and velocity:
                notes.append((int(note), int(status & 0x0F)))
            continue
        raise ValueError(f"unsupported MIDI status {status:#x}")
    return notes


def build_kit(out: Path, sr: int, seed: int, counts: dict[str, int], grooves: int, bpm: float) -> dict:
    out.mkdir(parents=True, exist_ok=True)
    written = []
    for name, count in counts.items():
        for index in range(count):
            rng = np.random.default_rng(seed + index * 17 + sum(name.encode()))
            if name == "808":
                midi = (24, 26, 29, 31)[index % 4]
                audio, root = render_voice(name, sr, rng, midi)
                label = core.hz_to_note(root).replace("#", "s")
                filename = f"DiskLordz_Detroit_808_{label}_V{index + 1:03d}.wav"
            else:
                audio, _root = render_voice(name, sr, rng)
                filename = f"DiskLordz_Detroit_{name}_V{index + 1:03d}.wav"
            dest = out / name / filename
            dest.parent.mkdir(parents=True, exist_ok=True)
            sf.write(dest, audio, sr, subtype="PCM_24")
            written.append(dest.relative_to(out).as_posix())
    midi_files = []
    for index in range(grooves):
        rng = np.random.default_rng(seed + 5000 + index)
        steps = (16, 32, 64)[index % 3]
        swing = float(rng.uniform(0.08, 0.26))
        root = (24, 26, 29, 31)[index % 4]
        events = bounce_events(rng, steps, swing, root)
        dest = out / "midi" / f"DiskLordz_Detroit_Bounce_{index + 1:02d}.mid"
        write_midi(dest, events, bpm)
        midi_files.append(dest.relative_to(out).as_posix())
    manifest = {
        "engine": ENGINE,
        "sample_rate": sr,
        "seed": seed,
        "bpm": bpm,
        "one_shots": len(written),
        "grooves": len(midi_files),
        "files": written + midi_files,
    }
    (out / "manifest.json").write_text(json.dumps(manifest, indent=2), encoding="utf-8")
    return manifest


def main():
    parser = argparse.ArgumentParser(description=ENGINE)
    parser.add_argument("--full", action="store_true", help="150 one-shots and 20 MIDI grooves")
    parser.add_argument("--sr", type=int, choices=SAMPLE_RATE_CHOICES, default=48000)
    parser.add_argument("--seed", type=int, default=313140)
    parser.add_argument("--bpm", type=float, default=142.0)
    parser.add_argument("--out", default="DiskLordz_Detroit_Kit")
    args = parser.parse_args()
    counts = FULL_COUNTS if args.full else DEMO_COUNTS
    grooves = 20 if args.full else 2
    manifest = build_kit(Path(args.out), args.sr, args.seed, counts, grooves, args.bpm)
    print(f"Finished {manifest['one_shots']} one-shots and {manifest['grooves']} grooves in {args.out}")


if __name__ == "__main__":
    main()
