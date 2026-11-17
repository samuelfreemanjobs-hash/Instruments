#!/usr/bin/env python3
"""Package any Black Box era kit: cores + pitch/cassette/parallel FX + docs."""

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

from dsp_math import loop_samples  # noqa: E402
from scripts.black_box.generate_era import (  # noqa: E402
    kit_masters_dir,
    load_profile,
)
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


def _sorted_wavs(folder: Path) -> list[Path]:
    return sorted(folder.glob("*.wav")) if folder.is_dir() else []


def _core_folders(masters: Path) -> list[Path]:
    return sorted(p for p in masters.iterdir() if p.is_dir() and p.name != "_DOCS")


def _loop_folder(masters: Path) -> Path | None:
    for p in _core_folders(masters):
        if "loop" in p.name.lower():
            return p
    return None


def _oneshot_folders(masters: Path) -> list[Path]:
    loops = _loop_folder(masters)
    return [p for p in _core_folders(masters) if p != loops]


def _guide_text(profile: dict) -> str:
    title = profile["title"]
    identity = profile["identity"]
    pack = profile["pack_dir"]
    return f"""# {title} — hardware / DAW processing guide

**{identity}** · DiskLordz Black Box — The Lost Drum Machine Collection

Producer instructions for this zip. Original synthesis only — not a hardware ROM dump.

## What's in this pack

| Folder | What it is |
|--------|------------|
| `01_CORE_ONESHOTS/` | RAW era cores |
| `02_BITCRUSHED_AND_PITCHED/` | Rate-dropped / pitched / aliased variants |
| `03_CASSETTE_AND_TAPE/` | Cassette desk FX (drive → tape LPF → wow/flutter → hiss) |
| `04_PARALLEL_PROCESS_BANKS/` | RAW / TAPE / LOFI / CASSETTE siblings |
| `05_LOOPS/` | Construction loops |
| `06_MATCHED_MINI_KITS/` | Oneshots + BPM loop + cassette kick A/B |
| `_DOCS/` | This guide + product notes |

## Rate-drop / pitched

1. Start from cores or use `02_BITCRUSHED_AND_PITCHED/RateDropped_808s/`.
2. Pitch down hard (−12 to −24 st) for crawling subs.
3. Shorten with choke so notes stay punchy under cassette compression.

## Cassette / tape grit

- Soft clip → HF roll-off → optional hiss from `03_CASSETTE_AND_TAPE/`.
- A/B RAW vs CASSETTE in `04_PARALLEL_PROCESS_BANKS/` before locking a pad.

## Parallel banks

Same hit, four ways: **RAW** · **TAPE** · **LOFI** · **CASSETTE**.

## Device notes

PCM16 mono @ 44.1 kHz. Device paths UNVERIFIED until logged.  
`release_ready` false until HITL. Pack root: `{pack}/`
"""


def _build_pitched(pack: Path, masters: Path) -> int:
    dest = pack / "02_BITCRUSHED_AND_PITCHED"
    n = 0
    # Prefer 808 / kick folders for rate-drop
    sources = []
    for folder in _oneshot_folders(masters):
        low = folder.name.lower()
        if "808" in low or "sub" in low or "kick" in low or "bass" in low:
            sources.append(folder)
    if not sources:
        sources = _oneshot_folders(masters)[:1]
    rate = dest / "RateDropped_808s"
    rate.mkdir(parents=True)
    for folder in sources[:2]:
        for path in _sorted_wavs(folder)[:8]:
            buf = read_wav_mono(path)
            for st in (-12, -24):
                write_wav_mono(rate / f"{path.stem}_pitch{st}st.wav", pitch_shift(buf, st))
                n += 1

    pitched = dest / "Pitched_Tops"
    pitched.mkdir(parents=True)
    for folder in _oneshot_folders(masters):
        low = folder.name.lower()
        if any(x in low for x in ("snare", "cow", "clap", "hat")):
            for path in _sorted_wavs(folder)[:4]:
                buf = read_wav_mono(path)
                for st in (-7, 5):
                    write_wav_mono(pitched / f"{path.stem}_pitch{st}st.wav", pitch_shift(buf, st))
                    n += 1
            break

    aliased = dest / "Aliased_One_Shots"
    aliased.mkdir(parents=True)
    for folder in _oneshot_folders(masters)[:3]:
        for path in _sorted_wavs(folder)[:3]:
            buf = read_wav_mono(path)
            write_wav_mono(aliased / f"{path.stem}_alias.wav", bitcrush(buf, bit_depth=9, hold_sr=18000.0))
            n += 1
    return n


def _build_cassette(pack: Path, masters: Path) -> int:
    dest = pack / "03_CASSETTE_AND_TAPE"
    n = 0
    for folder in _oneshot_folders(masters):
        out = dest / folder.name
        out.mkdir(parents=True)
        for i, path in enumerate(_sorted_wavs(folder)[:4]):
            write_wav_mono(out / f"{path.stem}_cassette.wav", cassette_fx(read_wav_mono(path), seed=100 + i))
            n += 1
    return n


def _build_parallel(pack: Path, masters: Path) -> int:
    dest = pack / "04_PARALLEL_PROCESS_BANKS"
    n = 0
    roles = _oneshot_folders(masters)[:6]
    for lane in ("RAW", "TAPE", "LOFI", "CASSETTE"):
        for folder in roles:
            role = folder.name.split("_", 1)[-1] if "_" in folder.name else folder.name
            out = dest / lane / role
            out.mkdir(parents=True)
            for i, path in enumerate(_sorted_wavs(folder)[:3]):
                processed = process_lane(read_wav_mono(path), lane, seed=i)
                write_wav_mono(out / f"{lane}_{role}_{i+1:02d}.wav", processed)
                n += 1
    return n


def _build_mini_kits(pack: Path, profile: dict) -> int:
    from scripts.black_box.generate_era import _clap, _cowbell

    root = pack / "06_MATCHED_MINI_KITS"
    eng = profile["engine"]
    n = 0
    kits = [
        ("01_KitA", 94, 43.65, 220.0, 9000.0, "cnv-dr660-94-a.json"),
        ("02_KitB", 94, 36.71, 196.0, 8200.0, "cnv-arch-horror.json"),
        ("03_KitC", 134, 46.25, 233.0, 10000.0, "cnv-arch-bounce.json"),
    ]
    for kid, bpm, khz, shz, hhz, bp_name in kits:
        kdir = root / kid
        oneshots = kdir / "OneShots"
        oneshots.mkdir(parents=True)
        kick = generate_memphis_phonk_kick(
            root_freq=khz,
            clip_drive_db=float(eng.get("kick_clip_db", 4.0)),
            bit_depth=int(eng.get("bit_depth", 12)),
            lofi_sr=float(eng.get("lofi_sr", 26040)),
            tape_lpf_hz=float(eng.get("tape_lpf_hz", 12000)),
        )
        snare = generate_memphis_phonk_snare(membrane_root=shz, seed=700 + bpm)
        hat = generate_memphis_phonk_hat(center_hz=hhz, seed=800 + bpm)
        for role, buf in (
            ("Kick", kick),
            ("Snare", snare),
            ("HatClosed", hat),
            ("Clap", _clap(bpm)),
            ("Cowbell", _cowbell(1.0, eng)),
        ):
            write_wav_mono(oneshots / f"{profile['prefix']}_{kid}_{role}.wav", buf)
            n += 1
        write_wav_mono(oneshots / f"{profile['prefix']}_{kid}_Kick_Cassette.wav", cassette_fx(kick, seed=bpm))
        n += 1
        bp_path = ROOT / "kits/converter-night/loops" / bp_name
        bp = json.loads(bp_path.read_text(encoding="utf-8"))
        loop = render_memphis_loop(bp, seed=bpm)
        want = loop_samples(int(bp["bars"]), float(bp["bpm"]), 44100)
        if len(loop) > want:
            loop = loop[:want]
        elif len(loop) < want:
            loop = np.pad(loop, (0, want - len(loop)))
        write_wav_mono(kdir / f"{profile['prefix']}_{kid}_LOOP_{bpm}bpm.wav", loop)
        n += 1
        (kdir / "README.txt").write_text(
            f"Mini-kit {kid} @ {bpm} BPM\nLoad OneShots. Compare Kick vs Kick_Cassette.\nSee _DOCS/Hardware_Processing_Guide.md\n",
            encoding="utf-8",
        )
    return n


def package(out_dir: Path, masters: Path, profile: dict) -> dict:
    if not masters.is_dir():
        raise FileNotFoundError(f"missing masters: {masters}")
    if out_dir.exists():
        shutil.rmtree(out_dir)
    pack_name = profile["pack_dir"]
    pack = out_dir / pack_name
    core = pack / "01_CORE_ONESHOTS"
    core.mkdir(parents=True)

    core_count = 0
    loops = _loop_folder(masters)
    for folder in _oneshot_folders(masters):
        shutil.copytree(folder, core / folder.name)
        core_count += len(_sorted_wavs(folder))

    loop_count = 0
    if loops:
        shutil.copytree(loops, pack / "05_LOOPS")
        loop_count = len(_sorted_wavs(pack / "05_LOOPS"))

    pitched_n = _build_pitched(pack, masters)
    cassette_n = _build_cassette(pack, masters)
    parallel_n = _build_parallel(pack, masters)
    mini_n = _build_mini_kits(pack, profile)

    docs = pack / "_DOCS"
    docs.mkdir(exist_ok=True)
    if (masters / "manifest.json").exists():
        shutil.copy2(masters / "manifest.json", docs / "core_manifest.json")
    (docs / "Hardware_Processing_Guide.md").write_text(_guide_text(profile), encoding="utf-8")
    (docs / "PRODUCT.txt").write_text(
        "\n".join(
            [
                profile["title"],
                f"{profile['identity']} — DiskLordz Black Box Collection",
                f"SKU: {profile['sku']}",
                "Format: 44.1 kHz · PCM16 · mono",
                "Includes: cores + pitched + cassette + parallel banks + mini-kits",
                "READ: Hardware_Processing_Guide.md",
                "Original synthesis only — not a hardware ROM dump.",
                "release_ready: FALSE until HITL",
                "",
            ]
        ),
        encoding="utf-8",
    )
    shutil.copy(ROOT / "kits/black-box-collection/PRODUCT_LINEUP.md", docs / "COLLECTION.md")
    shutil.copy(ROOT / "kits/black-box-collection/SONIC_DNA.md", docs / "SONIC_DNA.md")

    kit_docs = kit_masters_dir(profile).parent / "PRODUCT.md"
    if kit_docs.exists():
        shutil.copy(kit_docs, docs / "PRODUCT.md")

    total = sum(1 for _ in pack.rglob("*.wav"))
    manifest = {
        "title": profile["title"],
        "identity": profile["identity"],
        "sku": profile["sku"],
        "pack_dir": pack_name,
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


def zip_pack(pack_root: Path, zip_path: Path, pack_name: str) -> None:
    zip_path.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(zip_path, "w", compression=zipfile.ZIP_DEFLATED) as zf:
        for path in sorted(pack_root.rglob("*")):
            if path.is_file():
                zf.write(path, arcname=str(Path(pack_name) / path.relative_to(pack_root)))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("era", help="bb93|bb96|bb98|bb2000")
    parser.add_argument("--masters", type=Path, default=None)
    parser.add_argument("--staging", type=Path, default=None)
    parser.add_argument("--zip", type=Path, default=None)
    args = parser.parse_args()
    profile = load_profile(args.era)
    masters = args.masters or kit_masters_dir(profile)
    staging = args.staging or (masters.parent / "pack_staging")
    zip_path = args.zip or Path(f"/opt/cursor/artifacts/{profile['pack_dir']}.zip")
    manifest = package(staging, masters, profile)
    pack_root = staging / profile["pack_dir"]
    zip_pack(pack_root, zip_path, profile["pack_dir"])
    print(json.dumps(manifest, indent=2))
    print(f"zip → {zip_path} ({zip_path.stat().st_size} bytes)")


if __name__ == "__main__":
    main()
