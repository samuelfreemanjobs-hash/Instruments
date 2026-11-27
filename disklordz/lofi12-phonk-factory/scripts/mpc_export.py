"""Optional MPC-friendly WAV layout (44.1kHz 16-bit, <=16 char names)."""

from __future__ import annotations

import re
import struct
import wave
import xml.etree.ElementTree as ET
from pathlib import Path


def _sanitize_mpc_name(name: str, *, max_len: int = 16) -> str:
    stem = Path(name).stem.upper()
    stem = re.sub(r"[^A-Z0-9_]", "", stem.replace("-", "_"))
    return stem[:max_len] or "SND"


def _sanitize_mpc2000_name(name: str) -> str:
    """DOS 8.3 style: eight-character stem + .WAV extension."""
    stem = Path(name).stem.upper()
    stem = re.sub(r"[^A-Z0-9]", "", stem.replace("-", "").replace("_", ""))
    return f"{(stem or 'SND')[:8]}.WAV"


def _write_xpm_program(
    pack_dir: Path,
    one_shot_names: list[str],
    *,
    program_name: str = "DL_TRAP",
) -> Path:
    """Minimal MPC pad program mapping kick/snare/clap/hat on pads 1–4."""
    root = ET.Element("Program", version="1.0")
    ET.SubElement(root, "Name").text = program_name[:16]
    pad_slots = one_shot_names[:16]
    for pad in range(16):
        pad_el = ET.SubElement(root, "Pad", number=str(pad + 1))
        if pad < len(pad_slots):
            ET.SubElement(pad_el, "Sample").text = pad_slots[pad]
            ET.SubElement(pad_el, "Tune").text = "0"
            ET.SubElement(pad_el, "Level").text = "100"
    path = pack_dir / "Programs" / f"{program_name[:12]}.xpm"
    path.parent.mkdir(parents=True, exist_ok=True)
    tree = ET.ElementTree(root)
    ET.indent(tree, space="  ")
    tree.write(path, encoding="utf-8", xml_declaration=True)
    return path


def export_mpc_pack(
    kit_dir: Path,
    *,
    dest: Path | None = None,
    mpc2000: bool = False,
    write_xpm: bool = True,
) -> Path:
    out = dest or (kit_dir / "MPC_Export")
    one_shots = out / "ONE_SHOTS"
    loops = out / "LOOPS"
    one_shots.mkdir(parents=True, exist_ok=True)
    loops.mkdir(parents=True, exist_ok=True)

    rename = _sanitize_mpc2000_name if mpc2000 else (lambda n: f"{_sanitize_mpc_name(n)}.WAV")

    exported: list[str] = []
    for wav in sorted((kit_dir / "One-Shots").rglob("*.wav")):
        dest_name = rename(wav.name)
        _copy_pcm(wav, one_shots / dest_name)
        exported.append(dest_name)
    for wav in sorted((kit_dir / "Loops").rglob("*.wav")):
        _copy_pcm(wav, loops / rename(wav.name))

    readme = out / "README_MPC.txt"
    naming = "8.3 DOS filenames" if mpc2000 else "truncated to 16 chars"
    readme.write_text(
        f"MPC pack: 44.1kHz 16-bit mono WAV, {naming}.\n"
        "Synthesized by DiskLordz Drum Factory — no external samples.\n"
        + ("Programs/*.xpm — pad map (first 16 one-shots).\n" if write_xpm else "")
    )
    if write_xpm and exported:
        _write_xpm_program(out, exported)
    return out


def _copy_pcm(src: Path, dest: Path) -> None:
    with wave.open(str(src), "rb") as w:
        rate = w.getframerate()
        frames = w.readframes(w.getnframes())
        width = w.getsampwidth()
        ch = w.getnchannels()
    if ch != 1:
        raise ValueError(f"MPC export expects mono: {src}")
    samples = [struct.unpack("<h", frames[i : i + 2])[0] / 32768.0 for i in range(0, len(frames), 2)]
    dest.parent.mkdir(parents=True, exist_ok=True)
    with wave.open(str(dest), "w") as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(rate)
        w.writeframes(
            b"".join(
                struct.pack("<h", int(max(-1.0, min(1.0, s)) * 32767)) for s in samples
            )
        )
