#!/usr/bin/env python3
"""DIRTY CIRCUITS + BB96 Dirty Circuits Edition from BB96 masters."""

from __future__ import annotations

import argparse
import json
import shutil
import sys
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
for p in (str(ROOT), str(ROOT / "scripts")):
    if p not in sys.path:
        sys.path.insert(0, p)

from scripts.black_box.process_fx import (  # noqa: E402
    bitcrush,
    cassette_fx,
    process_lane,
    read_wav_mono,
    write_wav_mono,
)

BB96_MASTERS = ROOT / "kits/black-box-96/masters"
DC_PROFILE = ROOT / "kits/black-box-collection/profiles/dirty_circuits.json"


def _dirty(buf, mode: str, seed: int = 0):
    if mode == "CLEAN":
        return process_lane(buf, "RAW", seed=seed)
    if mode == "DIRTY":
        x = bitcrush(buf, bit_depth=8, hold_sr=16000.0)
        return cassette_fx(x, drive_db=7.0, tape_lpf_hz=9000.0, hiss_db=-36.0, seed=seed)
    if mode == "RUINED":
        x = bitcrush(buf, bit_depth=6, hold_sr=11025.0)
        return cassette_fx(x, drive_db=10.0, tape_lpf_hz=7000.0, wow_depth_ms=0.8, hiss_db=-30.0, seed=seed + 7)
    raise ValueError(mode)


def _pick_cores(masters: Path, per_folder: int = 6) -> list[Path]:
    picks: list[Path] = []
    for folder in sorted(p for p in masters.iterdir() if p.is_dir() and "loop" not in p.name.lower()):
        picks.extend(sorted(folder.glob("*.wav"))[:per_folder])
    return picks


def build_dirty_circuits(out_dir: Path, masters: Path) -> dict:
    if out_dir.exists():
        shutil.rmtree(out_dir)
    pack = out_dir / "Dirty_Circuits"
    docs = pack / "_DOCS"
    docs.mkdir(parents=True)
    files = 0
    cores = _pick_cores(masters, per_folder=5)
    for mode in ("CLEAN", "DIRTY", "RUINED"):
        mode_dir = pack / "01_PROCESS_MODES" / mode
        mode_dir.mkdir(parents=True)
        for i, path in enumerate(cores):
            role = path.parent.name
            out = mode_dir / role
            out.mkdir(parents=True, exist_ok=True)
            write_wav_mono(out / f"BBDC_{mode}_{path.stem}.wav", _dirty(read_wav_mono(path), mode, seed=i))
            files += 1

    # exclusives: ruined aliases of snares/hats
    excl = pack / "02_EXCLUSIVES_RUINED"
    excl.mkdir(parents=True)
    for path in cores:
        if any(x in path.parent.name.lower() for x in ("snare", "hat", "808", "kick")):
            write_wav_mono(
                excl / f"BBDC_EX_{path.stem}_ruined.wav",
                _dirty(read_wav_mono(path), "RUINED", seed=99),
            )
            files += 1

    (docs / "Hardware_Processing_Guide.md").write_text(
        """# DIRTY CIRCUITS — processing guide

Corruption bus for DiskLordz Black Box.

| Folder | Use |
|--------|-----|
| `01_PROCESS_MODES/CLEAN` | Era core as reference |
| `01_PROCESS_MODES/DIRTY` | Extra crush + cassette desk |
| `01_PROCESS_MODES/RUINED` | Extreme alias + drive + wow |
| `02_EXCLUSIVES_RUINED` | One-off ruined exclusives |

A/B CLEAN → DIRTY → RUINED of the same stem. Not a ROM dump.
""",
        encoding="utf-8",
    )
    (docs / "PRODUCT.txt").write_text(
        "DIRTY CIRCUITS — Corrupted Machine\nSKU: BBDC\nrelease_ready: FALSE\n",
        encoding="utf-8",
    )
    shutil.copy(ROOT / "kits/black-box-collection/PRODUCT_LINEUP.md", docs / "COLLECTION.md")
    manifest = {
        "sku": "BBDC",
        "title": "DIRTY CIRCUITS",
        "wav_count": files,
        "release_ready": False,
        "modes": ["CLEAN", "DIRTY", "RUINED"],
    }
    (docs / "pack_manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    return manifest


def build_bb96dc(out_dir: Path, masters: Path) -> dict:
    if out_dir.exists():
        shutil.rmtree(out_dir)
    pack = out_dir / "Black_Box_96_Dirty_Circuits"
    docs = pack / "_DOCS"
    docs.mkdir(parents=True)
    files = 0
    cores = _pick_cores(masters, per_folder=8)
    pairs = pack / "01_CLEAN_TO_DIRTY_PAIRS"
    for path in cores:
        role = path.parent.name
        stem = path.stem
        for mode in ("CLEAN", "DIRTY"):
            out = pairs / role / mode
            out.mkdir(parents=True, exist_ok=True)
            write_wav_mono(out / f"BB96DC_{mode}_{stem}.wav", _dirty(read_wav_mono(path), mode))
            files += 1

    excl = pack / "02_EXCLUSIVES"
    excl.mkdir(parents=True)
    for path in cores[:24]:
        write_wav_mono(
            excl / f"BB96DC_EX_{path.stem}_ruined.wav",
            _dirty(read_wav_mono(path), "RUINED"),
        )
        files += 1

    (docs / "Hardware_Processing_Guide.md").write_text(
        """# BLACK BOX 96: Dirty Circuits Edition

Expanded flagship: clean-to-dirty pairs of BB96 cores + ruined exclusives.

1. Load a CLEAN hit from `01_CLEAN_TO_DIRTY_PAIRS/.../CLEAN/`
2. Layer or swap with matching DIRTY sibling
3. Use `02_EXCLUSIVES` for ruined one-offs

See collection `SONIC_DNA.md` Dirty Circuits bus.
""",
        encoding="utf-8",
    )
    (docs / "PRODUCT.txt").write_text(
        "BLACK BOX 96: Dirty Circuits Edition\nSKU: BB96DC\nExpanded Flagship\nrelease_ready: FALSE\n",
        encoding="utf-8",
    )
    shutil.copy(ROOT / "kits/black-box-collection/PRODUCT_LINEUP.md", docs / "COLLECTION.md")
    manifest = {
        "sku": "BB96DC",
        "title": "BLACK BOX 96: Dirty Circuits Edition",
        "wav_count": files,
        "release_ready": False,
    }
    (docs / "pack_manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    return manifest


def zip_dir(src: Path, zip_path: Path, arc_root: str) -> None:
    zip_path.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(zip_path, "w", compression=zipfile.ZIP_DEFLATED) as zf:
        for path in sorted(src.rglob("*")):
            if path.is_file():
                zf.write(path, arcname=str(Path(arc_root) / path.relative_to(src)))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--masters", type=Path, default=BB96_MASTERS)
    parser.add_argument("--staging", type=Path, default=ROOT / "kits/black-box-dirty-circuits/pack_staging")
    args = parser.parse_args()
    if not args.masters.is_dir():
        raise SystemExit("BB96 masters required — run generate_era.py bb96 first")

    dc_stage = args.staging / "dc"
    bb96dc_stage = args.staging / "bb96dc"
    m1 = build_dirty_circuits(dc_stage, args.masters)
    m2 = build_bb96dc(bb96dc_stage, args.masters)
    z1 = Path("/opt/cursor/artifacts/Dirty_Circuits.zip")
    z2 = Path("/opt/cursor/artifacts/Black_Box_96_Dirty_Circuits.zip")
    zip_dir(dc_stage / "Dirty_Circuits", z1, "Dirty_Circuits")
    zip_dir(bb96dc_stage / "Black_Box_96_Dirty_Circuits", z2, "Black_Box_96_Dirty_Circuits")
    print(json.dumps({"BBDC": m1, "BB96DC": m2, "zips": [str(z1), str(z2)]}, indent=2))


if __name__ == "__main__":
    main()
