#!/usr/bin/env python3
"""
Slice Memphis phonk loops into labeled one-shot WAVs (Step 1 training data).

Example:
  pip install -r requirements-analysis.txt
  python scripts/slice_phonk_loops.py --input ./reference-loops --output ./drums/sliced
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT))


def main() -> int:
    parser = argparse.ArgumentParser(description="Onset-slice loops → labeled one-shots")
    parser.add_argument("--input", type=Path, required=True, help="Folder of loop WAVs")
    parser.add_argument("--output", type=Path, required=True, help="Output root (label subfolders)")
    parser.add_argument("--sr", type=int, default=44100)
    parser.add_argument("--manifest", type=Path, default=None, help="JSON manifest path")
    args = parser.parse_args()

    from drum_synth_blueprint.phonk_kit.slicer import load_mono_wav, slice_loop_to_hits, write_wav_24

    loops = sorted(p for p in args.input.iterdir() if p.suffix.lower() == ".wav")
    if not loops:
        print(f"No .wav in {args.input}", file=sys.stderr)
        return 1

    manifest: list[dict] = []
    total = 0
    for loop_path in loops:
        mono, sr = load_mono_wav(loop_path, target_sr=args.sr)
        hits = slice_loop_to_hits(mono, sr)
        stem = loop_path.stem
        for i, (clip, spec) in enumerate(hits):
            out_dir = args.output / spec.label.value
            out_name = f"{stem}_{i:03d}.wav"
            out_path = out_dir / out_name
            write_wav_24(out_path, clip, sr)
            total += 1
            manifest.append(
                {
                    "source_loop": str(loop_path),
                    "output": str(out_path),
                    "label": spec.label.value,
                    "confidence": round(spec.confidence, 3),
                    "start_sample": spec.start,
                    "end_sample": spec.end,
                }
            )

    manifest_path = args.manifest or (args.output / "slice_manifest.json")
    manifest_path.parent.mkdir(parents=True, exist_ok=True)
    manifest_path.write_text(json.dumps(manifest, indent=2), encoding="utf-8")
    print(json.dumps({"loops": len(loops), "slices": total, "manifest": str(manifest_path)}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
