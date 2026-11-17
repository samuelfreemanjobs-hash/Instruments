#!/usr/bin/env python3
"""Package BLACK BOX 96 commercial zip: cores + pitch/cassette/parallel FX + docs.

Applies the Converter Night–style kit instructions at package time:
- Bitcrushed / pitched variants
- Cassette desk FX
- Parallel RAW / TAPE / LOFI / CASSETTE banks
- Matched mini-kits
- Hardware_Processing_Guide in _DOCS
"""

from __future__ import annotations

import argparse
import json
import shutil
import sys
import zipfile
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[2]
for p in (str(ROOT), str(ROOT / "scripts")):
    if p not in sys.path:
        sys.path.insert(0, p)

from scripts.black_box.process_fx import (  # noqa: E402
    bitcrush,
    cassette_fx,
    pitch_shift,
    process_lane,
    read_wav_mono,
    write_wav_mono,
)
from scripts.memphis_oneshots.phonk_hat import generate_memphis_phonk_hat  # noqa: E402
from scripts.memphis_oneshots.phonk_kick import generate_memphis_phonk_kick  # noqa: E402
from scripts.memphis_oneshots.phonk_snare import generate_memphis_phonk_snare  # noqa: E402
from scripts.memphis_oneshots.render_memphis_loop import render_memphis_loop  # noqa: E402
from dsp_math import loop_samples  # noqa: E402

MASTERS = ROOT / "kits/black-box-96/masters"
PACK_NAME = "Black_Box_96"
TITLE = "BLACK BOX 96"
SUBTITLE = "The Original Machine — Memphis Phonk / Southern Drum Kit"

CORE_FOLDERS = [
    "01_808_Subs",
    "02_Kicks",
    "03_Snares",
    "04_Claps",
    "05_Hats_Closed",
    "06_Hats_Open",
    "07_Cowbells",
    "08_Toms",
    "09_Percussion",
]

PARALLEL_ROLES = {
    "01_808_Subs": ("808s", 4),
    "02_Kicks": ("Kicks", 3),
    "03_Snares": ("Snares", 4),
    "04_Claps": ("Claps", 2),
    "05_Hats_Closed": ("Hats_Closed", 3),
    "07_Cowbells": ("Cowbells", 3),
}

MINI_KITS = [
    {
        "id": "01_OriginalMachine",
        "bpm": 94,
        "kick_hz": 43.65,
        "snare_hz": 220.0,
        "hat_hz": 9200.0,
        "blueprint": ROOT / "kits/converter-night/loops/cnv-dr660-94-a.json",
    },
    {
        "id": "02_ConverterCrawl",
        "bpm": 94,
        "kick_hz": 36.71,
        "snare_hz": 196.0,
        "hat_hz": 8600.0,
        "blueprint": ROOT / "kits/converter-night/loops/cnv-arch-horror.json",
    },
    {
        "id": "03_BitcrushBed",
        "bpm": 94,
        "kick_hz": 41.20,
        "snare_hz": 233.0,
        "hat_hz": 9800.0,
        "blueprint": ROOT / "kits/converter-night/loops/cnv-arch-bitcrush.json",
    },
    {
        "id": "04_ClubBounce",
        "bpm": 134,
        "kick_hz": 46.25,
        "snare_hz": 247.0,
        "hat_hz": 10500.0,
        "blueprint": ROOT / "kits/converter-night/loops/cnv-arch-bounce.json",
    },
]


def _sorted_wavs(folder: Path) -> list[Path]:
    return sorted(folder.glob("*.wav"))


def _build_pitched(pack: Path, masters: Path) -> int:
    dest = pack / "02_BITCRUSHED_AND_PITCHED"
    n = 0
    rate = dest / "RateDropped_808s"
    rate.mkdir(parents=True)
    for path in _sorted_wavs(masters / "01_808_Subs")[:12]:
        buf = read_wav_mono(path)
        for st in (-12, -24):
            name = path.stem + f"_pitch{st}st.wav"
            write_wav_mono(rate / name, pitch_shift(buf, st))
            n += 1

    pitched_snr = dest / "Pitched_Snares"
    pitched_snr.mkdir(parents=True)
    for path in _sorted_wavs(masters / "03_Snares")[:10]:
        buf = read_wav_mono(path)
        for st in (-7, 5):
            name = f"{path.stem}_pitch{st}st.wav"
            write_wav_mono(pitched_snr / name, pitch_shift(buf, st))
            n += 1

    pitched_cow = dest / "Pitched_Cowbells"
    pitched_cow.mkdir(parents=True)
    for path in _sorted_wavs(masters / "07_Cowbells")[:8]:
        buf = read_wav_mono(path)
        for st in (-5, 7):
            name = f"{path.stem}_pitch{st}st.wav"
            write_wav_mono(pitched_cow / name, pitch_shift(buf, st))
            n += 1

    aliased = dest / "Aliased_One_Shots"
    aliased.mkdir(parents=True)
    for folder, take in (("03_Snares", 4), ("05_Hats_Closed", 4), ("02_Kicks", 4)):
        for path in _sorted_wavs(masters / folder)[:take]:
            buf = read_wav_mono(path)
            crushed = bitcrush(buf, bit_depth=9, hold_sr=18000.0)
            write_wav_mono(aliased / f"{path.stem}_alias.wav", crushed)
            n += 1
    return n


def _build_cassette(pack: Path, masters: Path) -> int:
    dest = pack / "03_CASSETTE_AND_TAPE"
    n = 0
    picks = [
        ("01_808_Subs", 6),
        ("02_Kicks", 4),
        ("03_Snares", 6),
        ("04_Claps", 4),
        ("05_Hats_Closed", 4),
        ("07_Cowbells", 4),
        ("08_Toms", 3),
    ]
    for folder, take in picks:
        out = dest / folder
        out.mkdir(parents=True)
        for i, path in enumerate(_sorted_wavs(masters / folder)[:take]):
            buf = read_wav_mono(path)
            fx = cassette_fx(buf, seed=960 + i)
            write_wav_mono(out / f"{path.stem}_cassette.wav", fx)
            n += 1
    return n


def _build_parallel(pack: Path, masters: Path) -> int:
    dest = pack / "04_PARALLEL_PROCESS_BANKS"
    n = 0
    for lane in ("RAW", "TAPE", "LOFI", "CASSETTE"):
        for src_folder, (role, take) in PARALLEL_ROLES.items():
            out = dest / lane / role
            out.mkdir(parents=True)
            for i, path in enumerate(_sorted_wavs(masters / src_folder)[:take]):
                buf = read_wav_mono(path)
                processed = process_lane(buf, lane, seed=i)
                write_wav_mono(out / f"BB96_{lane}_{role}_{i+1:02d}.wav", processed)
                n += 1
    return n


def _build_mini_kits(pack: Path) -> int:
    root = pack / "06_MATCHED_MINI_KITS"
    n = 0
    for kit in MINI_KITS:
        kdir = root / kit["id"]
        oneshots = kdir / "OneShots"
        oneshots.mkdir(parents=True)
        kick = generate_memphis_phonk_kick(root_freq=kit["kick_hz"], clip_drive_db=4.5)
        snare = generate_memphis_phonk_snare(membrane_root=kit["snare_hz"], seed=700 + kit["bpm"])
        hat = generate_memphis_phonk_hat(center_hz=kit["hat_hz"], seed=900 + kit["bpm"])
        # clap / cow from process_fx-friendly simple gens via masters not required
        from scripts.black_box.generate_bb96 import _clap, _cowbell

        clap = _clap(seed=kit["bpm"])
        cow = _cowbell(pitch=0.95 + kit["bpm"] / 1000.0)
        for role, buf in (
            ("Kick", kick),
            ("Snare", snare),
            ("HatClosed", hat),
            ("Clap", clap),
            ("Cowbell", cow),
        ):
            write_wav_mono(oneshots / f"BB96_{kit['id']}_{role}.wav", buf)
            n += 1
        # cassette sibling of kick for A/B inside mini-kit
        write_wav_mono(
            oneshots / f"BB96_{kit['id']}_Kick_Cassette.wav",
            cassette_fx(kick, seed=kit["bpm"]),
        )
        n += 1

        bp = json.loads(kit["blueprint"].read_text(encoding="utf-8"))
        loop = render_memphis_loop(bp, seed=kit["bpm"])
        want = loop_samples(int(bp["bars"]), float(bp["bpm"]), 44100)
        if len(loop) > want:
            loop = loop[:want]
        elif len(loop) < want:
            loop = np.pad(loop, (0, want - len(loop)))
        write_wav_mono(kdir / f"BB96_{kit['id']}_LOOP_{kit['bpm']}bpm.wav", loop)
        n += 1
        (kdir / "README.txt").write_text(
            "\n".join(
                [
                    f"Mini-kit: {kit['id']}",
                    f"BPM: {kit['bpm']}",
                    "Load OneShots/ to pads. Drop the LOOP on a track.",
                    "Compare Kick vs Kick_Cassette for desk grit.",
                    "See _DOCS/Hardware_Processing_Guide.md",
                    "",
                ]
            ),
            encoding="utf-8",
        )
    return n


def package(out_dir: Path, masters: Path) -> dict:
    if not masters.is_dir():
        raise FileNotFoundError(f"run generate_bb96.py first: missing {masters}")
    if out_dir.exists():
        shutil.rmtree(out_dir)
    pack = out_dir / PACK_NAME
    core = pack / "01_CORE_ONESHOTS"
    core.mkdir(parents=True)

    core_count = 0
    for folder in CORE_FOLDERS:
        src = masters / folder
        if not src.is_dir():
            raise FileNotFoundError(src)
        shutil.copytree(src, core / folder)
        core_count += len(_sorted_wavs(src))

    loops_src = masters / "10_Loops"
    loops_dst = pack / "05_LOOPS"
    shutil.copytree(loops_src, loops_dst)
    loop_count = len(_sorted_wavs(loops_dst))

    pitched_n = _build_pitched(pack, masters)
    cassette_n = _build_cassette(pack, masters)
    parallel_n = _build_parallel(pack, masters)
    mini_n = _build_mini_kits(pack)

    docs = pack / "_DOCS"
    docs.mkdir(exist_ok=True)
    if (masters / "manifest.json").exists():
        shutil.copy2(masters / "manifest.json", docs / "core_manifest.json")

    (docs / "PRODUCT.txt").write_text(
        "\n".join(
            [
                TITLE,
                SUBTITLE,
                "Collection: DiskLordz Black Box — The Lost Drum Machine Collection",
                "SKU: BB96",
                "Category: Drum sample kit (oneshots + loops + process banks)",
                "Promise: Signature vintage drum-machine sound —",
                "  thick 808s · aggressive snares · metallic cowbells ·",
                "  crunchy hats · analog toms · dry claps",
                "  + pitched / cassette / RAW-TAPE-LOFI parallel banks",
                "Format: 44.1 kHz · PCM16 · mono",
                "Original synthesis only — not a hardware ROM dump.",
                "READ: Hardware_Processing_Guide.md (this folder)",
                "release_ready: FALSE until HITL",
                "",
            ]
        ),
        encoding="utf-8",
    )
    shutil.copy(
        ROOT / "kits/black-box-96/docs/Hardware_Processing_Guide.md",
        docs / "Hardware_Processing_Guide.md",
    )
    shutil.copy(ROOT / "kits/black-box-96/PRODUCT.md", docs / "PRODUCT.md")
    shutil.copy(ROOT / "kits/black-box-collection/PRODUCT_LINEUP.md", docs / "COLLECTION.md")
    shutil.copy(ROOT / "kits/black-box-collection/SONIC_DNA.md", docs / "SONIC_DNA.md")

    total = sum(1 for _ in pack.rglob("*.wav"))
    manifest = {
        "title": TITLE,
        "identity": "The Original Machine",
        "sku": "BB96",
        "pack_dir": PACK_NAME,
        "release_ready": False,
        "wav_count": total,
        "counts": {
            "core_oneshots": core_count,
            "loops": loop_count,
            "bitcrushed_pitched": pitched_n,
            "cassette_tape": cassette_n,
            "parallel_banks": parallel_n,
            "matched_mini_kits": mini_n,
        },
        "features": [
            "core_oneshots",
            "bitcrushed_and_pitched",
            "cassette_fx",
            "parallel_RAW_TAPE_LOFI_CASSETTE",
            "matched_mini_kits",
            "hardware_processing_guide",
        ],
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


def _refresh_listen(pack_root: Path) -> None:
    listen = Path("/opt/cursor/artifacts/Black_Box_96_listen")
    if listen.exists():
        shutil.rmtree(listen, ignore_errors=True)
    listen.mkdir(parents=True)
    picks: list[Path] = []
    core = pack_root / "01_CORE_ONESHOTS"
    picks += list((core / "01_808_Subs").glob("*.wav"))[:1]
    picks += list((core / "03_Snares").glob("*.wav"))[:1]
    picks += list((pack_root / "03_CASSETTE_AND_TAPE" / "03_Snares").glob("*.wav"))[:1]
    picks += list((pack_root / "02_BITCRUSHED_AND_PITCHED" / "RateDropped_808s").glob("*_pitch-24st.wav"))[:1]
    picks += list((pack_root / "04_PARALLEL_PROCESS_BANKS" / "CASSETTE" / "808s").glob("*.wav"))[:1]
    picks += list((pack_root / "05_LOOPS").glob("*94bpm*"))[:1]
    picks += [pack_root / "_DOCS" / "PRODUCT.txt", pack_root / "_DOCS" / "Hardware_Processing_Guide.md"]
    for p in picks:
        if p and p.is_file():
            shutil.copy2(p, listen / p.name)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--masters", type=Path, default=MASTERS)
    parser.add_argument("--staging", type=Path, default=ROOT / "kits/black-box-96/pack_staging")
    parser.add_argument("--zip", type=Path, default=Path("/opt/cursor/artifacts/Black_Box_96.zip"))
    args = parser.parse_args()
    manifest = package(args.staging, args.masters)
    pack_root = args.staging / PACK_NAME
    zip_pack(pack_root, args.zip)
    _refresh_listen(pack_root)
    print(json.dumps(manifest, indent=2))
    print(f"zip → {args.zip} ({args.zip.stat().st_size} bytes)")


if __name__ == "__main__":
    main()
