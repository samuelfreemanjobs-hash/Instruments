#!/usr/bin/env python3
"""
DDSP 808 encoder — training loop with a folder of personal .wav drum samples.

Drop kicks/percussion into ``drums/`` (or pass ``--folder``). Resampling, mono,
padding/trim, and peak normalize are handled by ``DrumSampleDataset``.

Usage:
    python scripts/ddsp_training_loop.py
    python scripts/ddsp_training_loop.py --folder /path/to/kicks --epochs 50 --batch 8
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT))
DEFAULT_DRUMS = ROOT / "drums"


def main() -> int:
    parser = argparse.ArgumentParser(description="DDSP 808 encoder training on .wav folder")
    parser.add_argument(
        "--folder",
        type=Path,
        default=DEFAULT_DRUMS,
        help=f"Flat folder of .wav files (default: {DEFAULT_DRUMS})",
    )
    parser.add_argument("--epochs", type=int, default=20, help="Training epochs")
    parser.add_argument("--batch", type=int, default=4, help="Batch size")
    parser.add_argument("--lr", type=float, default=1e-3, help="Adam learning rate")
    parser.add_argument("--sr", type=int, default=44100, help="Target sample rate")
    parser.add_argument("--dur", type=float, default=2.0, help="Clip duration (seconds)")
    parser.add_argument(
        "--onnx",
        type=Path,
        default=ROOT / "artifacts" / "ddsp_808_encoder.onnx",
        help="Output ONNX path",
    )
    parser.add_argument("--workers", type=int, default=0, help="DataLoader worker processes")
    args = parser.parse_args()

    try:
        import torch  # noqa: F401
    except ImportError:
        print("Install train deps: pip install -r requirements-train.txt", file=sys.stderr)
        return 1

    from drum_synth_blueprint.torch_ddsp.pipeline import train_ddsp_808_from_folder

    metrics = train_ddsp_808_from_folder(
        args.folder,
        num_epochs=args.epochs,
        batch_size=args.batch,
        learning_rate=args.lr,
        sample_rate=args.sr,
        duration_sec=args.dur,
        num_workers=args.workers,
        export_path=args.onnx,
    )
    print(json.dumps(metrics, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
