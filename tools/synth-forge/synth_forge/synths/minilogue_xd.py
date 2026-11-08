from __future__ import annotations

from synth_forge.models import SynthParameters
from synth_forge.safety import clamp_for_hardware
from synth_forge.synths.base import SynthAdapter
from synth_forge.synths.util import crc16_ccitt, embed_params_block, payload_to_params


class MinilogueXdAdapter(SynthAdapter):
    synth_id = "minilogue_xd"
    display_name = "Korg Minilogue XD"
    PROG_SIZE = 1024
    PARAM_OFFSET = 64

    def pack(self, params: SynthParameters) -> bytes:
        safe = clamp_for_hardware(params)
        blob = bytearray(b"PROG") + bytes(self.PROG_SIZE - 4)
        embed_params_block(blob, self.PARAM_OFFSET, safe)
        crc_data = bytes(blob[: self.PROG_SIZE - 2])
        crc = crc16_ccitt(crc_data)
        blob[self.PROG_SIZE - 2] = (crc >> 8) & 0xFF
        blob[self.PROG_SIZE - 1] = crc & 0xFF
        return bytes(blob)

    def unpack(self, data: bytes) -> SynthParameters:
        if len(data) != self.PROG_SIZE or data[:4] != b"PROG":
            raise ValueError("invalid Minilogue XD program size or header")
        payload = data[self.PARAM_OFFSET : self.PARAM_OFFSET + 88]
        return clamp_for_hardware(payload_to_params(payload))

    def pack_sysex(self, params: SynthParameters) -> bytes:
        body = self.pack(params)
        return bytes([0xF0, 0x42, 0x30, 0x00, 0x01, 0x51]) + body + bytes([0xF7])
