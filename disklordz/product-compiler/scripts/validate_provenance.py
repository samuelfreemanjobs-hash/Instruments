#!/usr/bin/env python3
"""Exit 1 if any asset in a compile manifest fails Sound DNA provenance rules."""

from __future__ import annotations

import json
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[3]
DNA_SCHEMA = REPO_ROOT / "disklordz/sound-dna/schema/sound-asset.schema.json"


def load_manifest(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def load_asset_meta(asset_id: str, library_root: Path) -> dict | None:
    meta_path = library_root / "assets" / asset_id / "meta.json"
    if not meta_path.is_file():
        return None
    return json.loads(meta_path.read_text(encoding="utf-8"))


def check_provenance(meta: dict) -> list[str]:
    errors: list[str] = []
    prov = meta.get("provenance")
    if not prov:
        errors.append("missing provenance block")
        return errors
    if prov.get("commercial_ok") is not True:
        errors.append("commercial_ok is not true")
    if not prov.get("license"):
        errors.append("missing license")
    return errors


def main() -> None:
    if len(sys.argv) < 2:
        print("Usage: validate_provenance.py <manifest.json> [sample-library-root]", file=sys.stderr)
        sys.exit(2)
    manifest_path = Path(sys.argv[1])
    library_root = Path(sys.argv[2]) if len(sys.argv) > 2 else REPO_ROOT / "sample-library"
    manifest = load_manifest(manifest_path)
    asset_ids = manifest.get("asset_ids") or []
    blocked = False
    for aid in asset_ids:
        meta = load_asset_meta(aid, library_root)
        if meta is None:
            print(f"BLOCK {aid}: meta.json not found under {library_root}")
            blocked = True
            continue
        errs = check_provenance(meta)
        if errs:
            print(f"BLOCK {aid}: " + "; ".join(errs))
            blocked = True
    if blocked:
        sys.exit(1)
    print("provenance_check: passed")
    sys.exit(0)


if __name__ == "__main__":
    main()
