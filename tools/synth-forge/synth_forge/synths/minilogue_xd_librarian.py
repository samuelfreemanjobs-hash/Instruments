"""Korg Minilogue XD .mnlgxdprog (ZIP) librarian packer."""

from __future__ import annotations

import io
import json
import zipfile
from pathlib import Path

from synth_forge.bin_diff import load_offset_map, write_scaled_field
from synth_forge.models import SynthParameters
from synth_forge.synths.minilogue_xd import MinilogueXdAdapter
from synth_forge.synths.util import embed_params_block

_DATA_DIR = Path(__file__).resolve().parent.parent / "data" / "minilogue_xd"
PROG_BIN_NAME = "Prog_000.prog_bin"
INFO_XML_NAME = "prog_info.xml"
DEFAULT_OFFSETS = _DATA_DIR / "offsets.json"


def midi_cc_map() -> dict:
    path = _DATA_DIR / "midi_cc_map.json"
    return json.loads(path.read_text(encoding="utf-8"))


def _prog_info_xml(name: str) -> str:
    safe = name.replace("&", "&amp;").replace("<", "&lt;")[:32]
    return (
        '<?xml version="1.0" encoding="UTF-8"?>\n'
        f"<prog_info><name>{safe}</name></prog_info>\n"
    )


class MinilogueXdLibrarian:
    def __init__(self, offsets_path: Path | None = None) -> None:
        self.offsets = load_offset_map(offsets_path or DEFAULT_OFFSETS)
        self.adapter = MinilogueXdAdapter()

    def extract_prog_bin(self, mnlgxdprog: bytes) -> bytes:
        with zipfile.ZipFile(io.BytesIO(mnlgxdprog), "r") as zf:
            if PROG_BIN_NAME not in zf.namelist():
                raise ValueError(f"missing {PROG_BIN_NAME} in archive")
            return zf.read(PROG_BIN_NAME)

    def pack_mnlgxdprog(self, params: SynthParameters, name: str, *, seed_prog: bytes | None = None) -> bytes:
        prog = bytearray(seed_prog if seed_prog is not None else self.adapter.pack(params))
        if seed_prog is not None:
            self.apply_parameters(prog, params)
        else:
            block = self.offsets.by_id("params_block")
            if block is not None:
                embed_params_block(prog, block.offset, params)
        self._write_patch_name(prog, name)
        buf = io.BytesIO()
        with zipfile.ZipFile(buf, "w", compression=zipfile.ZIP_STORED) as zf:
            zf.writestr(PROG_BIN_NAME, bytes(prog))
            zf.writestr(INFO_XML_NAME, _prog_info_xml(name))
        return buf.getvalue()

    def apply_parameters(self, prog_bin: bytearray, params: SynthParameters) -> None:
        for field in self.offsets.fields:
            if field.id == "patch_name":
                continue
            if field.scale == "params_block":
                embed_params_block(prog_bin, field.offset, params)
                continue
            if not hasattr(params, field.id):
                continue
            write_scaled_field(prog_bin, field, float(getattr(params, field.id)))

    def _write_patch_name(self, prog_bin: bytearray, name: str) -> None:
        field = self.offsets.by_id("patch_name")
        if field is None:
            return
        raw = name.encode("ascii", errors="replace")[:16]
        for i, byte in enumerate(raw):
            idx = field.offset + i
            if idx < len(prog_bin):
                prog_bin[idx] = byte

    def unpack_parameters(self, mnlgxdprog: bytes) -> SynthParameters:
        prog = self.extract_prog_bin(mnlgxdprog)
        return self.adapter.unpack(prog)

    def load_seed_file(self, path: Path) -> bytes:
        data = path.read_bytes()
        if path.suffix.lower() in {".zip", ".mnlgxdprog"} or data[:2] == b"PK":
            return self.extract_prog_bin(data)
        if len(data) == MinilogueXdAdapter.PROG_SIZE:
            return data
        raise ValueError("seed must be .mnlgxdprog zip or 1024-byte prog bin")


def default_fixtures_dir() -> Path:
    return Path(__file__).resolve().parents[2] / "fixtures" / "minilogue_xd"
