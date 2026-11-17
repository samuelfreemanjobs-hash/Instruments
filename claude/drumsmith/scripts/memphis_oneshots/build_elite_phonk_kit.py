#!/usr/bin/env python3
"""Build an elite commercial-preview phonk drum kit (original synthesis).

Implements competitive research ROI #1–#2 / #4:
- Parallel RAW / TAPE / LOFI of the same core drums
- Matched mini-kits (oneshots + BPM-labeled loop)
- Cowbell oneshots for drift crossover

Output is a listen/HITL zip layout — release_ready stays false.
"""

from __future__ import annotations

import argparse
import json
import math
import shutil
import sys
import zipfile
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
from scripts.memphis_oneshots.render_memphis_loop import render_memphis_loop  # noqa: E402

KIT_NAME = "CNV_Elite_Phonk_Preview"


def _process_lane(buf: np.ndarray, lane: str, sample_rate: int = 44100) -> np.ndarray:
    """Derive TAPE / LOFI siblings from a RAW oneshot (same hit, parallel banks)."""
    x = buf.astype(np.float64)
    if lane == "RAW":
        out = x
    elif lane == "TAPE":
        # Soft saturate + darker LPF (~10.5 kHz)
        driven = np.tanh(x * (10.0 ** (3.0 / 20.0)))
        alpha = math.exp(-2.0 * math.pi * 10500.0 / sample_rate)
        filtered = np.zeros_like(driven)
        for i in range(1, len(driven)):
            filtered[i] = (1 - alpha) * driven[i] + alpha * filtered[i - 1]
        out = filtered
    elif lane == "LOFI":
        # Extra 10-bit hold + 22.05 kHz alias + clip
        step = sample_rate / 22050.0
        idx = (np.floor(np.arange(len(x)) / step) * step).astype(int)
        idx = np.clip(idx, 0, len(x) - 1)
        held = x[idx]
        q = 2 ** 9
        quantized = np.round(held * q) / q
        out = np.tanh(quantized * (10.0 ** (5.0 / 20.0)))
    else:
        raise ValueError(lane)
    peak = float(np.max(np.abs(out))) or 1.0
    return ((out / peak) * (10.0 ** (-0.1 / 20.0))).astype(np.float32)


def _cowbell(duration: float = 0.2, pitch: float = 1.0, sample_rate: int = 44100) -> np.ndarray:
    n = int(round(sample_rate * duration))
    t = np.linspace(0.0, duration, n, endpoint=False)
    f1, f2 = 540.0 * pitch, 800.0 * pitch
    sq = 0.55 * np.sign(np.sin(2 * np.pi * f1 * t)) + 0.45 * np.sign(np.sin(2 * np.pi * f2 * t))
    env = np.exp(-t / 0.08)
    out = sq * env
    peak = float(np.max(np.abs(out))) or 1.0
    return (out / peak * 0.75).astype(np.float32)


def _clap(seed: int = 1, sample_rate: int = 44100) -> np.ndarray:
    rng = np.random.default_rng(seed)
    n = int(0.12 * sample_rate)
    t = np.linspace(0.0, 0.12, n, endpoint=False)
    noise = rng.uniform(-1, 1, n)
    amp = np.zeros_like(t)
    for off_ms, g in ((0.0, 0.5), (0.012, 0.8), (0.028, 1.0)):
        i = int(off_ms * sample_rate)
        amp[i:] += g * np.exp(-(t[i:] - t[i]) / 0.035)
    # HPF-ish via diff
    y = np.diff(noise * amp, prepend=0.0)
    peak = float(np.max(np.abs(y))) or 1.0
    return (y / peak * 0.85).astype(np.float32)


MINI_KITS = [
    {
        "id": "01_ConverterCrawl",
        "bpm": 94,
        "kick_key_hz": 43.65,
        "snare_hz": 185.0,
        "hat_hz": 8600.0,
        "cow_pitch": 0.92,
        "blueprint": ROOT / "kits/converter-night/loops/cnv-dr660-94-a.json",
    },
    {
        "id": "02_BitcrushBed",
        "bpm": 94,
        "kick_key_hz": 36.71,
        "snare_hz": 220.0,
        "hat_hz": 9800.0,
        "cow_pitch": 1.0,
        "blueprint": ROOT / "kits/converter-night/loops/cnv-arch-bitcrush.json",
    },
    {
        "id": "03_HorrorSnap",
        "bpm": 94,
        "kick_key_hz": 41.20,
        "snare_hz": 196.0,
        "hat_hz": 7800.0,
        "cow_pitch": 0.85,
        "blueprint": ROOT / "kits/converter-night/loops/cnv-arch-horror.json",
    },
    {
        "id": "04_ClubBounce",
        "bpm": 134,
        "kick_key_hz": 46.25,
        "snare_hz": 233.0,
        "hat_hz": 10500.0,
        "cow_pitch": 1.08,
        "blueprint": ROOT / "kits/converter-night/loops/cnv-arch-bounce.json",
    },
]


def build_kit(out_dir: Path) -> dict:
    if out_dir.exists():
        shutil.rmtree(out_dir)
    parallel = out_dir / "01_PARALLEL_PROCESS_BANKS"
    mini_root = out_dir / "02_MATCHED_MINI_KITS"
    docs = out_dir / "_DOCS"
    for p in (parallel, mini_root, docs):
        p.mkdir(parents=True)

    # Core voice RAW seeds (shared across lanes)
    raw_voices = {
        "KICK": generate_memphis_phonk_kick(root_freq=43.65, clip_drive_db=4.0),
        "SNR": generate_memphis_phonk_snare(membrane_root=220.0, seed=660),
        "CH": generate_memphis_phonk_hat(center_hz=9000.0, seed=808),
        "CLAP": _clap(seed=42),
        "COW": _cowbell(pitch=1.0),
    }

    files: list[dict] = []
    for lane in ("RAW", "TAPE", "LOFI"):
        lane_dir = parallel / lane
        for role, buf in raw_voices.items():
            processed = _process_lane(buf, lane)
            folder = {
                "KICK": "Kicks",
                "SNR": "Snares",
                "CH": "Hi-Hats_Closed",
                "CLAP": "Claps",
                "COW": "Cowbells",
            }[role]
            name = f"CNV_{lane}_{role}_01.wav"
            path = lane_dir / folder / name
            write_wav_16bit_mono(path, processed)
            files.append({"lane": lane, "role": role, "path": str(path.relative_to(out_dir))})

    # Matched mini-kits
    for kit in MINI_KITS:
        kdir = mini_root / kit["id"]
        oneshots = kdir / "OneShots"
        oneshots.mkdir(parents=True)
        kick = generate_memphis_phonk_kick(root_freq=kit["kick_key_hz"], clip_drive_db=4.5)
        snare = generate_memphis_phonk_snare(membrane_root=kit["snare_hz"], seed=700 + kit["bpm"])
        hat = generate_memphis_phonk_hat(center_hz=kit["hat_hz"], seed=900 + kit["bpm"])
        clap = _clap(seed=kit["bpm"])
        cow = _cowbell(pitch=kit["cow_pitch"])
        for role, buf in (
            ("Kick", kick),
            ("Snare", snare),
            ("HatClosed", hat),
            ("Clap", clap),
            ("Cowbell", cow),
        ):
            path = oneshots / f"CNV_{kit['id']}_{role}.wav"
            write_wav_16bit_mono(path, buf)
            files.append({"mini": kit["id"], "role": role, "path": str(path.relative_to(out_dir))})

        bp = json.loads(kit["blueprint"].read_text(encoding="utf-8"))
        loop = render_memphis_loop(bp, seed=kit["bpm"])
        loop_name = f"CNV_{kit['id']}_LOOP_{kit['bpm']}bpm.wav"
        loop_path = kdir / loop_name
        write_wav_16bit_mono(loop_path, loop)
        files.append(
            {
                "mini": kit["id"],
                "role": "LOOP",
                "bpm": kit["bpm"],
                "frames": len(loop),
                "expected_frames": loop_samples(int(bp["bars"]), float(bp["bpm"]), 44100),
                "path": str(loop_path.relative_to(out_dir)),
            }
        )

    readme = docs / "README.txt"
    readme.write_text(
        "\n".join(
            [
                f"{KIT_NAME}",
                "Original DiskLordz Memphis / phonk drum kit preview (Converter Night).",
                "",
                "Commercial SKU = drum sample kit (oneshots + loops).",
                "NOT a Boss/E-mu ROM dump. Original synthesis only.",
                "Artist names are not used on storefront filenames.",
                "",
                "Structure (competitor-aligned):",
                "  01_PARALLEL_PROCESS_BANKS/RAW|TAPE|LOFI  — same hits, three process lanes",
                "  02_MATCHED_MINI_KITS/                    — oneshots + BPM-labeled loop",
                "",
                "Format: 44.1 kHz · PCM16 · mono",
                "release_ready: FALSE until HITL (ear / price / cover / legal)",
                "",
                "See docs/RESEARCH_ELITE_PHONK_KIT.md for competitive rationale.",
                "",
            ]
        ),
        encoding="utf-8",
    )
    shutil.copy(
        ROOT / "kits/converter-night/docs/ReadMe_License.txt",
        docs / "License.txt",
    )

    manifest = {
        "sku": "CNV",
        "pack": KIT_NAME,
        "release_ready": False,
        "delivery": {"sample_rate": 44100, "bit_depth": 16, "channels": 1},
        "competitive_features": [
            "parallel_RAW_TAPE_LOFI",
            "matched_mini_kits",
            "bpm_in_loop_filenames",
            "cowbell_oneshots",
        ],
        "file_count": len(files),
        "files": files,
    }
    (out_dir / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    return manifest


def zip_kit(out_dir: Path, zip_path: Path) -> None:
    zip_path.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(zip_path, "w", compression=zipfile.ZIP_DEFLATED) as zf:
        for path in sorted(out_dir.rglob("*")):
            if path.is_file():
                zf.write(path, arcname=str(Path(KIT_NAME) / path.relative_to(out_dir)))


def main() -> None:
    parser = argparse.ArgumentParser(description="Build elite phonk kit preview pack")
    parser.add_argument(
        "--out",
        type=Path,
        default=ROOT / "kits/converter-night/banks/elite_phonk_preview",
    )
    parser.add_argument(
        "--zip",
        type=Path,
        default=Path("/opt/cursor/artifacts/CNV_Elite_Phonk_Preview.zip"),
    )
    args = parser.parse_args()
    manifest = build_kit(args.out)
    zip_kit(args.out, args.zip)
    print(f"built {manifest['file_count']} files → {args.out}")
    print(f"zip → {args.zip} ({args.zip.stat().st_size} bytes)")


if __name__ == "__main__":
    main()
