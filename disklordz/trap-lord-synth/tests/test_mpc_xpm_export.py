#!/usr/bin/env python3
"""Validate MPC .xpm keygroup folders (structure + WAV presence). Not hardware verified."""

from __future__ import annotations

import argparse
import sys
import wave
import xml.etree.ElementTree as ET
from pathlib import Path


def validate_folder(folder: Path) -> None:
    xpms = list(folder.glob("*.xpm")) + list(folder.glob("**/*.xpm"))
    assert xpms, f"No .xpm in {folder}"
    xpm = xpms[0]
    root = ET.parse(xpm).getroot()
    assert root.tag in {"KeygroupProgram", "Program"}, root.tag
    wavs = list(folder.glob("*.WAV")) + list(folder.glob("*.wav"))
    assert wavs, "No WAV samples"
    for w in wavs:
        with wave.open(str(w), "rb") as wf:
            assert wf.getnframes() > 64
            assert wf.getframerate() == 44100
    print(f"OK {folder} ({len(wavs)} WAV, xpm={xpm.name})")


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("folder", type=Path, nargs="?", default=None)
    args = ap.parse_args()
    if args.folder:
        validate_folder(args.folder)
        return 0
    print("No folder provided — schema self-check only")
    assert ET.fromstring(
        '<?xml version="1.0"?><KeygroupProgram><Version><FileVersion>5000</FileVersion></Version></KeygroupProgram>'
    ).tag == "KeygroupProgram"
    print("OK xml schema smoke")
    return 0


if __name__ == "__main__":
    sys.exit(main())
