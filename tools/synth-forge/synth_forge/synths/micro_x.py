from __future__ import annotations

from synth_forge.models import SynthParameters
from synth_forge.safety import clamp_for_hardware
from synth_forge.synths.base import SynthAdapter
from synth_forge.synths.util import embed_params_block, payload_to_params


class MicroXAdapter(SynthAdapter):
    synth_id = "micro_x"
    display_name = "Korg microX"
    MEP_SIZE = 768
    PARAM_OFFSET = 48

    def pack(self, params: SynthParameters) -> bytes:
        safe = clamp_for_hardware(params)
        blob = bytearray(b"MEP\x00" + bytes(self.MEP_SIZE - 4))
        embed_params_block(blob, self.PARAM_OFFSET, safe)
        return bytes(blob)

    def unpack(self, data: bytes) -> SynthParameters:
        if len(data) != self.MEP_SIZE or data[:3] != b"MEP":
            raise ValueError("invalid microX .mep container")
        payload = data[self.PARAM_OFFSET : self.PARAM_OFFSET + 88]
        return clamp_for_hardware(payload_to_params(payload))

    def pack_sysex(self, params: SynthParameters) -> bytes:
        body = self.pack(params)[4:]
        return bytes([0xF0, 0x42, 0x30, 0x79]) + body[:120] + bytes([0xF7])
