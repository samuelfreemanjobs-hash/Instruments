#!/usr/bin/env python3
"""Write musically bounded factory presets and an SFZ map from WAV filenames."""

from __future__ import annotations

import argparse
import json
import random
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

PRESETS = (
    ("Heavy Sub 808s", {"kickBaseHz": 36.7, "kickPitchEnvHz": 160.0, "kickAmpDecaySec": 1.2, "kickDrive": 3.4, "kickClick": 0.18}),
    ("Crispy Trap Snares", {"snareToneHz": 220.0, "snareNoise": 0.62, "snareDecaySec": 0.22, "snareDrive": 2.1}),
    ("Lo-Fi Dusty Hats", {"hatToneHz": 290.0, "hatDecaySec": 0.04, "hatDrive": 1.3}),
    ("Open Hat Wash", {"hatToneHz": 340.0, "hatDecaySec": 0.42, "hatDrive": 1.2}),
    ("Tight Claps", {"clapSpacingSec": 0.010, "clapTailSec": 0.14, "clapDrive": 1.8}),
)


def clamp(value: float, low: float, high: float) -> float:
    return max(low, min(high, value))


def load_ranges(path: Path) -> dict[str, tuple[float, float]]:
    document = json.loads(path.read_text(encoding="utf-8"))
    return {item["id"]: (float(item["min"]), float(item["max"])) for item in document["parameters"]}


def write_presets(params: Path, out: Path, seed: int) -> int:
    ranges = load_ranges(params)
    rng = random.Random(seed)
    out.mkdir(parents=True, exist_ok=True)
    written = 0
    for index, (name, base) in enumerate(PRESETS, start=1):
        values = {}
        for key, (low, high) in ranges.items():
            center = base.get(key, (low + high) / 2.0)
            spread = (high - low) * 0.04
            values[key] = round(clamp(center + rng.uniform(-spread, spread), low, high), 4)
        payload = {"name": name, "category": name.split()[0], "parameters": values}
        (out / f"{index:02d}-{name.lower().replace(' ', '-')}.json").write_text(
            json.dumps(payload, indent=2), encoding="utf-8"
        )
        written += 1
    return written


def write_sfz(folder: Path, destination: Path) -> int:
    regions: list[str] = []
    velocity_re = re.compile(r"_v(\d+)", re.IGNORECASE)
    robin_re = re.compile(r"_rr(\d+)", re.IGNORECASE)
    note_re = re.compile(r"_([A-G]#?\d)(?:_|\.|$)", re.IGNORECASE)
    note_map = {"C1": 36, "D1": 38, "F#1": 42, "G1": 49}
    for wav in sorted(folder.rglob("*.wav")):
        stem = wav.stem
        note = 36
        match = note_re.search(stem + ".")
        if match:
            note = note_map.get(match.group(1).upper().replace("#", "#"), note)
        velocity = velocity_re.search(stem)
        if velocity:
            layer = int(velocity.group(1))
            lovel, hivel = (1, 63) if layer <= 1 else (64, 127)
        else:
            lovel, hivel = 1, 127
        robin = robin_re.search(stem)
        sequence = f" seq_position={robin.group(1)}" if robin else ""
        regions.append(
            f"<region> sample={wav.name} key={note} lovel={lovel} hivel={hivel}{sequence}"
        )
    destination.write_text("<group>\n" + "\n".join(regions) + "\n", encoding="utf-8")
    return len(regions)


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Generate factory presets and an SFZ keymap.")
    parser.add_argument("--params", type=Path, default=Path(__file__).resolve().parents[1] / "params.json")
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--seed", type=int, default=42)
    parser.add_argument("--sfz-from", type=Path)
    parser.add_argument("--sfz-out", type=Path)
    args = parser.parse_args(argv)
    count = write_presets(args.params, args.out, args.seed)
    sfz_count = 0
    if args.sfz_from and args.sfz_out:
        sfz_count = write_sfz(args.sfz_from, args.sfz_out)
    print(json.dumps({"presets": count, "sfzRegions": sfz_count}))
    if count == 0:
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
