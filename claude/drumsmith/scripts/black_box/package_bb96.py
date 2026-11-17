#!/usr/bin/env python3
"""Package BLACK BOX 96 commercial zip from masters/."""

from __future__ import annotations

import argparse
import json
import shutil
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
MASTERS = ROOT / "kits/black-box-96/masters"
PACK_NAME = "Black_Box_96"
TITLE = "BLACK BOX 96"
SUBTITLE = "The Original Machine — Memphis Phonk / Southern Drum Kit"


def package(out_dir: Path, masters: Path) -> dict:
    if not masters.is_dir():
        raise FileNotFoundError(f"run generate_bb96.py first: missing {masters}")
    if out_dir.exists():
        shutil.rmtree(out_dir)
    pack = out_dir / PACK_NAME
    shutil.copytree(masters, pack)
    # drop internal manifest name clash
    docs = pack / "_DOCS"
    docs.mkdir(exist_ok=True)
    if (pack / "manifest.json").exists():
        shutil.move(str(pack / "manifest.json"), str(docs / "manifest.json"))
    (docs / "PRODUCT.txt").write_text(
        "\n".join(
            [
                TITLE,
                SUBTITLE,
                "Collection: DiskLordz Black Box — The Lost Drum Machine Collection",
                "SKU: BB96",
                "Category: Drum sample kit (oneshots + loops)",
                "Promise: Signature vintage drum-machine sound —",
                "  thick 808s · aggressive snares · metallic cowbells ·",
                "  crunchy hats · analog toms · dry claps",
                "Format: 44.1 kHz · PCM16 · mono",
                "Original synthesis only — not a hardware ROM dump.",
                "release_ready: FALSE until HITL",
                "",
            ]
        ),
        encoding="utf-8",
    )
    shutil.copy(ROOT / "kits/black-box-96/PRODUCT.md", docs / "PRODUCT.md")
    shutil.copy(ROOT / "kits/black-box-collection/PRODUCT_LINEUP.md", docs / "COLLECTION.md")
    n = sum(1 for _ in pack.rglob("*.wav"))
    manifest = {
        "title": TITLE,
        "identity": "The Original Machine",
        "sku": "BB96",
        "pack_dir": PACK_NAME,
        "release_ready": False,
        "wav_count": n,
        "delivery": {"sample_rate": 44100, "bit_depth": 16, "channels": 1},
    }
    (docs / "pack_manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    return manifest


def zip_pack(pack_root: Path, zip_path: Path) -> None:
    zip_path.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(zip_path, "w", compression=zipfile.ZIP_DEFLATED) as zf:
        for path in sorted(pack_root.rglob("*")):
            if path.is_file():
                zf.write(path, arcname=str(Path(PACK_NAME) / path.relative_to(pack_root)))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--masters", type=Path, default=MASTERS)
    parser.add_argument("--staging", type=Path, default=ROOT / "kits/black-box-96/pack_staging")
    parser.add_argument("--zip", type=Path, default=Path("/opt/cursor/artifacts/Black_Box_96.zip"))
    args = parser.parse_args()
    manifest = package(args.staging, args.masters)
    pack_root = args.staging / PACK_NAME
    zip_pack(pack_root, args.zip)
    # listen picks
    listen = Path("/opt/cursor/artifacts/Black_Box_96_listen")
    if listen.exists():
        shutil.rmtree(listen, ignore_errors=True)
    listen.mkdir(parents=True)
    picks = list((pack_root / "01_808_Subs").glob("*.wav"))[:1]
    picks += list((pack_root / "03_Snares").glob("*.wav"))[:1]
    picks += list((pack_root / "05_Hats_Closed").glob("*.wav"))[:1]
    picks += list((pack_root / "07_Cowbells").glob("*.wav"))[:1]
    picks += list((pack_root / "10_Loops").glob("*94bpm*"))[:1]
    picks += list((pack_root / "10_Loops").glob("*134bpm*"))[:1]
    picks += [pack_root / "_DOCS/PRODUCT.txt"]
    for p in picks:
        if p and p.is_file():
            shutil.copy2(p, listen / p.name)
    print(json.dumps(manifest, indent=2))
    print(f"zip → {args.zip} ({args.zip.stat().st_size} bytes)")


if __name__ == "__main__":
    main()
