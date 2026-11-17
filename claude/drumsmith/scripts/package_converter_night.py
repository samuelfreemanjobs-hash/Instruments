#!/usr/bin/env python3
"""Package the full Converter Night commercial kit zip.

Assembles the 512-hit storefront + parallel process banks + matched mini-kits
under the locked store title. WAVs stay gitignored; zip lands in artifacts.
"""

from __future__ import annotations

import argparse
import json
import shutil
import sys
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
STOREFRONT = ROOT / "kits/converter-night/storefront/CNV_Converter_Night"
KIT_TITLE = "Converter Night"
SKU = "CNV"
PACK_DIR_NAME = "CNV_Converter_Night"


def _count_wavs(path: Path) -> int:
    return sum(1 for _ in path.rglob("*.wav"))


def package(
    out_dir: Path,
    *,
    include_extras: bool = True,
) -> dict:
    if out_dir.exists():
        shutil.rmtree(out_dir)
    out_dir.mkdir(parents=True)

    if not STOREFRONT.is_dir():
        raise FileNotFoundError(f"missing storefront: {STOREFRONT}")

    dest = out_dir / PACK_DIR_NAME
    shutil.copytree(STOREFRONT, dest)

    extras = {"parallel": 0, "mini": 0}
    if include_extras:
        # Build / reuse elite extras into 05_ and 06_
        sys.path.insert(0, str(ROOT))
        from scripts.memphis_oneshots.build_elite_phonk_kit import build_kit

        elite_tmp = out_dir / "_elite_tmp"
        build_kit(elite_tmp)
        parallel_src = elite_tmp / "01_PARALLEL_PROCESS_BANKS"
        mini_src = elite_tmp / "02_MATCHED_MINI_KITS"
        shutil.copytree(parallel_src, dest / "05_PARALLEL_PROCESS_BANKS")
        shutil.copytree(mini_src, dest / "06_MATCHED_MINI_KITS")
        extras["parallel"] = _count_wavs(dest / "05_PARALLEL_PROCESS_BANKS")
        extras["mini"] = _count_wavs(dest / "06_MATCHED_MINI_KITS")
        shutil.rmtree(elite_tmp)

    docs = dest / "_DOCS"
    docs.mkdir(exist_ok=True)
    (docs / "PRODUCT.txt").write_text(
        "\n".join(
            [
                f"{KIT_TITLE}",
                f"SKU: {SKU}",
                "Category: Drum sample kit (oneshots + loops)",
                "Promise: Original 90s Memphis rap / phonk drums —",
                "  rate-dropped subs · converter snap · tape-cutting hats",
                "Format: 44.1 kHz · PCM16 · mono",
                "Original synthesis only — not a hardware ROM dump.",
                "No artist names on storefront filenames.",
                "release_ready: FALSE until HITL (ear / price / cover / legal)",
                "",
                "Folders:",
                "  01 RAW hardware-style oneshots",
                "  02 Bitcrushed & pitched (subs / alias / reverse / stabs)",
                "  03 Loop builders (BPM 94 / 134 in filenames)",
                "  04 Lo-fi textures & noise",
                "  05 Parallel RAW / TAPE / LOFI process banks",
                "  06 Matched mini-kits (oneshots + BPM loops)",
                "",
            ]
        ),
        encoding="utf-8",
    )
    # Ensure product brief is present
    product_md = ROOT / "kits/converter-night/PRODUCT.md"
    if product_md.is_file():
        shutil.copy(product_md, docs / "PRODUCT.md")

    core = _count_wavs(dest) - extras["parallel"] - extras["mini"]
    # Count only core folders 01-04
    core = 0
    for folder in (
        "01_RAW_HARDWARE_ONESHOTS",
        "02_BITCRUSHED_AND_PITCHED",
        "03_LOOP_BUILDERS_AND_GROOVES",
        "04_LOFI_TEXTURES_AND_NOISE",
    ):
        core += _count_wavs(dest / folder)

    manifest = {
        "title": KIT_TITLE,
        "sku": SKU,
        "pack_dir": PACK_DIR_NAME,
        "release_ready": False,
        "delivery": {"sample_rate": 44100, "bit_depth": 16, "channels": 1},
        "counts": {
            "core_512": core,
            "parallel_process": extras["parallel"],
            "matched_mini_kits": extras["mini"],
            "total_wav": _count_wavs(dest),
        },
    }
    (dest / "manifest_pack.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    (out_dir / "pack_manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    return manifest


def zip_pack(src_pack: Path, zip_path: Path) -> None:
    zip_path.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(zip_path, "w", compression=zipfile.ZIP_DEFLATED) as zf:
        for path in sorted(src_pack.rglob("*")):
            if path.is_file():
                zf.write(path, arcname=str(Path(PACK_DIR_NAME) / path.relative_to(src_pack)))


def main() -> None:
    parser = argparse.ArgumentParser(description=f"Package full {KIT_TITLE} kit")
    parser.add_argument(
        "--out",
        type=Path,
        default=ROOT / "kits/converter-night/banks/full_pack_staging",
    )
    parser.add_argument(
        "--zip",
        type=Path,
        default=Path("/opt/cursor/artifacts/CNV_Converter_Night.zip"),
    )
    parser.add_argument("--no-extras", action="store_true")
    args = parser.parse_args()

    manifest = package(args.out, include_extras=not args.no_extras)
    pack_root = args.out / PACK_DIR_NAME
    zip_pack(pack_root, args.zip)
    # Stage a few listen picks (full tree can hit artifact FS quirks)
    listen = Path("/opt/cursor/artifacts") / "CNV_Converter_Night_listen"
    if listen.exists():
        shutil.rmtree(listen, ignore_errors=True)
    listen.mkdir(parents=True, exist_ok=True)
    picks = [
        pack_root / "01_RAW_HARDWARE_ONESHOTS/Kicks/CNV_01_KICK_BitPunch_NA_01.wav",
        pack_root / "01_RAW_HARDWARE_ONESHOTS/Snares",
        pack_root / "03_LOOP_BUILDERS_AND_GROOVES/Acoustic_Rhythm_Loops/CNV_03_LOOP_Rhythm_94_01.wav",
        pack_root / "03_LOOP_BUILDERS_AND_GROOVES/Acoustic_Rhythm_Loops/CNV_03_LOOP_Rhythm_134_10.wav",
        pack_root / "05_PARALLEL_PROCESS_BANKS/RAW/Kicks/CNV_RAW_KICK_01.wav",
        pack_root / "05_PARALLEL_PROCESS_BANKS/TAPE/Kicks/CNV_TAPE_KICK_01.wav",
        pack_root / "05_PARALLEL_PROCESS_BANKS/LOFI/Snares/CNV_LOFI_SNR_01.wav",
        pack_root / "06_MATCHED_MINI_KITS/01_ConverterCrawl/CNV_01_ConverterCrawl_LOOP_94bpm.wav",
        pack_root / "06_MATCHED_MINI_KITS/04_ClubBounce/CNV_04_ClubBounce_LOOP_134bpm.wav",
        pack_root / "_DOCS/PRODUCT.txt",
        pack_root / "manifest_pack.json",
    ]
    for src in picks:
        if src.is_file():
            shutil.copy2(src, listen / src.name)
        elif src.is_dir():
            # one snare sample from folder
            wavs = sorted(src.glob("*.wav"))
            if wavs:
                shutil.copy2(wavs[0], listen / wavs[0].name)

    print(json.dumps(manifest, indent=2))
    print(f"zip → {args.zip} ({args.zip.stat().st_size} bytes)")
    print(f"listen picks → {listen}")


if __name__ == "__main__":
    main()
