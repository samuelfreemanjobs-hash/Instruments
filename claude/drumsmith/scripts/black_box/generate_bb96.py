#!/usr/bin/env python3
"""CLI shim → generate_era.py bb96."""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT))

from scripts.black_box.generate_era import generate_all, kit_masters_dir, load_profile  # noqa: E402


def main() -> None:
    profile = load_profile("bb96")
    out = Path(sys.argv[1]) if len(sys.argv) > 1 else kit_masters_dir(profile)
    rows = generate_all(out, profile)
    print(f"BB96 generated {len(rows)} → {out}")


if __name__ == "__main__":
    main()
