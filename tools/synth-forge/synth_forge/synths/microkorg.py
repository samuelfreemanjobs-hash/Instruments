from __future__ import annotations

from synth_forge.models import SynthParameters
from synth_forge.safety import clamp_for_hardware
from synth_forge.synths.base import SynthAdapter
from synth_forge.synths.util import embed_params_block, payload_to_params


class MicroKorgAdapter(SynthAdapter):
    synth_id = "microkorg"
    display_name = "Korg microKORG"
    PRG_SIZE = 512
    PARAM_OFFSET = 40

    def pack(self, params: SynthParameters) -> bytes:
        safe = clamp_for_hardware(params)
        blob = bytearray(b"PRG\x01" + bytes(self.PRG_SIZE - 4))
        embed_params_block(blob, self.PARAM_OFFSET, safe)
        return bytes(blob)

    def unpack(self, data: bytes) -> SynthParameters:
        if len(data) != self.PRG_SIZE or data[:3] != b"PRG":
            raise ValueError("invalid microKORG program dump")
        payload = data[self.PARAM_OFFSET : self.PARAM_OFFSET + 88]
        return clamp_for_hardware(payload_to_params(payload))

    def pack_sysex(self, params: SynthParameters) -> bytes:
        body = self.pack(params)
        return bytes([0xF0, 0x42, 0x30, 0x58, 0x40]) + body[:100] + bytes([0xF7])
