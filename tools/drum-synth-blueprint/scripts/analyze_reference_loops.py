#!/usr/bin/env python3
"""Measure reference loops — BPM, sub%, centroid, RMS, dynamic range."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT))


def _dynamic_range_db(mono, eps: float = 1e-9) -> float:
    import numpy as np

    rms = np.sqrt(np.mean(mono**2) + eps)
    peak = float(np.max(np.abs(mono)) + eps)
    return float(20.0 * np.log10(peak / rms))


def analyze_file(path: Path, sr: int = 44100) -> dict:
    import librosa
    import numpy as np

    from drum_synth_blueprint.phonk_kit.slicer import _centroid_hz, _sub_ratio, load_mono_wav

    mono, file_sr = load_mono_wav(path, target_sr=sr)
    tempo, _ = librosa.beat.beat_track(y=mono, sr=sr)
    bpm = float(tempo) if np.ndim(tempo) == 0 else float(np.asarray(tempo).reshape(-1)[0])
    return {
        "file": path.name,
        "bpm_est": round(bpm, 1),
        "sub_pct_below_80hz": round(100.0 * _sub_ratio(mono, sr), 1),
        "centroid_hz": round(_centroid_hz(mono, sr), 1),
        "rms": round(float(np.sqrt(np.mean(mono**2))), 3),
        "dynamic_range_db": round(_dynamic_range_db(mono), 1),
    }


def main() -> int:
    parser = argparse.ArgumentParser(description="Analyze phonk reference loops")
    parser.add_argument(
        "--folder",
        type=Path,
        default=ROOT / "reference-loops",
        help="Folder of loop WAVs",
    )
    args = parser.parse_args()
    paths = sorted(p for p in args.folder.iterdir() if p.suffix.lower() == ".wav")
    if not paths:
        print(f"No .wav in {args.folder}", file=sys.stderr)
        return 1
    rows = [analyze_file(p) for p in paths]
    print(json.dumps(rows, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
