#!/usr/bin/env python3
"""Build a standalone Phonk Drum Studio bundle with PyInstaller."""

from __future__ import annotations

import platform
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
APP_ENTRY = ROOT / "app.py"
APP_NAME = "PhonkDrumStudio"


def build() -> int:
    if not APP_ENTRY.is_file():
        print(f"Missing entry point: {APP_ENTRY}", file=sys.stderr)
        return 1

    system = platform.system()
    sep = ";" if system == "Windows" else ":"

    hidden_imports = [
        "torch",
        "torchaudio",
        "soundfile",
        "sounddevice",
        "customtkinter",
        "numpy",
        "_soundfile_data",
    ]

    cmd = [
        sys.executable,
        "-m",
        "PyInstaller",
        "--noconfirm",
        "--clean",
        "--windowed",
        "--onefile",
        f"--name={APP_NAME}",
        f"--paths={ROOT}",
    ]

    for mod in hidden_imports:
        cmd.append(f"--hidden-import={mod}")

    # Bundle CustomTkinter theme assets when present.
    try:
        import customtkinter

        ctk_dir = Path(customtkinter.__file__).resolve().parent
        assets = ctk_dir / "assets"
        if assets.is_dir():
            cmd.append(f"--add-data={assets}{sep}customtkinter/assets")
    except ImportError:
        print("Warning: customtkinter not installed; asset bundling skipped.", file=sys.stderr)

    cmd.append(str(APP_ENTRY))

    print("Running:", " ".join(cmd))
    result = subprocess.run(cmd, cwd=ROOT, check=False)
    if result.returncode != 0:
        return result.returncode

    dist = ROOT / "dist"
    if system == "Darwin":
        artifact = dist / f"{APP_NAME}.app"
    elif system == "Windows":
        artifact = dist / f"{APP_NAME}.exe"
    else:
        artifact = dist / APP_NAME

    print(f"Build finished. Artifact: {artifact}")
    return 0


if __name__ == "__main__":
    raise SystemExit(build())
