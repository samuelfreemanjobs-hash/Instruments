#!/usr/bin/env python3
"""Build headless CLI, render 24 audition WAVs + example MPC export, zip artifacts."""

from __future__ import annotations

import json
import shutil
import subprocess
import sys
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BUILD = ROOT / "build"
CLI = BUILD / "TrapLordSynthRenderCLI_artefacts" / "Release" / "TrapLordSynthRenderCLI"
if sys.platform == "win32":
    CLI = CLI.with_suffix(".exe")

ARTIFACTS = Path("/opt/cursor/artifacts")
AUDITION_ZIP = ARTIFACTS / "trap-lord-synth-auditions-24.zip"
MPC_ZIP = ARTIFACTS / "trap-lord-synth-mpc-brass-example.zip"
MANIFEST = ROOT / "assets/factory/audition_manifest.json"


def run(cmd: list[str], **kwargs) -> None:
    print("+", " ".join(cmd))
    subprocess.check_call(cmd, **kwargs)


def cmake_build_cli() -> None:
    if not BUILD.exists():
        run(
            [
                "cmake",
                "-B",
                str(BUILD),
                "-DCMAKE_BUILD_TYPE=Release",
                "-DCMAKE_CXX_COMPILER=g++-12",
                "-DCMAKE_C_COMPILER=gcc-12",
            ],
            cwd=ROOT,
        )
    run(["cmake", "--build", str(BUILD), "-j", "--target", "TrapLordSynthRenderCLI"], cwd=ROOT)
    if not CLI.exists():
        raise SystemExit(f"CLI missing: {CLI}")


def pick_brass_preset() -> int:
    bank = json.loads((ROOT / "assets/factory/preset_bank.json").read_text(encoding="utf-8"))
    for i, p in enumerate(bank["presets"]):
        if p["category"] == "Brass":
            return i
    raise SystemExit("No Brass preset in bank")


def main() -> int:
    ARTIFACTS.mkdir(parents=True, exist_ok=True)
    stage = ARTIFACTS / "trap-lord-auditions-staging"
    mpc_stage = ARTIFACTS / "trap-lord-mpc-staging"
    shutil.rmtree(stage, ignore_errors=True)
    shutil.rmtree(mpc_stage, ignore_errors=True)
    stage.mkdir(parents=True)
    mpc_stage.mkdir(parents=True)

    cmake_build_cli()
    run([str(CLI), "auditions", str(stage)], cwd=ROOT)

    wavs = sorted(stage.glob("*.wav"))
    if len(wavs) != 24:
        raise SystemExit(f"Expected 24 WAV files, got {len(wavs)}")

    # Regenerate manifest (2 per category)
    bank = json.loads((ROOT / "assets/factory/preset_bank.json").read_text(encoding="utf-8"))
    by_cat: dict[str, list] = {}
    for i, p in enumerate(bank["presets"]):
        by_cat.setdefault(p["category"], []).append({"index": i, "name": p["name"]})
    auditions = []
    for cat in sorted(by_cat):
        for slot, pick in enumerate(by_cat[cat][:2], start=1):
            auditions.append({"category": cat, "slot": slot, **pick})
    MANIFEST.write_text(json.dumps({"auditions": auditions, "wavCount": 24}, indent=2), encoding="utf-8")

    with zipfile.ZipFile(AUDITION_ZIP, "w", compression=zipfile.ZIP_DEFLATED) as zf:
        for w in wavs:
            zf.write(w, w.name)
        zf.write(MANIFEST, "audition_manifest.json")

    brass_idx = pick_brass_preset()
    run([str(CLI), "mpc", str(brass_idx), str(mpc_stage)], cwd=ROOT)

    prog_dirs = [p for p in mpc_stage.iterdir() if p.is_dir()]
    if not prog_dirs:
        raise SystemExit("MPC export produced no program folder")
    mpc_folder = prog_dirs[0]

    run([sys.executable, str(ROOT / "tests/test_mpc_xpm_export.py"), str(mpc_folder)], cwd=ROOT)

    with zipfile.ZipFile(MPC_ZIP, "w", compression=zipfile.ZIP_DEFLATED) as zf:
        for path in mpc_folder.rglob("*"):
            if path.is_file():
                zf.write(path, path.relative_to(mpc_stage).as_posix())

    print(f"Wrote {AUDITION_ZIP} ({len(wavs)} WAV)")
    print(f"Wrote {MPC_ZIP} (validator OK on {mpc_folder.name})")
    print(f"Brass preset index: {brass_idx}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
