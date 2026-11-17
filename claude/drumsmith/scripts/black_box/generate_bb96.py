#!/usr/bin/env python3
"""BB96 wrapper — shared era generator."""
from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path[:0] = [str(ROOT), str(ROOT / "scripts")]

from scripts.black_box.generate_era import generate_all, kit_masters_dir, load_profile  # noqa: E402

# re-export helpers used by package_bb96 mini-kits
from scripts.black_box.generate_era import _clap, _cowbell  # noqa: E402


def main() -> None:
    profile = load_profile("bb96")
    out = kit_masters_dir(profile)
    rows = generate_all(out, profile)
    print(f"BB96 generated {len(rows)} samples → {out}")


if __name__ == "__main__":
    main()
