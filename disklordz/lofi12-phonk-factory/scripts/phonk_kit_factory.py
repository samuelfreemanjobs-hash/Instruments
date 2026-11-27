#!/usr/bin/env python3
"""CLI — full DiskLordz drum kits (Phase 1)."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

SCRIPT_DIR = Path(__file__).resolve().parent
sys.path.insert(0, str(SCRIPT_DIR))

from drum_kit_builder import KIT_SIZES, build_kit  # noqa: E402
from factory_modes import FACTORY_MODE_IDS, resolve_factory_mode  # noqa: E402


def main() -> None:
    p = argparse.ArgumentParser(description="Build synthesized drum kit ZIP contents")
    p.add_argument("--mode", default="memphis_underground", help=f"Factory mode ({', '.join(FACTORY_MODE_IDS)})")
    p.add_argument("--out", type=Path, required=True)
    p.add_argument("--kit-size", default="full", choices=tuple(KIT_SIZES.keys()))
    p.add_argument("--seed", type=int, default=0)
    args = p.parse_args()
    mode = resolve_factory_mode(args.mode)
    manifest = build_kit(mode_id=mode, out_dir=args.out, kit_size=args.kit_size, seed=args.seed)
    print(json.dumps({"mode": mode, "out": str(args.out), "oneShots": manifest["oneShotCount"]}, indent=2))


if __name__ == "__main__":
    main()
