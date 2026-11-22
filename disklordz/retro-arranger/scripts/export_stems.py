#!/usr/bin/env python3
"""CLI stem export without web server."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT))

from retro_arranger.models import ArrangementSpec  # noqa: E402
from retro_arranger.midi_writer import export_stems_to_dir  # noqa: E402
from retro_arranger.pipeline import compose  # noqa: E402


def main() -> int:
    p = argparse.ArgumentParser()
    p.add_argument("--out", type=Path, default=Path("out/retro-arranger"))
    p.add_argument("--bpm", type=int, default=98)
    p.add_argument("--key", default="A")
    p.add_argument("--scale", default="dorian")
    args = p.parse_args()
    spec = ArrangementSpec(bpm=args.bpm, key_root=args.key, scale=args.scale)
    result = compose(spec)
    paths = export_stems_to_dir(result, args.out)
    for name, path in sorted(paths.items()):
        print(path)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
