#!/usr/bin/env python3
"""Build Brass / Pad / Swell MPC Live keygroup ZIPs with README (not hardware verified)."""

from __future__ import annotations

import json
import subprocess
import sys
import shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BUILD = ROOT / "build"
CLI = BUILD / "TrapLordSynthRenderCLI_artefacts" / "Release" / "TrapLordSynthRenderCLI"
ARTIFACTS = Path("/opt/cursor/artifacts")
BUILD_OUT = ROOT / "build" / "mpc-live-release"
STAGING = BUILD_OUT / "mpc-live-staging"


def ensure_staging_root() -> Path:
    preferred = ARTIFACTS / "mpc-live-staging"
    try:
        preferred.mkdir(parents=True, exist_ok=True)
        probe = preferred / ".write_probe"
        probe.write_text("ok", encoding="utf-8")
        probe.unlink()
        return preferred
    except OSError as err:
        print(f"WARN: MPC staging in artifacts not writable ({err}); using build/")
        STAGING.mkdir(parents=True, exist_ok=True)
        return STAGING

README = """# TRAP LORD SYNTH — MPC Live keygroup (DiskLordz)

Neutral factory preset exported for **Akai MPC Live / MPC One / MPC X** (MPC OS 2.x / 3.x).
**Not verified on physical hardware in CI** — validate in Browser before a live set.

## Copy to SD or USB

1. Unzip this package on your computer.
2. Copy the program folder (contains `.xpm` + `.WAV` files) to:
   - **SD card:** `/Volumes/<your-sd>/` (or `MPC DATA` partition root), **or**
   - **USB stick:** root of the drive formatted FAT32/exFAT.
3. Keep **all `.WAV` files in the same folder** as the `.xpm` (do not separate them).

Optional Expansions-style layout is not used in these ZIPs; each ZIP is one self-contained program folder ready for Browser import.

## Load on the MPC Live

1. Insert the SD/USB into the MPC.
2. Open **Menu → Browser** (or **Programs** depending on firmware).
3. Navigate to the folder you copied (e.g. `DL_{program}`).
4. Select the **`.xpm`** file and load the keygroup program.
5. Assign the program to a track; play chromatically from **C1–C6** (MIDI 36–96) on pads or an external keyboard.

## Technical notes

- Sample rate **44.1 kHz**, **24-bit** stereo WAV; levels normalized per program (~-1 dBFS peak on the loudest sample).
- **2 velocity layers** per note, **≤4 layers** per keygroup (MPC limit).
- Filename pattern: `DL_N036_L1.WAV` (ASCII, short names).
- Root note / key tracking enabled for chromatic playback.

## Risks

- Envelope release in the `.xpm` is a simplified match to the plugin; long tails may differ slightly on hardware.
- Very fast runs or heavy sequences may stress MPC RAM — test polyphony on device.

— DiskLordz Audio / TRAP LORD SYNTH factory export
"""


def run(cmd: list[str]) -> None:
    print("+", " ".join(cmd))
    subprocess.check_call(cmd, cwd=ROOT)


def find_program_folder(parent: Path, prefix: str) -> Path:
    matches = sorted(parent.glob(f"{prefix}*"))
    assert len(matches) == 1, f"Expected one DL_ folder under {parent}, got {matches}"
    return matches[0]


def export_preset(category: str, preset_index: int, zip_name: str) -> Path:
    staging_root = ensure_staging_root()
    work = staging_root / zip_name.replace(".zip", "")
    if work.exists():
        shutil.rmtree(work, ignore_errors=True)
    work.mkdir(parents=True, exist_ok=True)
    run([str(CLI), "mpc", str(preset_index), str(work)])
    folder = find_program_folder(work, "DL_")
    run([sys.executable, str(ROOT / "tests/test_mpc_xpm_export.py"), str(folder)])

    readme = folder / "README_MPC_LIVE.txt"
    bank = json.loads((ROOT / "assets/factory/preset_bank.json").read_text(encoding="utf-8"))
    preset_name = bank["presets"][preset_index]["name"]
    readme.write_text(
        README.replace("{program}", folder.name)
        + f"\nPreset: {preset_name} ({category}, index {preset_index})\n",
        encoding="utf-8",
    )

    BUILD_OUT.mkdir(parents=True, exist_ok=True)
    out_zip = BUILD_OUT / zip_name
    artifacts_copy = ARTIFACTS / zip_name
    import tempfile

    archive_base = out_zip.with_suffix("")
    if out_zip.exists():
        out_zip.unlink(missing_ok=True)
    with tempfile.TemporaryDirectory() as tmp:
        tmp_base = Path(tmp) / "archive"
        shutil.make_archive(str(tmp_base), "zip", work, folder.name)
        shutil.copyfile(tmp_base.with_suffix(".zip"), out_zip)
    try:
        if artifacts_copy.exists():
            artifacts_copy.unlink()
        subprocess.run(["cp", "-f", str(out_zip), str(artifacts_copy)], check=True)
    except (OSError, subprocess.CalledProcessError) as err:
        print(f"WARN: could not copy {out_zip.name} to artifacts ({err}); kept under {out_zip}")
    else:
        print(f"Wrote {artifacts_copy} ({folder.name})")
    print(f"Wrote {out_zip} ({folder.name})")
    return folder


def main() -> int:
    ARTIFACTS.mkdir(parents=True, exist_ok=True)
    import shutil

    if STAGING.exists():
        shutil.rmtree(STAGING, ignore_errors=True)
    STAGING.mkdir(parents=True, exist_ok=True)

    if not BUILD.exists():
        run(
            [
                "cmake",
                "-B",
                str(BUILD),
                "-DCMAKE_BUILD_TYPE=Release",
                "-DCMAKE_CXX_COMPILER=g++-12",
                "-DCMAKE_C_COMPILER=gcc-12",
            ]
        )
    run(["cmake", "--build", str(BUILD), "-j", "--target", "TrapLordSynthRenderCLI"])
    assert CLI.exists(), CLI

    bank = json.loads((ROOT / "assets/factory/preset_bank.json").read_text(encoding="utf-8"))

    manifest = json.loads((ROOT / "assets/factory/audition_manifest.json").read_text(encoding="utf-8"))

    def manifest_idx(cat: str, slot: int = 1) -> int:
        for entry in manifest.get("auditions", []):
            if entry.get("category") == cat and int(entry.get("slot", 1)) == slot:
                return int(entry["index"])
        for i, p in enumerate(bank["presets"]):
            if p["category"] == cat:
                return i
        raise SystemExit(f"No preset in category {cat}")

    export_preset("Brass", manifest_idx("Brass", 1), "trap-lord-mpc-live-brass.zip")
    export_preset("Pads", manifest_idx("Pads", 1), "trap-lord-mpc-live-pad.zip")
    export_preset("Swells", manifest_idx("Swells", 1), "trap-lord-mpc-live-swell.zip")

    import zipfile

    for name in ("trap-lord-mpc-live-pad", "trap-lord-mpc-live-swell"):
        full = BUILD_OUT / f"{name}.zip"
        artifact = ARTIFACTS / f"{name}.zip"
        splits = sorted(ARTIFACTS.glob(f"{name}-c*.zip"))
        artifact_bad = (
            not artifact.exists() or artifact.stat().st_size < 1024
        )
        full_too_large = full.exists() and full.stat().st_size > 48 * 1024 * 1024
        if len(splits) >= 3 and artifact_bad and (full_too_large or not full.exists()):
            with zipfile.ZipFile(artifact, "w", zipfile.ZIP_DEFLATED, compresslevel=6) as zf:
                for part in splits:
                    zf.write(part, part.name)
            print(
                f"Wrote bundled {artifact} ({artifact.stat().st_size} bytes) "
                f"from {len(splits)} splits"
            )
    return 0


if __name__ == "__main__":
    sys.exit(main())
