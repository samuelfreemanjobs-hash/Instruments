#!/usr/bin/env python3
"""DIRTY CIRCUITS + BB96 Dirty Circuits Edition from BB96 masters.

Driven by profiles/dirty_circuits.json — modes and pack identity are data.
"""

from __future__ import annotations

import argparse
import json
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
for p in (str(ROOT), str(ROOT / "scripts")):
    if p not in sys.path:
        sys.path.insert(0, p)

from scripts.black_box.generate_era import load_profile  # noqa: E402
from scripts.black_box.process_fx import (  # noqa: E402
    bitcrush,
    cassette_fx,
    process_lane,
    read_wav_mono,
    write_wav_mono,
    zip_tree,
)

BB96_MASTERS = ROOT / "kits/black-box-96/masters"


def _apply_mode(buf, mode: str, recipes: dict, seed: int = 0):
    recipe = recipes[mode]
    if recipe.get("lane") == "RAW":
        return process_lane(buf, "RAW", seed=seed)
    x = bitcrush(
        buf,
        bit_depth=int(recipe.get("bit_depth", 8)),
        hold_sr=float(recipe.get("hold_sr", 16000.0)),
    )
    return cassette_fx(
        x,
        drive_db=float(recipe.get("drive_db", 7.0)),
        tape_lpf_hz=float(recipe.get("tape_lpf_hz", 9000.0)),
        wow_depth_ms=float(recipe.get("wow_depth_ms", 0.35)),
        hiss_db=float(recipe.get("hiss_db", -36.0)),
        seed=seed + int(recipe.get("seed_offset", 0)),
    )


def _all_oneshots(masters: Path) -> list[Path]:
    out: list[Path] = []
    for folder in sorted(p for p in masters.iterdir() if p.is_dir() and "loop" not in p.name.lower()):
        out.extend(sorted(folder.glob("*.wav")))
    return out


def _pick_cores(masters: Path, per_folder: int) -> list[Path]:
    picks: list[Path] = []
    for folder in sorted(p for p in masters.iterdir() if p.is_dir() and "loop" not in p.name.lower()):
        picks.extend(sorted(folder.glob("*.wav"))[:per_folder])
    return picks


def build_dirty_circuits(out_dir: Path, masters: Path, profile: dict | None = None) -> dict:
    """Standalone Dirty Circuits bus pack."""
    profile = profile or load_profile("dirty_circuits")
    recipes = profile["mode_fx"]
    modes = list(profile.get("modes") or recipes.keys())
    per_folder = int(profile.get("bbdc_per_folder", 10))
    excl_roles = [r.lower() for r in profile.get("exclusive_roles", [])]
    pack_name = profile["pack_dir"]
    prefix = profile["prefix"]

    if out_dir.exists():
        shutil.rmtree(out_dir)
    pack = out_dir / pack_name
    docs = pack / "_DOCS"
    docs.mkdir(parents=True)
    files = 0
    cores = _pick_cores(masters, per_folder=per_folder)
    for mode in modes:
        mode_dir = pack / "01_PROCESS_MODES" / mode
        mode_dir.mkdir(parents=True)
        for i, path in enumerate(cores):
            role = path.parent.name
            out = mode_dir / role
            out.mkdir(parents=True, exist_ok=True)
            write_wav_mono(
                out / f"{prefix}_{mode}_{path.stem}.wav",
                _apply_mode(read_wav_mono(path), mode, recipes, seed=i),
            )
            files += 1

    excl = pack / "02_EXCLUSIVES_RUINED"
    excl.mkdir(parents=True)
    for path in cores:
        low = path.parent.name.lower()
        if any(x in low for x in excl_roles):
            write_wav_mono(
                excl / f"{prefix}_EX_{path.stem}_ruined.wav",
                _apply_mode(read_wav_mono(path), "RUINED", recipes, seed=99),
            )
            files += 1

    (docs / "Hardware_Processing_Guide.md").write_text(
        f"""# {profile['title']} — processing guide

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
        f"{profile['title']} — {profile['identity']}\nSKU: {profile['sku']}\nrelease_ready: FALSE\n",
        encoding="utf-8",
    )
    shutil.copy(ROOT / "kits/black-box-collection/PRODUCT_LINEUP.md", docs / "COLLECTION.md")
    manifest = {
        "sku": profile["sku"],
        "title": profile["title"],
        "wav_count": files,
        "release_ready": False,
        "modes": modes,
    }
    (docs / "pack_manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    return manifest


def build_bb96dc(out_dir: Path, masters: Path, profile: dict | None = None) -> dict:
    """BLACK BOX 96: Dirty Circuits Edition — full clean-to-dirty expansion."""
    profile = profile or load_profile("dirty_circuits")
    recipes = profile["mode_fx"]
    edition = profile["edition_bb96dc"]
    pack_name = edition["pack_dir"]
    prefix = edition["prefix"]

    if out_dir.exists():
        shutil.rmtree(out_dir)
    pack = out_dir / pack_name
    docs = pack / "_DOCS"
    docs.mkdir(parents=True)
    files = 0
    oneshots = _all_oneshots(masters)
    if not oneshots:
        raise FileNotFoundError(f"no oneshots in {masters}")

    pairs = pack / "01_CLEAN_TO_DIRTY_PAIRS"
    for i, path in enumerate(oneshots):
        role = path.parent.name
        stem = path.stem
        buf = read_wav_mono(path)
        for mode in ("CLEAN", "DIRTY"):
            out = pairs / role / mode
            out.mkdir(parents=True, exist_ok=True)
            write_wav_mono(out / f"{prefix}_{mode}_{stem}.wav", _apply_mode(buf, mode, recipes, seed=i))
            files += 1

    excl = pack / "02_EXCLUSIVES_RUINED"
    excl.mkdir(parents=True)
    for i, path in enumerate(oneshots):
        write_wav_mono(
            excl / f"{prefix}_EX_{path.stem}_ruined.wav",
            _apply_mode(read_wav_mono(path), "RUINED", recipes, seed=i + 3),
        )
        files += 1

    cass = pack / "03_CASSETTE_DESK"
    for i, path in enumerate(oneshots):
        role = path.parent.name
        out = cass / role
        out.mkdir(parents=True, exist_ok=True)
        write_wav_mono(
            out / f"{prefix}_CASS_{path.stem}.wav",
            cassette_fx(read_wav_mono(path), drive_db=5.5, seed=200 + i),
        )
        files += 1

    loops_src = next((p for p in masters.iterdir() if p.is_dir() and "loop" in p.name.lower()), None)
    loop_n = 0
    if loops_src:
        shutil.copytree(loops_src, pack / "04_LOOPS")
        loop_n = len(list((pack / "04_LOOPS").glob("*.wav")))
        files += loop_n

    (docs / "Hardware_Processing_Guide.md").write_text(
        f"""# {edition['title']}

Expanded flagship — clean-to-dirty variations + exclusives of the full BB96 oneshot set.

| Folder | What to do |
|--------|------------|
| `01_CLEAN_TO_DIRTY_PAIRS/<role>/CLEAN` | Map as your base pad |
| `01_CLEAN_TO_DIRTY_PAIRS/<role>/DIRTY` | Same hit, crushed/cassette dirt — layer or swap |
| `02_EXCLUSIVES_RUINED` | Extreme ruined one-offs |
| `03_CASSETTE_DESK` | Desk-only cassette pass (no extra bitcrush) |
| `04_LOOPS` | Construction loops from BB96 |

Stem names match across CLEAN / DIRTY / RUINED / CASS for easy pairing.
""",
        encoding="utf-8",
    )
    (docs / "PRODUCT.txt").write_text(
        "\n".join(
            [
                edition["title"],
                f"SKU: {edition['sku']}",
                f"Identity: {edition['identity']}",
                "Collection: DiskLordz Black Box",
                f"Oneshot pairs: {len(oneshots)} CLEAN + {len(oneshots)} DIRTY",
                f"Ruined exclusives: {len(oneshots)}",
                f"Cassette desk: {len(oneshots)}",
                f"Loops: {loop_n}",
                "release_ready: FALSE until HITL",
                "",
            ]
        ),
        encoding="utf-8",
    )
    shutil.copy(ROOT / "kits/black-box-collection/PRODUCT_LINEUP.md", docs / "COLLECTION.md")
    kit_product = ROOT / "kits/black-box-96-dirty-circuits/PRODUCT.md"
    if kit_product.exists():
        shutil.copy(kit_product, docs / "PRODUCT.md")

    manifest = {
        "sku": edition["sku"],
        "title": edition["title"],
        "identity": edition["identity"],
        "wav_count": files,
        "oneshot_sources": len(oneshots),
        "loops": loop_n,
        "banks": {
            "clean_dirty_pairs": len(oneshots) * 2,
            "ruined_exclusives": len(oneshots),
            "cassette_desk": len(oneshots),
            "loops": loop_n,
        },
        "release_ready": False,
    }
    (docs / "pack_manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    return manifest


def zip_dir(src: Path, zip_path: Path, arc_root: str) -> None:
    zip_tree(src, zip_path, arc_root)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--masters", type=Path, default=BB96_MASTERS)
    parser.add_argument(
        "--staging",
        type=Path,
        default=ROOT / "kits/black-box-dirty-circuits/pack_staging",
    )
    args = parser.parse_args()
    if not args.masters.is_dir():
        raise SystemExit("BB96 masters required — run generate_era.py bb96 first")

    profile = load_profile("dirty_circuits")
    dc_stage = args.staging / "dc"
    bb96dc_stage = args.staging / "bb96dc"
    m1 = build_dirty_circuits(dc_stage, args.masters, profile)
    m2 = build_bb96dc(bb96dc_stage, args.masters, profile)
    kit_stage = ROOT / "kits/black-box-96-dirty-circuits/pack_staging"
    if kit_stage.exists():
        shutil.rmtree(kit_stage)
    shutil.copytree(bb96dc_stage, kit_stage)

    z1 = Path("/opt/cursor/artifacts/Dirty_Circuits.zip")
    z2 = Path("/opt/cursor/artifacts/Black_Box_96_Dirty_Circuits.zip")
    zip_dir(dc_stage / profile["pack_dir"], z1, profile["pack_dir"])
    zip_dir(
        bb96dc_stage / profile["edition_bb96dc"]["pack_dir"],
        z2,
        profile["edition_bb96dc"]["pack_dir"],
    )
    print(json.dumps({"BBDC": m1, "BB96DC": m2, "zips": [str(z1), str(z2)]}, indent=2))


if __name__ == "__main__":
    main()
