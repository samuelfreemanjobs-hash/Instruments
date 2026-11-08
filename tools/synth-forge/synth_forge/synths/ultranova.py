from __future__ import annotations

from synth_forge.models import SynthParameters
from synth_forge.safety import clamp_for_hardware
from synth_forge.synths.base import SynthAdapter
from synth_forge.synths.util import embed_params_block, payload_to_params


class UltraNovaAdapter(SynthAdapter):
    synth_id = "ultranova"
    display_name = "Novation UltraNova"
    DUMP_SIZE = 512
    PARAM_OFFSET = 32

    def pack(self, params: SynthParameters) -> bytes:
        safe = clamp_for_hardware(params)
        blob = bytearray(bytes([0xF0, 0x00, 0x20, 0x29, 0x71]) + bytes(self.DUMP_SIZE - 5))
        embed_params_block(blob, self.PARAM_OFFSET, safe)
        blob[-1] = 0xF7
        return bytes(blob)

    def unpack(self, data: bytes) -> SynthParameters:
        if len(data) != self.DUMP_SIZE or data[0] != 0xF0:
            raise ValueError("invalid UltraNova SysEx dump")
        payload = data[self.PARAM_OFFSET : self.PARAM_OFFSET + 88]
        return clamp_for_hardware(payload_to_params(payload))
