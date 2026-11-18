#!/usr/bin/env python3
"""Package any Black Box era kit: cores + pitch/cassette/parallel FX + docs."""

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

from scripts.black_box.generate_era import (  # noqa: E402
    kit_masters_dir,
    load_profile,
)
from scripts.black_box import layered as L  # noqa: E402
from scripts.black_box.curate_hero import build_hero_manifest, select_heroes  # noqa: E402
from scripts.black_box.process_fx import (  # noqa: E402
    DESK_PRESETS,
    bitcrush,
    cassette_fx,
    pitch_shift,
    process_desk,
    process_lane,
    read_wav_mono,
    write_wav_24,
    write_wav_mono,
    write_wav_stereo_16,
    zip_tree,
)


def _sorted_wavs(folder: Path) -> list[Path]:
    return sorted(folder.glob("*.wav")) if folder.is_dir() else []


def _recipe_tag(path: Path) -> str:
    """BB96_808_Thick_F0_01.wav → Thick; falls back to stem."""
    parts = path.stem.split("_")
    return parts[2] if len(parts) >= 3 else path.stem


def _sample_wavs(folder: Path, limit: int) -> list[Path]:
    """Prefer one file per recipe tag so diversify kits are not alphabet-biased to Crawl."""
    wavs = _sorted_wavs(folder)
    if limit <= 0 or len(wavs) <= limit:
        return wavs[:limit]
    by_tag: dict[str, list[Path]] = {}
    for path in wavs:
        by_tag.setdefault(_recipe_tag(path), []).append(path)
    picked: list[Path] = []
    # Round-robin first of each tag (already sorted → lowest index / F0_01 first)
    tags = sorted(by_tag.keys())
    while len(picked) < limit and any(by_tag[t] for t in tags):
        for tag in tags:
            if len(picked) >= limit:
                break
            if by_tag[tag]:
                picked.append(by_tag[tag].pop(0))
    return picked


def _core_folders(masters: Path) -> list[Path]:
    return sorted(p for p in masters.iterdir() if p.is_dir() and p.name != "_DOCS")


def _folder_for_kind(masters: Path, profile: dict, kind: str) -> Path | None:
    name = (profile.get("folder_map") or {}).get(kind)
    if not name:
        return None
    path = masters / name
    return path if path.is_dir() else None


def _loop_folder(masters: Path, profile: dict | None = None) -> Path | None:
    if profile:
        mapped = _folder_for_kind(masters, profile, "LOOP")
        if mapped:
            return mapped
    for p in _core_folders(masters):
        if "loop" in p.name.lower():
            return p
    return None


def _oneshot_folders(masters: Path, profile: dict | None = None) -> list[Path]:
    loops = _loop_folder(masters, profile)
    return [p for p in _core_folders(masters) if p != loops]


def _folders_for_kinds(masters: Path, profile: dict, kinds: tuple[str, ...]) -> list[Path]:
    out: list[Path] = []
    for kind in kinds:
        folder = _folder_for_kind(masters, profile, kind)
        if folder:
            out.append(folder)
    return out


def _identity_slug(profile: dict) -> str:
    identity = profile.get("identity", "Kit")
    cleaned = identity.replace("The ", "").replace("the ", "")
    return "".join(c for c in cleaned.title() if c.isalnum()) or "Kit"


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
| `06_MATCHED_MINI_KITS/` | Oneshots + BPM loop + glued bus + cassette A/B |
| `07_DESK_PRESETS/` | SP_HOLD / CASSETTE / CRUSHED / TRUNK |
| `08_STUDIO_24BIT/` | 24-bit mono heroes (Splice-friendly lane) |
| `09_STEREO_TOPS/` | Haas-widened snares/hats/claps |
| `10_VELOCITY_LAYERS/` | Soft / Hard variants of hero hits |
| `11_CONSTRUCTION_KITS/` | Oneshots + loop + MIDI + stems idea |
| `12_STARTER_HERO/` | Curated deadly bank (not the whole archive) |
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


def _build_pitched(pack: Path, masters: Path, profile: dict) -> int:
    dest = pack / "02_BITCRUSHED_AND_PITCHED"
    n = 0
    sources = _folders_for_kinds(masters, profile, ("808", "KICK"))
    if not sources:
        sources = _oneshot_folders(masters, profile)[:1]
    rate = dest / "RateDropped_808s"
    rate.mkdir(parents=True)
    for folder in sources[:2]:
        for path in _sample_wavs(folder, 8):
            buf = read_wav_mono(path)
            for st in (-12, -24):
                write_wav_mono(rate / f"{path.stem}_pitch{st}st.wav", pitch_shift(buf, st))
                n += 1

    pitched = dest / "Pitched_Tops"
    pitched.mkdir(parents=True)
    tops = _folders_for_kinds(masters, profile, ("SNR", "COW", "CLAP", "CH", "OH"))
    if tops:
        for path in _sample_wavs(tops[0], 4):
            buf = read_wav_mono(path)
            for st in (-7, 5):
                write_wav_mono(pitched / f"{path.stem}_pitch{st}st.wav", pitch_shift(buf, st))
                n += 1

    aliased = dest / "Aliased_One_Shots"
    aliased.mkdir(parents=True)
    for folder in _oneshot_folders(masters, profile)[:3]:
        for path in _sample_wavs(folder, 3):
            buf = read_wav_mono(path)
            write_wav_mono(aliased / f"{path.stem}_alias.wav", bitcrush(buf, bit_depth=9, hold_sr=18000.0))
            n += 1
    return n


def _build_cassette(pack: Path, masters: Path, profile: dict) -> int:
    dest = pack / "03_CASSETTE_AND_TAPE"
    n = 0
    for folder in _oneshot_folders(masters, profile):
        out = dest / folder.name
        out.mkdir(parents=True)
        for i, path in enumerate(_sample_wavs(folder, 4)):
            write_wav_mono(out / f"{path.stem}_cassette.wav", cassette_fx(read_wav_mono(path), seed=100 + i))
            n += 1
    return n


def _build_parallel(pack: Path, masters: Path, profile: dict) -> int:
    dest = pack / "04_PARALLEL_PROCESS_BANKS"
    n = 0
    prefix = profile["prefix"]
    roles = _oneshot_folders(masters, profile)[:6]
    for lane in ("RAW", "TAPE", "LOFI", "CASSETTE"):
        for folder in roles:
            role = folder.name.split("_", 1)[-1] if "_" in folder.name else folder.name
            out = dest / lane / role
            out.mkdir(parents=True)
            for i, path in enumerate(_sample_wavs(folder, 3)):
                processed = process_lane(read_wav_mono(path), lane, seed=i)
                write_wav_mono(out / f"{prefix}_{lane}_{role}_{i+1:02d}.wav", processed)
                n += 1
    return n


def _pick_wav(folder: Path | None, index: int = 0) -> Path | None:
    if not folder:
        return None
    wavs = _sorted_wavs(folder)
    return wavs[index] if index < len(wavs) else None


def _build_mini_kits(pack: Path, masters: Path, profile: dict) -> int:
    """Matched mini-kits = select-from-masters + cassette A/B (no re-synthesis)."""
    root = pack / "06_MATCHED_MINI_KITS"
    slug = _identity_slug(profile)
    prefix = profile["prefix"]
    n = 0
    loop_folder = _loop_folder(masters, profile)
    loop_wavs = _sorted_wavs(loop_folder) if loop_folder else []
    role_kinds = (
        ("Kick", ("808", "KICK")),
        ("Snare", ("SNR",)),
        ("HatClosed", ("CH",)),
        ("Clap", ("CLAP",)),
        ("Cowbell", ("COW",)),
    )
    kit_ids = (f"01_{slug}", "02_KitB", "03_KitC")
    for kit_i, kid in enumerate(kit_ids):
        kdir = root / kid
        oneshots = kdir / "OneShots"
        oneshots.mkdir(parents=True)
        kick_path = None
        for role, kinds in role_kinds:
            path = None
            for kind in kinds:
                folder = _folder_for_kind(masters, profile, kind)
                path = _pick_wav(folder, kit_i)
                if path:
                    break
            if not path:
                continue
            dest = oneshots / f"{prefix}_{kid}_{role}.wav"
            shutil.copy2(path, dest)
            n += 1
            if role == "Kick":
                kick_path = path
        if kick_path:
            kick_buf = read_wav_mono(kick_path)
            write_wav_mono(
                oneshots / f"{prefix}_{kid}_Kick_Cassette.wav",
                cassette_fx(kick_buf, seed=100 + kit_i),
            )
            write_wav_mono(
                oneshots / f"{prefix}_{kid}_Kick_Glued.wav",
                L.bus_glue(kick_buf, drive_db=2.8, lpf_hz=13000.0),
            )
            n += 2
        if kit_i < len(loop_wavs):
            loop_src = loop_wavs[kit_i]
            # Preserve BPM token from filename when present
            bpm_token = "94"
            for part in loop_src.stem.split("_"):
                if part.endswith("bpm"):
                    bpm_token = part.replace("bpm", "")
                    break
            loop_buf = L.bus_glue(read_wav_mono(loop_src), drive_db=1.8, lpf_hz=15000.0)
            write_wav_mono(kdir / f"{prefix}_{kid}_LOOP_{bpm_token}bpm.wav", loop_buf)
            mid = loop_src.with_suffix(".mid")
            if mid.exists():
                shutil.copy2(mid, kdir / f"{prefix}_{kid}_LOOP_{bpm_token}bpm.mid")
            n += 1
        (kdir / "README.txt").write_text(
            f"Mini-kit {kid}\nKick RAW / Glued / Cassette A/B + glued loop.\n"
            "See _DOCS/Hardware_Processing_Guide.md\n",
            encoding="utf-8",
        )
    return n


def _build_desk_presets(pack: Path, masters: Path, profile: dict) -> int:
    dest = pack / "07_DESK_PRESETS"
    n = 0
    prefix = profile["prefix"]
    roles = _oneshot_folders(masters, profile)[:5]
    for preset in DESK_PRESETS:
        for folder in roles:
            role = folder.name.split("_", 1)[-1] if "_" in folder.name else folder.name
            out = dest / preset / role
            out.mkdir(parents=True)
            for i, path in enumerate(_sample_wavs(folder, 2)):
                write_wav_mono(
                    out / f"{prefix}_{preset}_{role}_{i+1:02d}.wav",
                    process_desk(read_wav_mono(path), preset, seed=i),
                )
                n += 1
    return n


def _build_studio_24(pack: Path, masters: Path, profile: dict, heroes: list[Path]) -> int:
    dest = pack / "08_STUDIO_24BIT"
    dest.mkdir(parents=True)
    n = 0
    for path in heroes[:48]:
        write_wav_24(dest / f"{path.stem}_24bit.wav", read_wav_mono(path))
        n += 1
    return n


def _build_stereo_tops(pack: Path, masters: Path, profile: dict) -> int:
    dest = pack / "09_STEREO_TOPS"
    n = 0
    for kind in ("SNR", "CH", "OH", "CLAP"):
        folder = _folder_for_kind(masters, profile, kind)
        if not folder:
            continue
        out = dest / kind
        out.mkdir(parents=True)
        for path in _sample_wavs(folder, 6):
            stereo = L.stereo_widen(read_wav_mono(path), width=0.32)
            write_wav_stereo_16(out / f"{path.stem}_STEREO.wav", stereo)
            n += 1
    return n


def _build_velocity(pack: Path, masters: Path, profile: dict, heroes: list[Path]) -> int:
    dest = pack / "10_VELOCITY_LAYERS"
    soft_d = dest / "Soft"
    hard_d = dest / "Hard"
    soft_d.mkdir(parents=True, exist_ok=True)
    hard_d.mkdir(parents=True, exist_ok=True)
    n = 0
    for path in heroes[:36]:
        buf = read_wav_mono(path)
        write_wav_mono(soft_d / f"{path.stem}_Soft.wav", L.apply_velocity(buf, 0.58))
        write_wav_mono(hard_d / f"{path.stem}_Hard.wav", L.apply_velocity(buf, 1.0))
        n += 2
    return n


def _build_construction_kits(pack: Path, masters: Path, profile: dict) -> int:
    """Construction kit = kick/snare/hat + matching loop + MIDI when present."""
    root = pack / "11_CONSTRUCTION_KITS"
    prefix = profile["prefix"]
    loops = _sorted_wavs(_loop_folder(masters, profile))
    n = 0
    for i in range(min(4, len(loops))):
        kdir = root / f"{i+1:02d}_Kit"
        ones = kdir / "OneShots"
        ones.mkdir(parents=True)
        for role, kinds in (("Kick", ("808", "KICK")), ("Snare", ("SNR",)), ("Hat", ("CH",))):
            path = None
            for kind in kinds:
                path = _pick_wav(_folder_for_kind(masters, profile, kind), i)
                if path:
                    break
            if path:
                write_wav_mono(ones / f"{prefix}_CK{i+1:02d}_{role}.wav", L.bus_glue(read_wav_mono(path)))
                n += 1
        loop = loops[i]
        write_wav_mono(kdir / loop.name, L.bus_glue(read_wav_mono(loop), drive_db=1.5))
        n += 1
        mid = loop.with_suffix(".mid")
        if mid.exists():
            shutil.copy2(mid, kdir / mid.name)
        (kdir / "README.txt").write_text(
            "Construction kit: glued oneshots + loop (+ MIDI if present).\n",
            encoding="utf-8",
        )
    return n


def _build_starter_hero(pack: Path, heroes: list[Path], profile: dict) -> int:
    dest = pack / "12_STARTER_HERO"
    dest.mkdir(parents=True)
    n = 0
    for path in heroes:
        # Use-case style name: keep recipe tag visible
        shutil.copy2(path, dest / path.name)
        n += 1
    manifest = build_hero_manifest(heroes, profile)
    (dest / "HERO_MANIFEST.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    (dest / "README.txt").write_text(
        "Curated hero bank — fewer, deadlier hits than the full archive.\n"
        "Prefer these for demos / store previews; full cores stay in 01_CORE_ONESHOTS.\n",
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
    loops = _loop_folder(masters, profile)
    for folder in _oneshot_folders(masters, profile):
        shutil.copytree(folder, core / folder.name)
        core_count += len(_sorted_wavs(folder))

    loop_count = 0
    if loops:
        shutil.copytree(loops, pack / "05_LOOPS")
        loop_count = len(_sorted_wavs(pack / "05_LOOPS"))

    pitched_n = _build_pitched(pack, masters, profile)
    cassette_n = _build_cassette(pack, masters, profile)
    parallel_n = _build_parallel(pack, masters, profile)
    mini_n = _build_mini_kits(pack, masters, profile)
    heroes = select_heroes(masters, per_kind=8, max_total=96)
    desk_n = _build_desk_presets(pack, masters, profile)
    studio_n = _build_studio_24(pack, masters, profile, heroes)
    stereo_n = _build_stereo_tops(pack, masters, profile)
    vel_n = _build_velocity(pack, masters, profile, heroes)
    ck_n = _build_construction_kits(pack, masters, profile)
    hero_n = _build_starter_hero(pack, heroes, profile)
    # Copy loop MIDI alongside loops folder
    if loops:
        for mid in loops.glob("*.mid"):
            shutil.copy2(mid, pack / "05_LOOPS" / mid.name)
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
                "Format: 44.1 kHz · PCM16 mono cores · 24-bit Studio lane · stereo tops",
                "Includes: cores + FX banks + desk presets + construction kits + Starter Hero",
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
            "desk_presets": desk_n,
            "studio_24bit": studio_n,
            "stereo_tops": stereo_n,
            "velocity_layers": vel_n,
            "construction_kits": ck_n,
            "starter_hero": hero_n,
        },
        "features": [
            "core_oneshots",
            "bitcrushed_and_pitched",
            "cassette_fx",
            "parallel_RAW_TAPE_LOFI_CASSETTE",
            "matched_mini_kits",
            "desk_presets",
            "studio_24bit_lane",
            "stereo_tops",
            "velocity_layers",
            "construction_kits",
            "starter_hero_bank",
            "hardware_processing_guide",
        ],
        "delivery": {"sample_rate": 44100, "bit_depth": 16, "channels": 1, "studio_lane_bit_depth": 24},
    }
    (docs / "pack_manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    return manifest


def zip_pack(pack_root: Path, zip_path: Path, pack_name: str) -> None:
    zip_tree(pack_root, zip_path, pack_name)


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
