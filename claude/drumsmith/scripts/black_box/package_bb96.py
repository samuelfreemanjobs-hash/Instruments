#!/usr/bin/env python3
"""BB96 wrapper — shared era packager."""
from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path[:0] = [str(ROOT), str(ROOT / "scripts")]

from scripts.black_box.generate_era import kit_masters_dir, load_profile  # noqa: E402
from scripts.black_box.package_era import package, zip_pack  # noqa: E402


def main() -> None:
    profile = load_profile("bb96")
    masters = kit_masters_dir(profile)
    staging = masters.parent / "pack_staging"
    zip_path = Path("/opt/cursor/artifacts/Black_Box_96.zip")
    manifest = package(staging, masters, profile)
    zip_pack(staging / profile["pack_dir"], zip_path, profile["pack_dir"])
    print(json.dumps(manifest, indent=2))
    print(f"zip → {zip_path} ({zip_path.stat().st_size} bytes)")


if __name__ == "__main__":
    main()
