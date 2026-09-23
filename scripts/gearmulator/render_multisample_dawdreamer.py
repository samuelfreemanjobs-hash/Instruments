#!/usr/bin/env python3
"""Batch-render a note grid via DawDreamer (VST3/VST host in Python).

Requires: pip install -r gearmulator-lane/requirements-dawdreamer.txt

Example:
  python3 scripts/gearmulator/render_multisample_dawdreamer.py \\
    --plugin build/Wave909/Wave909_artefacts/Release/VST3/WAVE-909.vst3 \\
    --instrument-label wave909_lead --name wave909_smoke --note-start 60 --note-end 63
"""
from __future__ import annotations

import argparse
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from _dawdreamer_session import render_session_grid


def main() -> int:
    repo_root = Path(__file__).resolve().parents[2]
    parser = argparse.ArgumentParser(description="DawDreamer multisample grid")
    parser.add_argument("--plugin", type=Path, required=True)
    parser.add_argument("--out-dir", type=Path, default=repo_root / "gearmulator-lane/multisamples/out")
    parser.add_argument("--name", default="dawdreamer_capture", help="Session folder suffix")
    parser.add_argument("--instrument-label", default="", help="Name for your new preset/instrument SKU")
    parser.add_argument("--program", type=int, default=0)
    parser.add_argument("--preset", type=Path, default=None, help=".fxp or .vstpreset")
    parser.add_argument("--note-start", type=int, default=36)
    parser.add_argument("--note-end", type=int, default=84)
    parser.add_argument("--note-step", type=int, default=3)
    parser.add_argument("--velocity", type=int, default=100)
    parser.add_argument("--seconds", type=float, default=3.0)
    parser.add_argument("--sample-rate", type=int, default=48000)
    parser.add_argument("--block-size", type=int, default=512)
    args = parser.parse_args()

    if not args.plugin.exists():
        print(f"Plugin not found: {args.plugin}", file=sys.stderr)
        return 1

    label = args.instrument_label or args.name
    session = f"{args.name}_p{args.program}_v{args.velocity}"
    dest = args.out_dir / session

    print(f"Rendering session {session} -> {dest}")
    manifest = render_session_grid(
        args.plugin,
        dest,
        program=args.program,
        note_start=args.note_start,
        note_end=args.note_end,
        note_step=args.note_step,
        velocity=args.velocity,
        seconds=args.seconds,
        sample_rate=args.sample_rate,
        block_size=args.block_size,
        preset_path=args.preset,
        instrument_label=label,
        source_plugin=str(args.plugin),
    )
    print(f"Done. Manifest: {manifest}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
