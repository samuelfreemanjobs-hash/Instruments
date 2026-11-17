#!/usr/bin/env python3
"""Mix kit masters onto a trap_loop_architect blueprint grid → exact loop_samples PCM16 WAV.

Original arrangement only. Does not claim artist/machine authenticity.
"""
from __future__ import annotations

import argparse
import json
import math
import struct
import wave
from pathlib import Path

from dsp_math import loop_samples


def read_pcm16_mono(path: Path) -> tuple[list[float], int]:
    with wave.open(str(path), 'rb') as w:
        if w.getnchannels() != 1 or w.getsampwidth() != 2:
            raise ValueError(f'expected mono PCM16: {path}')
        rate = w.getframerate()
        raw = w.readframes(w.getnframes())
    samples = list(struct.unpack('<' + 'h' * (len(raw) // 2), raw))
    return [s / 32768.0 for s in samples], rate


def write_pcm16_mono(path: Path, samples: list[float], rate: int) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    clipped = [max(-1.0, min(1.0, x)) for x in samples]
    frames = struct.pack('<' + 'h' * len(clipped), *(int(round(x * 32767.0)) for x in clipped))
    with wave.open(str(path), 'wb') as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(rate)
        w.writeframes(frames)


def peak_normalize(buf: list[float], ceiling: float = 10 ** (-0.1 / 20.0)) -> list[float]:
    peak = max((abs(x) for x in buf), default=0.0)
    if peak <= 1e-12:
        return buf
    g = ceiling / peak
    return [x * g for x in buf]


def pick_masters(kit: Path, kinds: list[str], variant: int = 0) -> dict[str, Path]:
    if not isinstance(variant, int) or variant < 0:
        raise ValueError('variant must be a non-negative int')
    manifest = json.loads((kit / 'manifest.json').read_text(encoding='utf8'))
    by_kind: dict[str, list[Path]] = {}
    for e in manifest['samples']:
        by_kind.setdefault(e['kind'], []).append(kit / e['path'])
    chosen = {}
    for kind in kinds:
        paths = by_kind.get(kind) or []
        if not paths:
            raise FileNotFoundError(f'no masters for kind {kind} in {kit}')
        chosen[kind] = paths[variant % len(paths)]
    return chosen


def render_loop(
    blueprint: dict, kit: Path, out: Path, ceiling_dbfs: float = -0.1, variant: int = 0
) -> dict:
    bpm = float(blueprint['bpm'])
    bars = int(blueprint['bars'])
    sr = int(blueprint['sample_rate'])
    grid = int(blueprint['grid_subdivisions_per_bar'])
    samples_bar = int(blueprint.get('samples_per_bar') or loop_samples(1, bpm, sr))
    if samples_bar != loop_samples(1, bpm, sr):
        raise ValueError(
            f"blueprint samples_per_bar {samples_bar} != loop_samples(1,{bpm},{sr})="
            f"{loop_samples(1, bpm, sr)}"
        )
    # Grid uses per-bar rounding; file length uses loop_samples(bars) (may differ by ±1).
    grid_total = samples_bar * bars
    file_len = loop_samples(bars, bpm, sr)
    step_len = samples_bar / float(grid)
    lanes = blueprint['lanes']
    masters = pick_masters(kit, [k for k in lanes if lanes[k]], variant=variant)
    audio = {k: read_pcm16_mono(p)[0] for k, p in masters.items()}
    mix_len = max(grid_total, file_len)
    buf = [0.0] * mix_len
    hits = 0
    for kind, events in lanes.items():
        if kind not in audio:
            continue
        oneshot = audio[kind]
        for ev in events:
            step = int(ev['step'])
            vel = float(ev.get('vel', 100)) / 127.0
            start = int(round(step * step_len))
            if start >= mix_len:
                continue
            for i, s in enumerate(oneshot):
                j = start + i
                if j >= mix_len:
                    break
                buf[j] += s * vel
            hits += 1
    if len(buf) < file_len:
        buf.extend([0.0] * (file_len - len(buf)))
    elif len(buf) > file_len:
        buf = buf[:file_len]
    ceiling = 10 ** (ceiling_dbfs / 20.0)
    buf = peak_normalize(buf, ceiling)
    # micro-fade edges (1 ms)
    fade = max(1, int(sr * 0.001))
    for i in range(fade):
        g = i / fade
        buf[i] *= g
        buf[-(i + 1)] *= g
    write_pcm16_mono(out, buf, sr)
    peak = max(abs(x) for x in buf) if buf else 0.0
    return {
        'path': str(out),
        'frames': len(buf),
        'samples_per_bar': samples_bar,
        'grid_total_frames': grid_total,
        'loop_samples_bars': file_len,
        'hits_placed': hits,
        'sample_peak_dbfs': 20.0 * math.log10(peak) if peak > 0 else -120.0,
        'bpm': bpm,
        'bars': bars,
    }


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--blueprint', required=True, type=Path)
    ap.add_argument('--kit', required=True, type=Path)
    ap.add_argument('--out', required=True, type=Path)
    ap.add_argument('--ceiling-dbfs', type=float, default=-0.1)
    ap.add_argument('--variant', type=int, default=0, help='master index offset per lane')
    args = ap.parse_args()
    bp = json.loads(args.blueprint.read_text(encoding='utf8'))
    meta = render_loop(bp, args.kit, args.out, args.ceiling_dbfs, variant=args.variant)
    meta['variant'] = args.variant
    print(json.dumps(meta, indent=2))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
