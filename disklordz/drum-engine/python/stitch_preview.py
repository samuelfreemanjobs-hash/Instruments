#!/usr/bin/env python3
"""Stitch one-shots into a 140 BPM kick-hat-snare-hat preview."""

from __future__ import annotations

import argparse
import shutil
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from pcmio import peak_normalize, read_wav, resample_linear, write_wav24


def pick(files: list[Path], token: str) -> Path | None:
    for path in files:
        if token in path.stem.lower():
            return path
    return None


def stitch(folder: Path, destination: Path, bpm: float, seconds: float, rate: int) -> None:
    files = sorted(folder.rglob("*.wav"))
    if not files:
        raise SystemExit(f"no wav files in {folder}")
    voices = {
        "kick": pick(files, "808") or pick(files, "kick"),
        "snare": pick(files, "snare") or pick(files, "clap"),
        "hat": pick(files, "hat"),
    }
    loaded = {}
    for name, path in voices.items():
        if path is None:
            continue
        samples, source_rate, _channels = read_wav(path)
        loaded[name] = resample_linear(samples, source_rate, rate)
    if "kick" not in loaded:
        loaded["kick"] = loaded[next(iter(loaded))]
    pattern = ["kick", "hat", "snare", "hat"]
    step = int(rate * (60.0 / bpm) / 4.0)
    mix = [0.0] * int(rate * seconds)
    for index in range(0, len(mix), step):
        token = pattern[(index // step) % len(pattern)]
        hit = loaded.get(token) or loaded["kick"]
        for offset, sample in enumerate(hit):
            at = index + offset
            if at >= len(mix):
                break
            mix[at] += sample
    write_wav24(destination, peak_normalize(mix, -1.0), rate)


def encode_mp3(wav: Path, mp3: Path) -> bool:
    ffmpeg = shutil.which("ffmpeg")
    if ffmpeg is None:
        return False
    result = subprocess.run(
        [ffmpeg, "-y", "-i", str(wav), "-codec:a", "libmp3lame", "-q:a", "4", str(mp3)],
        capture_output=True,
        text=True,
        check=False,
    )
    return result.returncode == 0 and mp3.exists()


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Build a beat-synced kit preview.")
    parser.add_argument("folder", type=Path)
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--bpm", type=float, default=140.0)
    parser.add_argument("--seconds", type=float, default=30.0)
    parser.add_argument("--rate", type=int, default=48000)
    parser.add_argument("--mp3", action="store_true")
    args = parser.parse_args(argv)
    stitch(args.folder, args.out, args.bpm, args.seconds, args.rate)
    mp3_ok = encode_mp3(args.out, args.out.with_suffix(".mp3")) if args.mp3 else False
    print(f"preview={args.out} mp3={mp3_ok}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
