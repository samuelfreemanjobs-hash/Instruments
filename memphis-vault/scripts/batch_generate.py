#!/usr/bin/env python3
"""Memphis Vault — Batch Generator (procedural one-shots, no external samples).

Generates royalty-free WAV one-shots from deterministic math + seeded RNG.
Each batch run writes a folder: ``--out/<batch_id>/{manifest.json, wav/*}``.

Requirements: Python 3.9+ stdlib only (wave, math, hashlib).
"""

from __future__ import annotations

import argparse
import hashlib
import json
import math
import random
import struct
import wave
from pathlib import Path
from typing import Callable, Iterator

SAMPLE_RATE = 44100


def write_wav_mono_16bit(path: Path, samples: list[float]) -> str:
    """Peak-normalize to ~-1 dBFS, write mono 16-bit PCM WAV. Returns sha256 hex."""
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


def lerp(a: float, b: float, t: float) -> float:
    return a + (b - a) * t


def env_exponential(n: int, decay: float) -> list[float]:
    """0..1 exponential decay shape of length n."""
    out: list[float] = []
    for i in range(n):
        t = i / max(1, n - 1)
        out.append(math.exp(-decay * t * 10.0))
    return out


def rng_for_batch(batch_id: str, global_seed: int | None) -> random.Random:
    """Reproducible RNG per batch.

    With ``global_seed`` set, each batch still gets a distinct stream by mixing
    ``batch_id`` into the seed. With no global seed, the seed is derived only from
    ``batch_id`` so re-running with the same id reproduces the same WAVs.
    """
    if global_seed is not None:
        mix = hashlib.sha256(f"memphis-vault::{global_seed}::{batch_id}".encode()).digest()
    else:
        mix = hashlib.sha256(f"memphis-vault::{batch_id}".encode()).digest()
    return random.Random(int.from_bytes(mix[:8], "big"))


def gen_kick(rng: random.Random) -> list[float]:
    dur_ms = rng.uniform(180.0, 420.0)
    n = max(800, int(SAMPLE_RATE * dur_ms / 1000.0))
    f0 = rng.uniform(38.0, 58.0)
    f1 = rng.uniform(28.0, 42.0)
    curve = rng.uniform(2.0, 6.0)
    amp = rng.uniform(0.85, 1.0)
    phase = rng.uniform(0.0, math.tau)
    env = env_exponential(n, rng.uniform(0.35, 0.75))
    samp: list[float] = []
    for i in range(n):
        t = i / SAMPLE_RATE
        br = t * n / max(1, n - 1)
        f = lerp(f0, f1, br**curve)
        s = math.sin(phase + math.tau * f * t) * env[i] * amp
        samp.append(s)
    return samp


def gen_snare(rng: random.Random) -> list[float]:
    dur_ms = rng.uniform(90.0, 220.0)
    n = max(600, int(SAMPLE_RATE * dur_ms / 1000.0))
    tone_f = rng.uniform(180.0, 280.0)
    tone_mix = rng.uniform(0.08, 0.22)
    decay = rng.uniform(0.5, 1.1)
    env = env_exponential(n, decay)
    hp_state = 0.0
    prev_in = 0.0
    coeff = rng.uniform(0.92, 0.98)
    samp: list[float] = []
    for i in range(n):
        t = i / SAMPLE_RATE
        noise = rng.gauss(0.0, 1.0)
        x = noise - prev_in
        prev_in = noise
        hp_state = coeff * (hp_state + x)
        tone = math.sin(math.tau * tone_f * t) * env[i] * tone_mix
        body = hp_state * 0.35 * env[i]
        samp.append((body + tone) * 0.85)
    return samp


def gen_hat(rng: random.Random, open_hat: bool) -> list[float]:
    dur_ms = rng.uniform(35.0, 95.0) if not open_hat else rng.uniform(120.0, 320.0)
    n = max(200, int(SAMPLE_RATE * dur_ms / 1000.0))
    decay = rng.uniform(1.2, 3.5) if not open_hat else rng.uniform(0.4, 0.9)
    env = env_exponential(n, decay)
    filt = rng.uniform(0.88, 0.97)
    z = [0.0, 0.0]
    samp: list[float] = []
    for i in range(n):
        w = rng.gauss(0.0, 1.0)
        z[0] = lerp(z[0], w, 1.0 - filt)
        z[1] = lerp(z[1], z[0], 0.45 + rng.random() * 0.3)
        s = z[1] * env[i] * (0.5 + rng.random() * 0.35)
        samp.append(s)
    return samp


def gen_rim(rng: random.Random) -> list[float]:
    n = max(150, int(SAMPLE_RATE * rng.uniform(0.04, 0.12)))
    f = rng.uniform(600.0, 1200.0)
    ring = rng.uniform(0.92, 0.988)
    y = 0.0
    env = env_exponential(n, rng.uniform(2.0, 5.0))
    samp: list[float] = []
    for i in range(n):
        t = i / SAMPLE_RATE
        x = math.sin(math.tau * f * t) if i < 12 else 0.0
        x += rng.gauss(0.0, 0.15) * env[i]
        y = ring * y + x
        samp.append(y * env[i] * 0.9)
    return samp


def gen_clap(rng: random.Random) -> list[float]:
    n = max(400, int(SAMPLE_RATE * rng.uniform(0.12, 0.28)))
    env = env_exponential(n, rng.uniform(0.55, 0.95))
    times = sorted(
        [rng.uniform(0.001, 0.012) * i + rng.uniform(0.0, 0.004) for i in range(4)]
    )
    centers = [int(SAMPLE_RATE * t) for t in times]
    samp = [0.0] * n
    for c in centers:
        if c >= n:
            continue
        w = int(SAMPLE_RATE * rng.uniform(0.008, 0.02))
        for j in range(w):
            idx = c + j
            if idx >= n:
                break
            e = env[idx] * math.exp(-j / max(1.0, w * 0.35))
            samp[idx] += rng.gauss(0.0, 1.0) * e * 0.45
    return samp


GENERATORS: dict[str, Callable[[random.Random], list[float]]] = {
    "kick": gen_kick,
    "snare": gen_snare,
    "hat_closed": lambda r: gen_hat(r, False),
    "hat_open": lambda r: gen_hat(r, True),
    "rim": gen_rim,
    "clap": gen_clap,
}


def parse_names(s: str) -> list[str]:
    parts = [p.strip() for p in s.split(",") if p.strip()]
    for p in parts:
        if p not in GENERATORS:
            raise SystemExit(f"Unknown layer name: {p!r}. Choose from: {sorted(GENERATORS)}")
    return parts


def iter_batches(batch_prefix: str, count: int) -> Iterator[str]:
    for i in range(count):
        yield f"{batch_prefix}_{i + 1:04d}"


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n", 1)[0])
    ap.add_argument("--out", type=Path, required=True, help="Output root directory")
    ap.add_argument("--count", type=int, default=1, help="Number of batch folders to generate")
    ap.add_argument("--prefix", type=str, default="memphis", help="Batch id prefix")
    ap.add_argument(
        "--layers",
        type=str,
        default="kick,snare,hat_closed,hat_open,rim,clap",
        help="Comma-separated layer names",
    )
    ap.add_argument(
        "--seed",
        type=int,
        default=None,
        help="Global RNG seed (default: derive from each batch id for reproducible packs)",
    )
    args = ap.parse_args()

    names = parse_names(args.layers)
    args.out.mkdir(parents=True, exist_ok=True)

    summary: list[dict[str, object]] = []

    for batch_id in iter_batches(args.prefix, args.count):
        rng = rng_for_batch(batch_id, args.seed)

        batch_dir = args.out / batch_id
        wav_dir = batch_dir / "wav"
        wav_dir.mkdir(parents=True, exist_ok=True)

        samples_meta: list[dict[str, object]] = []
        for layer in names:
            fn = f"{layer}.wav"
            path = wav_dir / fn
            samples = GENERATORS[layer](rng)
            sha = write_wav_mono_16bit(path, samples)
            samples_meta.append(
                {
                    "name": layer,
                    "filename": f"wav/{fn}",
                    "sha256": sha,
                    "sourceId": f"memphis-vault-procedural://{batch_id}/{layer}",
                }
            )

        manifest = {
            "format": "MEMPHIS_VAULT_BATCH_MANIFEST",
            "version": 1,
            "batchId": batch_id,
            "sampleRate": SAMPLE_RATE,
            "layers": names,
            "samples": samples_meta,
            "generator": {
                "script": "memphis-vault/scripts/batch_generate.py",
                "description": "Procedural synthesis; no external audio files.",
            },
        }
        (batch_dir / "manifest.json").write_text(json.dumps(manifest, indent=2))

        summary.append({"batchId": batch_id, "path": str(batch_dir), "samples": len(samples_meta)})

    print(json.dumps({"batches": summary}, indent=2))


if __name__ == "__main__":
    main()
