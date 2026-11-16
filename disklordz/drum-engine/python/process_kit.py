#!/usr/bin/env python3
"""Turn a folder of drum bounces into a named 24-bit/48 kHz kit.

Catalog packs that already pass kit_qa.py (Memphis Vault is 44.1 kHz / 16-bit)
are a different product. Do not run this on that folder and call it the same SKU.
"""

from __future__ import annotations

import argparse
import json
import sys
import zipfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from pcmio import (
    peak_normalize,
    read_wav,
    resample_linear,
    soft_clip,
    trim_leading_zero_cross,
    write_wav24,
)


def kind_and_note(name: str) -> tuple[str, str]:
    stem = Path(name).stem
    lowered = stem.lower()
    kind = "Hit"
    for token, label in (
        ("808", "808"),
        ("kick", "Kick"),
        ("snare", "Snare"),
        ("clap", "Clap"),
        ("hat", "Hat"),
        ("perc", "Perc"),
        ("loop", "Loop"),
    ):
        if token in lowered:
            kind = label
            break
    note = "NA"
    for part in stem.replace("-", "_").split("_"):
        if len(part) >= 2 and part[0].isalpha() and part[-1].isdigit():
            note = part.upper()
    return kind, note


def master_file(source: Path, destination: Path, rate: int, peak_db: float, saturate: bool) -> dict:
    samples, source_rate, channels = read_wav(source)
    samples = trim_leading_zero_cross(samples)
    samples = resample_linear(samples, source_rate, rate)
    if saturate:
        samples = soft_clip(samples)
    kind, _note = kind_and_note(source.name)
    target = -1.0 if kind == "Loop" else peak_db
    samples = peak_normalize(samples, target)
    write_wav24(destination, samples, rate)
    return {
        "source": source.name,
        "file": destination.name,
        "channelsIn": channels,
        "rate": rate,
        "bits": 24,
        "peakDb": round(target, 3),
    }


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Master a folder of drum bounces into a commercial kit.")
    parser.add_argument("folder", type=Path)
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--brand", default="DLZ")
    parser.add_argument("--kit", default="808Lab")
    parser.add_argument("--rate", type=int, default=48000)
    parser.add_argument("--peak-db", type=float, default=-0.3, help="One-shot peak. Loops use -1.0 dBFS.")
    parser.add_argument("--saturate", action="store_true")
    parser.add_argument("--zip", action="store_true")
    return parser


def main(argv: list[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    sources = sorted(args.folder.rglob("*.wav"))
    if not sources:
        print(f"no wav files in {args.folder}", file=sys.stderr)
        return 1
    args.out.mkdir(parents=True, exist_ok=True)
    counters: dict[str, int] = {}
    report: list[dict] = []
    for source in sources:
        kind, note = kind_and_note(source.name)
        counters[kind] = counters.get(kind, 0) + 1
        name = f"{args.brand}_{args.kit}_{kind}_{counters[kind]:02d}_{note}.wav"
        report.append(master_file(source, args.out / name, args.rate, args.peak_db, args.saturate))
    manifest = {"format": "DISKLORDZ_KIT", "files": report, "humanAuditionRequired": True}
    (args.out / "manifest.json").write_text(json.dumps(manifest, indent=2), encoding="utf-8")
    archive = None
    if args.zip:
        archive = args.out.with_suffix(".zip")
        with zipfile.ZipFile(archive, "w", zipfile.ZIP_DEFLATED) as handle:
            for path in sorted(args.out.iterdir()):
                handle.write(path, arcname=f"{args.out.name}/{path.name}")
    print(json.dumps({"files": len(report), "out": str(args.out), "zip": str(archive) if archive else None}))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
