#!/usr/bin/env python3
"""Render DiskLordz Memphis Vault Vol. 1 — original one-shots, no sampled ROMs.

Run from the repo root:

    python disklordz/sound-factory/scripts/render_memphis_vault.py
"""

from __future__ import annotations

import hashlib
import json
import math
import shutil
import struct
import wave
import zipfile
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path

SR = 44100
PEAK = 0.89
PRODUCT = "Memphis Vault Vol. 1"
SKU = "memphis-vault-vol-1"
SEED = 884422

ROOT = Path(__file__).resolve().parents[1] / "kits" / SKU
COVER_SRC = Path(
    r"C:\Users\user\.cursor\projects\c-dev-Instruments\assets\image-4ed0a894-b2da-427d-b372-76c05d45fb81.jpg"
)


def note_hz(midi: int) -> float:
    return 440.0 * (2.0 ** ((midi - 69) / 12.0))


def note_name(midi: int) -> str:
    names = ("C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B")
    return f"{names[midi % 12]}{midi // 12 - 1}"


def hash_noise(i: int, seed: int) -> float:
    x = (i * 1664525 + seed * 1013904223 + 0x9E3779B9) & 0xFFFFFFFF
    x ^= x >> 16
    x = (x * 0x7FEB352D) & 0xFFFFFFFF
    x ^= x >> 15
    return ((x & 0xFFFF) / 32768.0) - 1.0


def tanh(x: float) -> float:
    if x > 8.0:
        return 1.0
    if x < -8.0:
        return -1.0
    return math.tanh(x)


def exp_decay(t: float, decay: float) -> float:
    return math.exp(-t / max(decay, 1e-4))


@dataclass
class Hit:
    folder: str
    filename: str
    instrument: str
    samples: list[float]
    key: str | None
    seed: int
    recipe: str


def clamp_finite(samples: list[float]) -> list[float]:
    out: list[float] = []
    for s in samples:
        if not math.isfinite(s):
            out.append(0.0)
        else:
            out.append(max(-4.0, min(4.0, s)))
    return out


def fade_edges(samples: list[float], in_n: int = 8, out_n: int = 64) -> None:
    n = len(samples)
    for i in range(min(in_n, n)):
        samples[i] *= i / in_n
    for i in range(min(out_n, n)):
        samples[n - 1 - i] *= i / out_n


def pitched_sine(seconds: float, freq_at) -> list[float]:
    n = int(SR * seconds)
    phase = 0.0
    out = [0.0] * n
    for i in range(n):
        t = i / SR
        f = max(20.0, min(12000.0, freq_at(t)))
        phase += f / SR
        out[i] = math.sin(2.0 * math.pi * phase)
    return out


def render_kick(midi: int, decay: float, click: float, drive: float, seed: int) -> list[float]:
    body = note_hz(midi)
    n = int(SR * min(0.7, decay * 4.5))
    phase = 0.0
    out = [0.0] * n
    for i in range(n):
        t = i / SR
        f = body * (1.0 + 7.5 * math.exp(-t * 38.0))
        phase += f / SR
        tone = math.sin(2.0 * math.pi * phase)
        transient = hash_noise(i, seed) * math.exp(-t / 0.0035)
        env = exp_decay(t, decay)
        out[i] = tanh((tone * 0.85 + transient * click) * drive) * env
    fade_edges(out)
    return out


def render_808(midi: int, decay: float, drop: float, drive: float, seed: int) -> list[float]:
    body = note_hz(midi)
    n = int(SR * min(2.4, decay * 3.2))
    phase = 0.0
    sub_phase = 0.0
    out = [0.0] * n
    for i in range(n):
        t = i / SR
        f = body * (1.0 + drop * math.exp(-t * 14.0))
        phase += f / SR
        sub_phase += (f * 0.5) / SR
        tone = math.sin(2.0 * math.pi * phase)
        sub = math.sin(2.0 * math.pi * sub_phase) * 0.35
        grit = hash_noise(i, seed) * 0.04 * math.exp(-t / 0.02)
        env = exp_decay(t, decay)
        out[i] = tanh((tone + sub + grit) * drive) * env
    fade_edges(out, out_n=128)
    return out


def render_snare(body_hz: float, snap: float, decay: float, drive: float, seed: int) -> list[float]:
    n = int(SR * 0.42)
    phase = 0.0
    out = [0.0] * n
    for i in range(n):
        t = i / SR
        phase += body_hz / SR
        tone = math.sin(2.0 * math.pi * phase) * exp_decay(t, decay)
        wire = hash_noise(i, seed) * snap * exp_decay(t, 0.045)
        if t > 0.008:
            wire += hash_noise(i, seed + 17) * snap * 0.45 * exp_decay(t - 0.008, 0.03)
        out[i] = tanh((tone * 0.55 + wire) * drive) * exp_decay(t, 0.16)
    fade_edges(out)
    return out


def render_clap(spread: float, decay: float, drive: float, seed: int) -> list[float]:
    n = int(SR * 0.36)
    bursts = (0.0, 0.010, 0.021 + spread * 0.01, 0.034 + spread * 0.016)
    out = [0.0] * n
    hp = 0.0
    prev = 0.0
    for i in range(n):
        t = i / SR
        burst = 0.0
        for off in bursts:
            if t >= off:
                burst += hash_noise(i, seed + int(off * 1000)) * exp_decay(t - off, 0.018)
        raw = burst
        hp = raw - prev + 0.96 * hp
        prev = raw
        out[i] = tanh(hp * drive) * exp_decay(t, decay)
    fade_edges(out)
    return out


def render_hat(open_hat: bool, bright: float, decay: float, seed: int) -> list[float]:
    seconds = 0.42 if open_hat else 0.09
    n = int(SR * seconds)
    ratios = (1.0, 1.37, 1.73, 2.19, 2.71, 3.33)
    base = 4800.0 + bright * 2200.0
    phases = [0.0] * len(ratios)
    out = [0.0] * n
    hp = 0.0
    prev = 0.0
    for i in range(n):
        t = i / SR
        metal = 0.0
        for k, ratio in enumerate(ratios):
            phases[k] += (base * ratio) / SR
            metal += math.sin(2.0 * math.pi * phases[k]) / (k + 1)
        raw = metal * 0.35 + hash_noise(i, seed) * (0.55 + bright)
        hp = raw - prev + 0.94 * hp
        prev = raw
        choke = 1.0 if open_hat else exp_decay(t, 0.02)
        out[i] = tanh(hp * 1.4) * exp_decay(t, decay) * choke
    fade_edges(out, out_n=32 if not open_hat else 64)
    return out


def render_cowbell(midi: int, decay: float, drive: float) -> list[float]:
    low = note_hz(midi)
    high = low * 1.505
    n = int(SR * min(0.7, decay * 3.0))
    p0 = 0.0
    p1 = 0.0
    out = [0.0] * n
    for i in range(n):
        t = i / SR
        p0 += low / SR
        p1 += high / SR
        a = math.sin(2.0 * math.pi * p0) + math.sin(6.0 * math.pi * p0) / 3.0
        b = math.sin(2.0 * math.pi * p1) + math.sin(6.0 * math.pi * p1) / 3.0
        env = exp_decay(t, decay)
        out[i] = tanh((a * 0.62 + b * 0.38) * drive) * env
    fade_edges(out)
    return out


def render_perc(kind: str, seed: int) -> list[float]:
    if kind == "rim":
        n = int(SR * 0.12)
        freq = 390 + (seed % 9) * 28
        out = [0.0] * n
        for i in range(n):
            t = i / SR
            env = exp_decay(t, 0.016 + (seed % 5) * 0.002)
            out[i] = (math.sin(2.0 * math.pi * freq * t) + 0.45 * math.sin(2.0 * math.pi * freq * 2 * t)) * env
        fade_edges(out, out_n=16)
        return out
    if kind == "block":
        n = int(SR * 0.16)
        out = [0.0] * n
        phase = 0.0
        for i in range(n):
            t = i / SR
            phase += 740 / SR
            out[i] = tanh(math.sin(2.0 * math.pi * phase) * 1.8) * exp_decay(t, 0.03)
        fade_edges(out, out_n=16)
        return out
    if kind == "click":
        n = int(SR * 0.04)
        out = [0.0] * n
        for i in range(n):
            t = i / SR
            out[i] = hash_noise(i, seed) * exp_decay(t, 0.004)
        fade_edges(out, in_n=1, out_n=8)
        return out
    if kind == "shaker":
        n = int(SR * 0.18)
        out = [0.0] * n
        for i in range(n):
            t = i / SR
            grain = abs(hash_noise(i, seed))
            out[i] = grain * hash_noise(i, seed + 3) * exp_decay(t, 0.05)
        fade_edges(out, out_n=24)
        return out
    n = int(SR * 0.2)
    out = [0.0] * n
    phase = 0.0
    for i in range(n):
        t = i / SR
        phase += 1320 / SR
        metal = math.sin(2.0 * math.pi * phase) * math.sin(2.0 * math.pi * 3.1 * phase)
        out[i] = tanh(metal * 2.0) * exp_decay(t, 0.04)
    fade_edges(out)
    return out


def render_fx(kind: str, seed: int) -> list[float]:
    n = int(SR * 0.7)
    out = [0.0] * n
    if kind == "tape-hiss":
        for i in range(n):
            t = i / SR
            out[i] = hash_noise(i, seed) * 0.35 * math.sin(math.pi * t / 0.7) ** 2
    elif kind == "noise-drop":
        hp = 0.0
        prev = 0.0
        for i in range(n):
            t = i / SR
            raw = hash_noise(i, seed)
            hp = raw - prev + 0.9 * hp
            prev = raw
            out[i] = tanh(hp * 2.2) * exp_decay(t, 0.12)
    elif kind == "reverse-noise":
        for i in range(n):
            t = i / SR
            out[i] = hash_noise(i, seed) * (t / 0.7) ** 2
    else:
        for i in range(n):
            t = i / SR
            burst = hash_noise(i, seed) * exp_decay(t % 0.08, 0.012)
            out[i] = tanh(burst * 3.0) * exp_decay(t, 0.2)
    fade_edges(out)
    return out


def build_hits() -> list[Hit]:
    hits: list[Hit] = []
    kick_notes = (36, 38, 41, 43, 45, 48, 40, 46)
    for i, midi in enumerate(kick_notes, start=1):
        decay = 0.09 + (i % 4) * 0.025
        hits.append(
            Hit(
                "01_Kicks",
                f"DLZ_MEM_KICK_{i:03d}_{note_name(midi)}.wav",
                "kick",
                render_kick(midi, decay, 0.35 + (i % 3) * 0.2, 1.4 + (i % 2) * 0.6, SEED + i),
                note_name(midi),
                SEED + i,
                "short body, pitch drop, click",
            )
        )
    bass_notes = (41, 43, 45, 48, 50, 53, 40, 46)
    for i, midi in enumerate(bass_notes, start=1):
        hits.append(
            Hit(
                "02_808s",
                f"DLZ_MEM_808_{i:03d}_{note_name(midi)}.wav",
                "808",
                render_808(midi, 0.55 + (i % 3) * 0.12, 0.35 + (i % 2) * 0.4, 1.6 + (i % 3) * 0.45, SEED + 100 + i),
                note_name(midi),
                SEED + 100 + i,
                "tuned sine, sub octave, pitch drop, drive",
            )
        )
    for i in range(1, 9):
        hits.append(
            Hit(
                "03_Snares",
                f"DLZ_MEM_SNARE_{i:03d}.wav",
                "snare",
                render_snare(150 + i * 14, 0.45 + (i % 4) * 0.18, 0.08 + (i % 3) * 0.03, 1.5 + (i % 2) * 0.7, SEED + 200 + i),
                None,
                SEED + 200 + i,
                "tuned body plus noise wires",
            )
        )
    for i in range(1, 7):
        hits.append(
            Hit(
                "04_Claps",
                f"DLZ_MEM_CLAP_{i:03d}.wav",
                "clap",
                render_clap(0.2 + i * 0.15, 0.08 + i * 0.012, 1.8 + (i % 3) * 0.4, SEED + 300 + i),
                None,
                SEED + 300 + i,
                "multi-burst filtered noise",
            )
        )
    for i in range(1, 7):
        hits.append(
            Hit(
                "05_Hats",
                f"DLZ_MEM_HAT_{i:03d}.wav",
                "hat_closed",
                render_hat(False, 0.2 + i * 0.12, 0.02 + i * 0.004, SEED + 400 + i),
                None,
                SEED + 400 + i,
                "metallic partials plus noise, closed",
            )
        )
    for i in range(1, 5):
        hits.append(
            Hit(
                "05_Hats",
                f"DLZ_MEM_OHAT_{i:03d}.wav",
                "hat_open",
                render_hat(True, 0.35 + i * 0.1, 0.08 + i * 0.02, SEED + 500 + i),
                None,
                SEED + 500 + i,
                "metallic partials plus noise, open",
            )
        )
    bell_notes = (65, 67, 69, 72, 74, 77)
    for i, midi in enumerate(bell_notes, start=1):
        hits.append(
            Hit(
                "06_Cowbells",
                f"DLZ_MEM_COWBELL_{i:03d}_{note_name(midi)}.wav",
                "cowbell",
                render_cowbell(midi, 0.12 + (i % 3) * 0.04, 1.7 + (i % 2) * 0.8),
                note_name(midi),
                SEED + 600 + i,
                "dual oscillator, lower partial labeled",
            )
        )
    perc_kinds = ("rim", "rim", "block", "click", "shaker", "metal")
    for i, kind in enumerate(perc_kinds, start=1):
        hits.append(
            Hit(
                "07_Percussion",
                f"DLZ_MEM_PERC_{i:03d}.wav",
                kind,
                render_perc(kind, SEED + 700 + i),
                None,
                SEED + 700 + i,
                kind,
            )
        )
    for i, kind in enumerate(("tape-hiss", "noise-drop", "reverse-noise", "stutter"), start=1):
        hits.append(
            Hit(
                "08_FX",
                f"DLZ_MEM_FX_{i:03d}.wav",
                kind,
                render_fx(kind, SEED + 800 + i),
                None,
                SEED + 800 + i,
                kind,
            )
        )
    return hits


def write_wav(path: Path, samples: list[float]) -> dict[str, float | int]:
    samples = clamp_finite(samples)
    peak = max(abs(s) for s in samples) or 1.0
    if peak < 1e-4:
        raise RuntimeError(f"silent render: {path.name}")
    scale = PEAK / peak
    path.parent.mkdir(parents=True, exist_ok=True)
    frames = bytearray()
    rms_acc = 0.0
    for s in samples:
        v = max(-1.0, min(1.0, s * scale))
        frames += struct.pack("<h", int(v * 32767))
        rms_acc += v * v
    with wave.open(str(path), "w") as handle:
        handle.setnchannels(1)
        handle.setsampwidth(2)
        handle.setframerate(SR)
        handle.writeframes(frames)
    return {
        "peak": PEAK,
        "rms": math.sqrt(rms_acc / len(samples)),
        "frames": len(samples),
        "seconds": round(len(samples) / SR, 4),
    }


def main() -> None:
    if ROOT.exists():
        shutil.rmtree(ROOT)
    ROOT.mkdir(parents=True)
    if not COVER_SRC.is_file():
        raise SystemExit(f"cover art missing: {COVER_SRC}")
    shutil.copyfile(COVER_SRC, ROOT / "cover.jpg")

    rendered = []
    created = datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
    for hit in build_hits():
        path = ROOT / hit.folder / hit.filename
        stats = write_wav(path, hit.samples)
        digest = hashlib.sha256(path.read_bytes()).hexdigest()
        rendered.append(
            {
                "file": f"{hit.folder}/{hit.filename}",
                "instrument": hit.instrument,
                "genre": "memphis-phonk",
                "key": hit.key,
                "bpm": None,
                "seed": hit.seed,
                "recipe": hit.recipe,
                "sampleRate": SR,
                "channels": 1,
                "bitDepth": 16,
                "seconds": stats["seconds"],
                "rms": round(float(stats["rms"]), 4),
                "sha256": digest,
                "date": created,
                "preset": SKU,
            }
        )
        print(f"{hit.filename:32} {stats['seconds']:.3f}s  rms {float(stats['rms']):.3f}")

    manifest = {
        "format": "DISKLORDZ_DRUM_KIT",
        "version": 1,
        "product": PRODUCT,
        "sku": SKU,
        "tagline": "Underground phonk samples",
        "origin": "original parametric synthesis",
        "sampleRate": SR,
        "channels": 1,
        "bitDepth": 16,
        "seed": SEED,
        "renderedAt": created,
        "cover": "cover.jpg",
        "count": len(rendered),
        "sounds": rendered,
        "license": "Original DiskLordz synthesis. Not sampled from records, machines, or third-party kits. Store license text is not final.",
    }
    (ROOT / "manifest.json").write_text(json.dumps(manifest, indent=2), encoding="utf-8")
    (ROOT / "README.txt").write_text(
        "\n".join(
            [
                "DISK LORDZ — MEMPHIS VAULT VOL. 1",
                "Underground phonk samples",
                "",
                "Original synthesized one-shots at 44.1 kHz, 16-bit, mono WAV.",
                "These are not recordings of a drum machine ROM or another sample pack.",
                "",
                "Folders",
                "  01_Kicks        short pitched kicks (key is the body fundamental)",
                "  02_808s         tuned bass drums (key is the sustained fundamental)",
                "  03_Snares       body plus noise wires (not key-labeled)",
                "  04_Claps        multi-burst noise (not key-labeled)",
                "  05_Hats         closed and open hats (not key-labeled)",
                "  06_Cowbells     dual oscillator (key is the lower partial)",
                "  07_Percussion   rims, block, click, shaker, metal (not key-labeled)",
                "  08_FX           texture one-shots (not key-labeled)",
                "",
                "Key names appear only where an oscillator fundamental was tuned on purpose.",
                "Pitch labels use scientific pitch: MIDI note 60 is C4.",
                "Render seed and recipe for each file are in manifest.json.",
                "",
                f"Sounds: {len(rendered)}",
                f"Rendered: {created}",
                "",
            ]
        ),
        encoding="utf-8",
    )

    zip_path = ROOT.parent / "Memphis-Vault-Vol-1.zip"
    if zip_path.exists():
        zip_path.unlink()
    with zipfile.ZipFile(zip_path, "w", compression=zipfile.ZIP_DEFLATED) as archive:
        for file in sorted(ROOT.rglob("*")):
            if file.is_file():
                archive.write(file, f"Memphis-Vault-Vol-1/{file.relative_to(ROOT).as_posix()}")
    print(f"\n{len(rendered)} sounds")
    print(ROOT)
    print(zip_path)


if __name__ == "__main__":
    main()
