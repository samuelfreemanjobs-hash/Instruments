#!/usr/bin/env python3
"""Build the Phonk Drum Studio standalone desktop application."""

from __future__ import annotations

import platform
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
SPEC = ROOT / "PhonkDrumStudio.spec"
APP_NAME = "PhonkDrumStudio"


def build() -> int:
    if not (ROOT / "run_desktop.py").is_file():
        print(f"Missing entry: {ROOT / 'run_desktop.py'}", file=sys.stderr)
        return 1

    cmd = [sys.executable, "-m", "PyInstaller", "--noconfirm", "--clean", str(SPEC)]
    print("Running:", " ".join(cmd))
    result = subprocess.run(cmd, cwd=ROOT, check=False)
    if result.returncode != 0:
        return result.returncode

    system = platform.system()
    dist = ROOT / "dist"
    if system == "Darwin":
        artifact = dist / f"{APP_NAME}.app"
    elif system == "Windows":
        artifact = dist / f"{APP_NAME}.exe"
    else:
        artifact = dist / APP_NAME

    print(f"Build finished. Desktop app: {artifact}")
    print("Install: run the executable directly (Linux/macOS may require PortAudio + tk).")
    return 0


if __name__ == "__main__":
    raise SystemExit(build())
