#!/usr/bin/env python3
"""Split large MPC keygroup folders into <24 MB artifact zips (octave range + single velocity layer)."""

from __future__ import annotations

import re
import shutil
import subprocess
import sys
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CLI = ROOT / "build" / "TrapLordSynthRenderCLI_artefacts" / "Release" / "TrapLordSynthRenderCLI"
ARTIFACTS = Path("/opt/cursor/artifacts")
MAX_BYTES = 24 * 1024 * 1024


def build_cli() -> None:
    subprocess.check_call(["cmake", "--build", str(ROOT / "build"), "-j", "--target", "TrapLordSynthRenderCLI"], cwd=ROOT)


def zip_folder(folder: Path, out_zip: Path) -> int:
    out_zip.parent.mkdir(parents=True, exist_ok=True)
    if out_zip.exists():
        out_zip.unlink()
    with zipfile.ZipFile(out_zip, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=6) as zf:
        for f in sorted(folder.rglob("*")):
            if f.is_file():
                zf.write(f, f.relative_to(folder.parent).as_posix())
    return out_zip.stat().st_size


def filter_program(src: Path, dst: Path, lo: int, hi: int) -> None:
    """Copy keygroup program with only notes lo..hi inclusive."""
    dst.mkdir(parents=True, exist_ok=True)
    xpm = next(src.glob("*.xpm"))
    text = xpm.read_text(encoding="utf-8")
    dst_xpm = dst / xpm.name
    dst_xpm.write_text(text, encoding="utf-8")

    note_re = re.compile(r"DL_N(\d{3})_L(\d+)\.WAV", re.I)
    for wav in src.glob("*.WAV"):
        m = note_re.match(wav.name)
        if not m:
            continue
        note = int(m.group(1))
        if lo <= note <= hi:
            shutil.copy2(wav, dst / wav.name)

    # trim xpm instruments to only copied notes — validator runs on subset
    import xml.etree.ElementTree as ET

    tree = ET.parse(dst_xpm)
    root = tree.getroot()
    instruments = root.find(".//Instruments")
    if instruments is None:
        return
    kept = []
    for inst in list(instruments.findall("Instrument")):
        lo_n = int(inst.findtext("LowNote", "0"))
        if lo <= lo_n <= hi:
            kept.append(inst)
        else:
            instruments.remove(inst)
    program = root.find(".//Program")
    if program is not None:
        num = program.find("NumKeygroups")
        if num is not None:
            num.text = str(len(kept))
    tree.write(dst_xpm, encoding="utf-8", xml_declaration=True)


def export_and_split(preset_index: int, label: str) -> list[Path]:
    staging = ARTIFACTS / f"mpc-split-staging-{label}"
    shutil.rmtree(staging, ignore_errors=True)
    staging.mkdir(parents=True)
    subprocess.check_call([str(CLI), "mpc", str(preset_index), str(staging), "1"], cwd=ROOT)
    prog_dirs = [p for p in staging.iterdir() if p.is_dir()]
    if not prog_dirs:
        raise SystemExit(f"No MPC program for preset {preset_index}")
    src = prog_dirs[0]

    outputs: list[Path] = []
    ranges = [(36, 48, "C1-C3"), (49, 60, "C4-C5"), (61, 72, "C6-C7")]
    for lo, hi, tag in ranges:
        part = staging / f"{src.name}_{tag}"
        filter_program(src, part, lo, hi)
        out = ARTIFACTS / f"trap-lord-mpc-live-{label}-{tag.lower()}.zip"
        size = zip_folder(part, out)
        if size <= MAX_BYTES:
            outputs.append(out)
            print(f"Wrote {out} ({size / 1e6:.1f} MB)")
        else:
            print(f"WARN {out} still {size / 1e6:.1f} MB — try smaller range")

    return outputs


def main() -> int:
    import json

    build_cli()
    bank = json.loads((ROOT / "assets/factory/preset_bank.json").read_text(encoding="utf-8"))
    presets = bank["presets"]

    manifest_path = ROOT / "assets/factory/audition_manifest.json"

    def pick_example_index(cat: str) -> int:
        if manifest_path.exists():
            man = json.loads(manifest_path.read_text(encoding="utf-8"))
            for row in man.get("auditions", []):
                if row.get("category") == cat and int(row.get("slot", 1)) == 1:
                    return int(row["index"])
        for i, p in enumerate(presets):
            if p["category"] == cat:
                return i
        return 0

    export_and_split(pick_example_index("Pads"), "pad")
    export_and_split(pick_example_index("Swells"), "swell")
    return 0


if __name__ == "__main__":
    sys.exit(main())
