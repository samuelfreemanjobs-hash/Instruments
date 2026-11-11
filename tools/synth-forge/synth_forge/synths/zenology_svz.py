"""Roland ZEN-Core .svz template mutation (seed-based)."""

from __future__ import annotations

import io
import zipfile
from pathlib import Path

from synth_forge.bin_diff import OffsetField, OffsetMap, load_offset_map, mask_unmapped_regions
from synth_forge.models import SynthParameters
from synth_forge.synths.util import embed_params_block

_DATA_DIR = Path(__file__).resolve().parent.parent / "data" / "zenology"
TONE_BLOCK_SIZE = 0x660
DEFAULT_OFFSETS = _DATA_DIR / "offsets.json"
NAME_MAX_LEN = 16


def _default_offset_map() -> OffsetMap:
    return OffsetMap(
        version=1,
        container="zen_core_tone",
        fields=[
            OffsetField(
                id="params_block",
                offset=96,
                scale="params_block",
                length=88,
                note="SynthForge staging block inside tone",
            ),
            OffsetField(
                id="patch_name",
                offset=0,
                scale="u8",
                note="tone name start",
            ),
        ],
    )


class ZenologySvzEngine:
    def __init__(self, offsets_path: Path | None = None) -> None:
        if offsets_path and offsets_path.is_file():
            self.offsets = load_offset_map(offsets_path)
        elif DEFAULT_OFFSETS.is_file():
            self.offsets = load_offset_map(DEFAULT_OFFSETS)
        else:
            self.offsets = _default_offset_map()

    def make_test_seed(self) -> bytes:
        tone = bytearray(TONE_BLOCK_SIZE)
        embed_params_block(tone, 96, SynthParameters(category="seed"))
        return bytes(b"SVZ\x00") + bytes(tone)

    def locate_tone(self, data: bytes) -> tuple[int, int]:
        if data[:4] == b"SVZ\x00":
            start = 4
            end = min(len(data), start + TONE_BLOCK_SIZE)
            return start, end
        if data[:2] == b"PK":
            with zipfile.ZipFile(io.BytesIO(data), "r") as zf:
                for name in zf.namelist():
                    if name.lower().endswith((".svz", ".bin")):
                        return self.locate_tone(zf.read(name))
            raise ValueError("no tone container in zip seed")
        if len(data) >= TONE_BLOCK_SIZE:
            return 0, TONE_BLOCK_SIZE
        raise ValueError("unrecognized .svz seed")

    def _write_name(self, tone: bytearray, name: str) -> None:
        field = self.offsets.by_id("patch_name")
        if field is None:
            return
        raw = name.encode("ascii", errors="replace")[:NAME_MAX_LEN]
        for i, byte in enumerate(raw):
            idx = field.offset + i
            if idx < len(tone):
                tone[idx] = byte

    def apply_parameters_to_container(self, container: bytearray, params: SynthParameters) -> None:
        start, end = self.locate_tone(container)
        tone = container[start:end]
        if len(tone) < TONE_BLOCK_SIZE:
            raise ValueError("tone block too short")
        for field in self.offsets.fields:
            if field.id == "patch_name":
                continue
            if field.scale == "params_block":
                embed_params_block(tone, field.offset, params)
            elif hasattr(params, field.id):
                from synth_forge.bin_diff import write_scaled_field

                write_scaled_field(tone, field, float(getattr(params, field.id)))
        container[start:end] = tone

    def pack_svz(self, params: SynthParameters, name: str, *, seed: bytes | None = None) -> bytes:
        container = bytearray(seed if seed is not None else self.make_test_seed())
        self.apply_parameters_to_container(container, params)
        start, end = self.locate_tone(container)
        tone = bytearray(container[start:end])
        self._write_name(tone, name)
        container[start:end] = tone
        return bytes(container)

    def pack_svz_bank(self, presets: list[tuple[SynthParameters, str]], *, seed: bytes | None = None) -> bytes:
        buf = io.BytesIO()
        with zipfile.ZipFile(buf, "w", compression=zipfile.ZIP_DEFLATED) as zf:
            for idx, (params, name) in enumerate(presets, start=1):
                blob = self.pack_svz(params, name, seed=seed)
                zf.writestr(f"FRG_{idx:03d}_{name[:24]}.svz", blob)
        return buf.getvalue()

    def unpack_parameters(self, data: bytes) -> SynthParameters:
        start, _end = self.locate_tone(data)
        tone = data[start : start + TONE_BLOCK_SIZE]
        block = self.offsets.by_id("params_block")
        if block is None:
            raise ValueError("params_block missing from offset map")
        from synth_forge.synths.util import payload_to_params

        length = block.length or 88
        return payload_to_params(tone[block.offset : block.offset + length])

    def masked_fingerprint(self, data: bytes) -> bytes:
        start, end = self.locate_tone(data)
        tone = data[start:end]
        return mask_unmapped_regions(tone, self.offsets)

    @staticmethod
    def default_fixtures_dir() -> Path:
        return Path(__file__).resolve().parents[2] / "fixtures" / "zenology"
