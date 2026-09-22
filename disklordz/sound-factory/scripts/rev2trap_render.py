"""Render multisample zones via Rev2TrapOfflineRender binary."""

from __future__ import annotations

import hashlib
import json
import subprocess
from pathlib import Path

from factory_qa import analyze_wav
from trap_synth import default_multisample_roots
from wav_util import SAMPLE_RATE

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_BINARY = ROOT.parents[1] / "build" / "Rev2Trap" / "Rev2TrapOfflineRender"


def find_binary(explicit: Path | None = None) -> Path:
    if explicit and explicit.is_file():
        return explicit
    if DEFAULT_BINARY.is_file():
        return DEFAULT_BINARY
    raise FileNotFoundError(
        "Rev2TrapOfflineRender not found; build with: "
        "cmake --build build -j --target Rev2TrapOfflineRender"
    )


def program_index_from_slot(slot: dict) -> int:
    sid = slot.get("slotId", "REV2_001")
    try:
        num = int(sid.split("_")[-1])
        return max(0, min(127, num - 1))
    except ValueError:
        return 0


def build_zones_rev2trap(
    out_dir: Path,
    instrument_id: str,
    slot: dict,
    binary: Path | None = None,
) -> dict:
    render_bin = find_binary(binary)
    program = program_index_from_slot(slot)
    roots = default_multisample_roots()
    zones = []
    for root in roots:
        lo, hi = max(0, root - 1), min(127, root + 1)
        fname = f"{instrument_id}_root{root}.wav"
        rel = f"samples/{fname}"
        wav = out_dir / rel
        wav.parent.mkdir(parents=True, exist_ok=True)
        subprocess.run(
            [
                str(render_bin),
                str(wav),
                str(program),
                str(root),
                "110",
                "2.8",
                str(SAMPLE_RATE),
            ],
            check=True,
        )
        qa = analyze_wav(wav)
        if not qa.pass_qa:
            raise RuntimeError(f"QA failed {wav}: {qa.reasons}")
        zones.append(
            {
                "samplePath": rel,
                "rootKey": root,
                "loKey": lo,
                "hiKey": hi,
                "loVel": 1,
                "hiVel": 127,
                "volumeDb": 0.0,
            }
        )
    return {
        "format": "DISKLORDZ_INSTRUMENT_MAP",
        "version": 1,
        "instrumentId": instrument_id,
        "name": slot.get("name", instrument_id),
        "productId": slot.get("productId", "REV2-TRAP-128"),
        "sampleRate": SAMPLE_RATE,
        "engine": "Rev2TrapOfflineRender",
        "program": program,
        "zones": zones,
        "metadata": {"provenance": "rev2_trap_vsti_v1"},
    }
