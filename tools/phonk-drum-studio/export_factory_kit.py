#!/usr/bin/env python3
"""CLI wrapper: Phonk Drum Studio WAV folder → Disklordz factory kit."""

from __future__ import annotations

import runpy
import sys
from pathlib import Path

SCRIPT = (
    Path(__file__).resolve().parents[2]
    / "disklordz"
    / "sound-factory"
    / "scripts"
    / "build_studio_kit.py"
)

if __name__ == "__main__":
    sys.argv[0] = str(SCRIPT)
    runpy.run_path(str(SCRIPT), run_name="__main__")
