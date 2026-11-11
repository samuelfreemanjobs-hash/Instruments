#!/usr/bin/env python3
"""Validate MPC Live-style keygroup folders (.xpm + .WAV). Not hardware verified."""

from __future__ import annotations

import argparse
import re
import sys
import wave
import xml.etree.ElementTree as ET
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
from trap_lord_audio_metrics import (  # noqa: E402
    MPC_NOTE_LEVEL_SPREAD_DB_MAX,
    mpc_program_level_spread_db,
    mpc_program_levels_ok,
)

ASCII_WAV = re.compile(r"^DL_N\d{3}_L[1-4]\.WAV$")
MAX_PROGRAM_BYTES = 125 * 1024 * 1024
EXPECTED_FILE_VERSION = "5000"


def text(elem: ET.Element, tag: str, default: str = "") -> str:
    child = elem.find(tag)
    return (child.text or default).strip() if child is not None else default


def validate_folder(folder: Path, *, chromatic_lo: int = 36, chromatic_hi: int = 96) -> None:
    xpms = sorted(folder.glob("*.xpm"))
    assert xpms, f"No .xpm in {folder}"
    assert len(xpms) == 1, f"Expected one .xpm, got {len(xpms)}"
    xpm = xpms[0]
    root = ET.parse(xpm).getroot()
    assert root.tag == "KeygroupProgram", root.tag
    version_el = root.find("Version")
    fv = text(version_el, "FileVersion") if version_el is not None else text(root, "FileVersion")
    assert fv == EXPECTED_FILE_VERSION, f"FileVersion {fv!r} != {EXPECTED_FILE_VERSION}"

    program = root.find("Program")
    assert program is not None
    assert text(program, "ProgramType") == "keygroup"

    instruments = program.find("Instruments")
    assert instruments is not None
    instruments_list = list(instruments.findall("Instrument"))
    assert instruments_list, "No Instrument entries"

    covered: set[int] = set()
    total_layers = 0
    for inst in instruments_list:
        lo = int(text(inst, "LowNote", "0"))
        hi = int(text(inst, "HighNote", "127"))
        assert lo == hi, f"Expected one note per keygroup, got {lo}-{hi}"
        covered.add(lo)
        layers = inst.find("Layers")
        assert layers is not None
        layer_els = list(layers.findall("Layer"))
        assert 1 <= len(layer_els) <= 4, f"Layer count {len(layer_els)}"
        total_layers += len(layer_els)
        for layer in layer_els:
            assert text(layer, "KeyTrack", "").lower() in {"true", "1"}
            root_note = int(text(layer, "RootNote", "61"))
            assert root_note == lo + 1, f"RootNote {root_note} expected {lo + 1}"
            name = text(layer, "SampleName")
            assert ASCII_WAV.match(name), f"Bad sample name {name!r}"

    expected = set(range(chromatic_lo, chromatic_hi + 1))
    missing = expected - covered
    assert not missing, f"Missing chromatic notes: {sorted(missing)[:8]}... ({len(missing)} total)"
    assert len(covered) == len(expected), f"Expected {len(expected)} keygroups, got {len(covered)}"

    wavs = sorted(folder.glob("*.WAV"))
    assert len(wavs) == total_layers, f"WAV count {len(wavs)} != layer count {total_layers}"
    bytes_total = 0
    for w in wavs:
        assert ASCII_WAV.match(w.name), f"Bad WAV filename {w.name}"
        with wave.open(str(w), "rb") as wf:
            assert wf.getframerate() == 44100
            assert wf.getnchannels() == 2
            assert wf.getsampwidth() in (2, 3)
            assert wf.getnframes() > 64
        bytes_total += w.stat().st_size

    assert bytes_total <= MAX_PROGRAM_BYTES, f"Program size {bytes_total} exceeds budget"

    ok, spread, msg = mpc_program_levels_ok(folder, MPC_NOTE_LEVEL_SPREAD_DB_MAX)
    assert ok, f"Note level spread {spread:.1f} dB exceeds {MPC_NOTE_LEVEL_SPREAD_DB_MAX} dB ({msg})"

    print(
        f"OK {folder.name} ({len(instruments_list)} keygroups, {len(wavs)} WAV, "
        f"{bytes_total / (1024 * 1024):.1f} MB, xpm={xpm.name})"
    )


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
