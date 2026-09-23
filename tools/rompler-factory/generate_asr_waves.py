#!/usr/bin/env python3
"""Generate ASR-style chromatic wave zones (48 kHz mono) + zones.json for HISE import."""

from __future__ import annotations

import argparse
import json
import math
import struct
import wave
from dataclasses import asdict, dataclass
from pathlib import Path

SAMPLE_RATE = 48000
ROOT = Path(__file__).resolve().parents[2] / "content" / "asr-class-v0" / "factory" / "waves"

# MIDI note numbers: C1 .. C6 every minor third (M3 = 3 semitones)
DEFAULT_NOTES = list(range(24, 85, 3))  # 24=C1, ... up to 84=C6 region


@dataclass
class Zone:
    filename: str
    root_midi: int
    lo_key: int
    hi_key: int
    lo_vel: int
    hi_vel: int
    source: str
    seed: int
    preset: str


def write_wav(path: Path, samples: list[float]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    peak = max(abs(s) for s in samples) or 1.0
    scale = 0.89 / peak
    with wave.open(str(path), "w") as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(SAMPLE_RATE)
        frames = b"".join(
            struct.pack("<h", int(max(-1.0, min(1.0, s * scale)) * 32767)) for s in samples
        )
        w.writeframes(frames)


def midi_to_hz(note: int) -> float:
    return 440.0 * (2.0 ** ((note - 69) / 12.0))


def render_wave(preset: str, root_midi: int, seed: int, duration_s: float) -> list[float]:
    f0 = midi_to_hz(root_midi)
    n = max(1, int(SAMPLE_RATE * duration_s))
    out: list[float] = []

    # Preset tone + decay character (deterministic from seed)
    detune = 1.0 + ((seed % 17) - 8) * 0.0008
    brightness = 0.15 + (seed % 11) * 0.04
    decay = 0.12 if preset == "dark" else 0.08
    if preset == "dark":
        decay += 0.06 + (seed % 5) * 0.02

    for i in range(n):
        t = i / SAMPLE_RATE
        env = math.exp(-t / decay)
        # FM-ish partial stack for "wave" character
        s0 = math.sin(2 * math.pi * f0 * detune * t)
        s1 = math.sin(2 * math.pi * f0 * 2.01 * t) * brightness
        s2 = math.sin(2 * math.pi * f0 * 3.02 * t + seed * 0.01) * brightness * 0.35
        nse = math.sin(t * 9127.0 + seed) * math.sin(t * 4123.0 + seed * 0.3)
        noise_amt = 0.04 if preset == "bright" else 0.012
        sample = (s0 * 0.7 + s1 * 0.25 + s2 * 0.15 + nse * noise_amt) * env
        out.append(sample)
    return out


def note_name(midi: int) -> str:
    names = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"]
    return f"{names[midi % 12]}{midi // 12 - 1}"


def build_zones(preset: str, notes: list[int], seed_base: int, duration_s: float) -> list[Zone]:
    zones: list[Zone] = []
    for i, midi in enumerate(notes):
        seed = seed_base + i
        fname = f"ASR_Wave_{preset}_{note_name(midi)}_s{seed}.wav"
        lo = midi - 1 if i > 0 else midi
        hi = midi + 1 if i < len(notes) - 1 else midi
        if i == 0:
            hi = (notes[0] + notes[1]) // 2 if len(notes) > 1 else midi
        elif i == len(notes) - 1:
            lo = (notes[-2] + notes[-1]) // 2
        else:
            lo = (notes[i - 1] + notes[i]) // 2 + 1
            hi = (notes[i] + notes[i + 1]) // 2
        zones.append(
            Zone(
                filename=fname,
                root_midi=midi,
                lo_key=lo,
                hi_key=hi,
                lo_vel=0,
                hi_vel=127,
                source="factory_parametric_v1",
                seed=seed,
                preset=preset,
            )
        )
        pcm = render_wave(preset, midi, seed, duration_s)
        write_wav(ROOT / preset / fname, pcm)
    return zones


def main() -> None:
    parser = argparse.ArgumentParser(description="Generate ASR-style wave multisample zones")
    parser.add_argument("--preset", choices=("dark", "bright"), default="dark")
    parser.add_argument("--seed-base", type=int, default=1000)
    parser.add_argument("--duration-ms", type=int, default=220)
    parser.add_argument("--notes", type=str, default="", help="Comma-separated MIDI notes (optional)")
    args = parser.parse_args()

    notes = DEFAULT_NOTES
    if args.notes.strip():
        notes = [int(x.strip()) for x in args.notes.split(",") if x.strip()]

    duration_s = args.duration_ms / 1000.0
    zones = build_zones(args.preset, notes, args.seed_base, duration_s)

    manifest = {
        "instrument": f"ASR_Wave_{args.preset}",
        "sample_rate": SAMPLE_RATE,
        "profile": "otto5506",
        "source": "factory_parametric_v1",
        "preset": args.preset,
        "seed_base": args.seed_base,
        "zones": [asdict(z) for z in zones],
    }
    out_dir = ROOT / args.preset
    out_dir.mkdir(parents=True, exist_ok=True)
    manifest_path = out_dir / "zones.json"
    manifest_path.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    print(f"Wrote {len(zones)} zones under {out_dir}")
    print(f"Manifest: {manifest_path}")


if __name__ == "__main__":
    main()
