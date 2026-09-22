"""Render multisample zones via Rev2TrapOfflineRender binary."""

from __future__ import annotations

import subprocess
import tempfile
from pathlib import Path

from factory_qa import analyze_wav, qa_score
from post_render import post_process_wav
from trap_synth import default_multisample_roots
from wav_util import SAMPLE_RATE

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_BINARY = ROOT.parents[1] / "build" / "Rev2Trap" / "Rev2TrapOfflineRender"
JZ400_OFFSET = 128
REV2_MAX = 127
JZ400_MAX = 399


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
    product = slot.get("productId", "REV2-TRAP-128")
    try:
        num = int(sid.split("_")[-1])
    except ValueError:
        num = 1
    if product == "JZ400":
        return JZ400_OFFSET + max(0, min(JZ400_MAX, num - 1))
    return max(0, min(REV2_MAX, num - 1))


def render_note(
    render_bin: Path,
    wav: Path,
    program: int,
    midi_note: int,
    velocity: int = 110,
    seconds: float = 2.8,
) -> None:
    subprocess.run(
        [
            str(render_bin),
            str(wav),
            str(program),
            str(midi_note),
            str(velocity),
            str(seconds),
            str(SAMPLE_RATE),
        ],
        check=True,
    )
    post_process_wav(wav)


def pick_best_program(
    render_bin: Path,
    base_program: int,
    product_id: str,
    best_of: int,
) -> int:
    if best_of <= 1:
        return base_program
    candidates = []
    for n in range(best_of):
        if product_id == "JZ400":
            prog = JZ400_OFFSET + ((base_program - JZ400_OFFSET + n) % (JZ400_MAX + 1))
        else:
            prog = (base_program + n) % (REV2_MAX + 1)
        with tempfile.TemporaryDirectory() as tmp:
            wav = Path(tmp) / "probe.wav"
            render_note(render_bin, wav, prog, 60)
            qa = analyze_wav(wav)
            candidates.append((qa_score(qa), prog, qa.pass_qa))
    passing = [c for c in candidates if c[2]]
    pool = passing if passing else candidates
    pool.sort(key=lambda x: x[0], reverse=True)
    return pool[0][1]


def build_zones_rev2trap(
    out_dir: Path,
    instrument_id: str,
    slot: dict,
    binary: Path | None = None,
    best_of: int = 1,
) -> dict:
    render_bin = find_binary(binary)
    base_program = program_index_from_slot(slot)
    program = pick_best_program(render_bin, base_program, slot.get("productId", ""), best_of)
    roots = default_multisample_roots()
    zones = []
    for root in roots:
        lo, hi = max(0, root - 1), min(127, root + 1)
        fname = f"{instrument_id}_root{root}.wav"
        rel = f"samples/{fname}"
        wav = out_dir / rel
        wav.parent.mkdir(parents=True, exist_ok=True)
        render_note(render_bin, wav, program, root)
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
        "metadata": {"provenance": "rev2_trap_vsti_v2"},
    }
