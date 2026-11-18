#!/usr/bin/env python3
"""Headless 2-bar Memphis loop renderer — synthesizes oneshots onto a blueprint grid.

P2 twin for OfflineAudio: no browser required. Mixes KICK/SNARE/HAT/CLAP (and a
simple cowbell) from MEMPHIS engines onto drumsmith-loop-blueprint-v1 JSON → PCM16 WAV.
Supports loop-only voice banks so loops are not identical to oneshot kits.
"""

from __future__ import annotations

import argparse
import json
import struct
import sys
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS = ROOT / "scripts"
for p in (str(ROOT), str(SCRIPTS)):
    if p not in sys.path:
        sys.path.insert(0, p)

from dsp_math import loop_samples  # noqa: E402
from scripts.memphis_oneshots.phonk_hat import generate_memphis_phonk_hat  # noqa: E402
from scripts.memphis_oneshots.phonk_kick import (  # noqa: E402
    generate_memphis_phonk_kick,
    write_wav_16bit_mono,
)
from scripts.memphis_oneshots.phonk_snare import generate_memphis_phonk_snare  # noqa: E402


def _cowbell(sample_rate: int = 44100, duration: float = 0.18, pitch: float = 1.0) -> np.ndarray:
    """Minimal dual-square 808-ish cowbell for loop beds."""
    n = int(round(sample_rate * duration))
    t = np.linspace(0.0, duration, n, endpoint=False)
    f1, f2 = 540.0 * pitch, 800.0 * pitch
    sq = 0.55 * np.sign(np.sin(2 * np.pi * f1 * t)) + 0.45 * np.sign(np.sin(2 * np.pi * f2 * t))
    env = np.exp(-t / 0.07)
    out = sq * env
    peak = float(np.max(np.abs(out))) or 1.0
    return (out / peak * 0.7).astype(np.float32)


def _soft_clap(sample_rate: int = 44100, seed: int = 0, duration: float = 0.12) -> np.ndarray:
    """Soft multi-hit clap for under-snare Horror Snap layers (not a hard oneshot clone)."""
    rng = np.random.default_rng(900 + seed)
    n = int(round(sample_rate * duration))
    t = np.linspace(0.0, duration, n, endpoint=False)
    noise = rng.uniform(-1.0, 1.0, n)
    amp = np.zeros_like(t)
    for off, g in ((0.0, 0.4), (0.009, 0.7), (0.018, 1.0), (0.03, 0.45)):
        idx = int(off * sample_rate)
        if idx < n:
            amp[idx:] += g * np.exp(-(t[idx:] - t[idx]) / 0.03)
    y = np.diff(noise * amp, prepend=0.0)
    peak = float(np.max(np.abs(y))) or 1.0
    return (y / peak * 0.55).astype(np.float32)


def _voice_buffers(
    seed: int = 0,
    *,
    loop_only: bool = False,
    eng: dict | None = None,
    voice: dict | None = None,
) -> dict[str, np.ndarray]:
    eng = eng or {}
    voice = voice or {}
    # Vary body per seed so looping the same blueprint does not clone the same kit
    kick_root = 43.65 * (2.0 ** (((seed % 7) - 3) / 12.0))
    # Pitched-down Memphis snare bodies (~155–185 Hz) for Paul/Juicy snap ear
    snare_root = 155.0 + (seed % 9) * 3.5
    hat_hz = 8600.0 + (seed % 11) * 180.0
    # Loop-only banks: different fundamentals / grit than oneshot defaults
    if loop_only:
        kick_root *= 0.94
        snare_root += 8.0
        hat_hz -= 600.0
        kick_dur, kick_decay, kick_click = 0.28, 0.14, 0.28
        eight_dur, eight_decay = 1.1, 0.55
        clip = 3.5 + (seed % 3) * 0.4
        bits = int(eng.get("bit_depth", 11))
        lofi = float(eng.get("lofi_sr", 22050))
    else:
        kick_dur, kick_decay, kick_click = 0.22, 0.11, 0.35
        eight_dur, eight_decay = 1.4, 0.65
        clip = 4.5 + (seed % 4) * 0.3
        bits = int(eng.get("bit_depth", 12))
        lofi = float(eng.get("lofi_sr", 26040))

    return {
        "KICK": generate_memphis_phonk_kick(
            root_freq=kick_root,
            duration=kick_dur,
            amp_decay=kick_decay,
            click_amount=kick_click,
            clip_drive_db=clip,
            bit_depth=bits,
            lofi_sr=lofi,
            seed=1000 + seed,
        ),
        "SNARE": generate_memphis_phonk_snare(
            membrane_root=snare_root,
            seed=660 + seed,
            clip_drive_db=float(eng.get("snare_clip_db", 5.0)),
            bit_depth=bits,
            lofi_sr=lofi,
        ),
        "CLAP": _soft_clap(seed=seed),
        "HAT": generate_memphis_phonk_hat(
            center_hz=hat_hz,
            seed=808 + seed,
            bit_depth=bits,
            lofi_sr=lofi if voice.get("hat_use_lofi", True) else 44100.0,
        ),
        "COWBELL": _cowbell(pitch=0.9 + (seed % 5) * 0.05),
        "808": generate_memphis_phonk_kick(
            root_freq=kick_root,
            duration=eight_dur,
            pitch_mod_depth=42.0 + (seed % 8),
            amp_decay=eight_decay,
            click_amount=0.45 if not loop_only else 0.3,
            clip_drive_db=5.0 if not loop_only else 3.5,
            bit_depth=bits,
            lofi_sr=lofi,
            seed=2000 + seed,
        ),
    }


def render_memphis_loop(
    blueprint: dict,
    *,
    ceiling_dbfs: float = -0.1,
    seed: int = 0,
    loop_only: bool = False,
    eng: dict | None = None,
    voice: dict | None = None,
) -> np.ndarray:
    """Return float32 mono mix for the blueprint."""
    bpm = float(blueprint["bpm"])
    bars = int(blueprint["bars"])
    sr = int(blueprint.get("sample_rate", 44100))
    grid = int(blueprint["grid_subdivisions_per_bar"])
    samples_bar = int(blueprint.get("samples_per_bar") or loop_samples(1, bpm, sr))
    file_len = loop_samples(bars, bpm, sr)
    mix_len = max(samples_bar * bars, file_len)
    step_len = samples_bar / float(grid)

    voices = _voice_buffers(seed=seed, loop_only=loop_only, eng=eng, voice=voice)
    buf = np.zeros(mix_len, dtype=np.float64)
    lanes = blueprint.get("lanes") or {}
    for kind, events in lanes.items():
        oneshot = voices.get(kind)
        if oneshot is None or not events:
            continue
        for ev in events:
            step = int(ev["step"])
            vel = float(ev.get("vel", 100)) / 127.0
            start = int(round(step * step_len))
            if start >= mix_len:
                continue
            end = min(mix_len, start + len(oneshot))
            sl = end - start
            buf[start:end] += oneshot[:sl].astype(np.float64) * vel

    peak = float(np.max(np.abs(buf))) or 1.0
    ceiling = 10.0 ** (ceiling_dbfs / 20.0)
    out = (buf / peak) * ceiling
    return out[:file_len].astype(np.float32)


def write_blueprint_midi(blueprint: dict, path: Path) -> None:
    """Write a minimal Type-0 SMF from blueprint lanes (GM drum map-ish)."""
    note_map = {"KICK": 36, "808": 35, "SNARE": 38, "CLAP": 39, "HAT": 42, "COWBELL": 56}
    bpm = float(blueprint["bpm"])
    bars = int(blueprint["bars"])
    grid = int(blueprint["grid_subdivisions_per_bar"])
    # 480 PPQN; quarter = 480; bar = 4 quarters
    ppqn = 480
    ticks_per_step = max(1, int(round((ppqn * 4) / grid)))
    tempo = int(round(60_000_000 / bpm))

    events: list[tuple[int, bytes]] = []
    # tempo meta
    events.append((0, bytes([0xFF, 0x51, 0x03, (tempo >> 16) & 0xFF, (tempo >> 8) & 0xFF, tempo & 0xFF])))
    for kind, lane in (blueprint.get("lanes") or {}).items():
        note = note_map.get(kind)
        if note is None:
            continue
        for ev in lane or []:
            step = int(ev["step"])
            vel = int(max(1, min(127, ev.get("vel", 100))))
            tick = step * ticks_per_step
            events.append((tick, bytes([0x99, note, vel])))
            events.append((tick + max(1, ticks_per_step // 2), bytes([0x89, note, 0x00])))
    events.sort(key=lambda x: x[0])
    # end of track
    end_tick = bars * grid * ticks_per_step
    events.append((end_tick, bytes([0xFF, 0x2F, 0x00])))

    def vlq(value: int) -> bytes:
        out = bytearray([value & 0x7F])
        value >>= 7
        while value:
            out.insert(0, (value & 0x7F) | 0x80)
            value >>= 7
        return bytes(out)

    track = bytearray()
    prev = 0
    for tick, payload in events:
        track.extend(vlq(tick - prev))
        track.extend(payload)
        prev = tick

    header = struct.pack(">4sIHHH", b"MThd", 6, 0, 1, ppqn)
    track_chunk = struct.pack(">4sI", b"MTrk", len(track)) + bytes(track)
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(header + track_chunk)


def main() -> None:
    parser = argparse.ArgumentParser(description="Render a Memphis kit loop from blueprint JSON")
    parser.add_argument(
        "--blueprint",
        type=Path,
        default=ROOT / "kits/converter-night/loops/cnv-dr660-94-a.json",
    )
    parser.add_argument("--out", type=Path, default=Path("/tmp/cnv_memphis_loop_94.wav"))
    parser.add_argument("--seed", type=int, default=0)
    parser.add_argument("--loop-only", action="store_true")
    args = parser.parse_args()

    bp = json.loads(args.blueprint.read_text(encoding="utf-8"))
    audio = render_memphis_loop(bp, seed=args.seed, loop_only=args.loop_only)
    write_wav_16bit_mono(args.out, audio, sample_rate=int(bp.get("sample_rate", 44100)))
    print(
        f"wrote {args.out} ({len(audio)} samples, bpm={bp.get('bpm')}, bars={bp.get('bars')})"
    )


if __name__ == "__main__":
    main()
