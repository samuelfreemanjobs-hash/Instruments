#!/usr/bin/env python3
"""CLI kit generation for batch/offline use (mirrors website parametric factory v1)."""

from __future__ import annotations

import argparse
import hashlib
import json
import math
import struct
import wave
from pathlib import Path

SAMPLE_RATE = 44100
NAMES = ("kick", "snare", "hat_closed", "hat_open", "rim", "clap")

# Keep in sync with website/src/lib/generation/prompt-params.ts PRESET_BASE
PRESET_BASE = {
    "boulevard-86": {"kickPitch": 52, "kickDecay": 0.22, "snareBody": 0.35, "snareSnap": 0.25},
    "midnight-circuit": {"kickPitch": 38, "kickDecay": 0.35, "snareBody": 0.2, "snareSnap": 0.55},
    "disklordz-screw": {"kickPitch": 34, "kickDecay": 0.55, "snareBody": 0.55, "snareSnap": 0.15},
    "terminal-mirage": {"kickPitch": 58, "kickDecay": 0.18, "snareBody": 0.3, "snareSnap": 0.35},
    "mpc-ready-808": {"kickPitch": 45, "kickDecay": 0.28, "snareBody": 0.25, "snareSnap": 0.4},
}


def seed_from(prompt: str, preset: str) -> int:
    h = hashlib.sha256(f"{preset}::{prompt}".encode()).hexdigest()
    return int(h[:8], 16)


def write_wav(path: Path, samples: list[float]) -> str:
    peak = max(abs(s) for s in samples) or 1.0
    scale = 0.89 / peak
    path.parent.mkdir(parents=True, exist_ok=True)
    with wave.open(str(path), "w") as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(SAMPLE_RATE)
        frames = b"".join(
            struct.pack("<h", int(max(-1.0, min(1.0, s * scale)) * 32767)) for s in samples
        )
        w.writeframes(frames)
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main() -> None:
    p = argparse.ArgumentParser()
    p.add_argument("--preset", required=True)
    p.add_argument("--prompt", required=True)
    p.add_argument("--out", type=Path, required=True)
    args = p.parse_args()

    base = PRESET_BASE.get(args.preset, PRESET_BASE["mpc-ready-808"])
    s = seed_from(args.prompt, args.preset)
    # Minimal offline path: reuse static stub packs from public/samples when present
    stub_root = Path(__file__).resolve().parents[2] / "website" / "public" / "samples" / args.preset
    manifest_samples = []
    for name in NAMES:
        src = stub_root / f"{name}.wav"
        dst = args.out / f"{name}.wav"
        if src.is_file():
            dst.parent.mkdir(parents=True, exist_ok=True)
            dst.write_bytes(src.read_bytes())
            sha = hashlib.sha256(dst.read_bytes()).hexdigest()
        else:
            sha = write_wav(dst, [0.0] * 1000)
        manifest_samples.append(
            {"name": name, "filename": f"{name}.wav", "sha256": sha, "sourceId": f"factory-cli://{args.preset}/{name}"}
        )

    manifest = {
        "format": "DISKLORDZ_DRUM_KIT_MANIFEST",
        "version": 1,
        "prompt": args.prompt,
        "presetId": args.preset,
        "factorySeed": s,
        "factoryBase": base,
        "samples": manifest_samples,
    }
    (args.out / "manifest.json").write_text(json.dumps(manifest, indent=2))
    print(json.dumps({"out": str(args.out), "seed": s}, indent=2))


if __name__ == "__main__":
    main()
