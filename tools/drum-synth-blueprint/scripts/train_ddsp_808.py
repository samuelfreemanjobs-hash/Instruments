#!/usr/bin/env python3
"""Train DDSP 808 encoder and export ddsp_808_encoder.onnx."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT))


def main() -> int:
    parser = argparse.ArgumentParser(description="DDSP 808 PyTorch train + ONNX export")
    parser.add_argument("--steps", type=int, default=60)
    parser.add_argument(
        "--duration-sec",
        type=float,
        default=2.0,
        help="Teacher/synth clip length for training smoke",
    )
    parser.add_argument(
        "--export",
        type=Path,
        default=Path("artifacts/ddsp_808_encoder.onnx"),
        help="Output ONNX path (default under tools/drum-synth-blueprint/artifacts/)",
    )
    parser.add_argument("--smoke-only", action="store_true", help="Skip ONNX export")
    args = parser.parse_args()

    try:
        import torch  # noqa: F401
    except ImportError:
        print("Install train deps: pip install -r requirements-train.txt", file=sys.stderr)
        return 1

    from drum_synth_blueprint.torch_ddsp.pipeline import export_ddsp_808_encoder_onnx, train_ddsp_808_smoke

    metrics = train_ddsp_808_smoke(steps=args.steps, duration_sec=args.duration_sec)
    if not args.smoke_only:
        out = export_ddsp_808_encoder_onnx(args.export)
        metrics["onnx_path"] = str(out.resolve())
    print(json.dumps(metrics, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
