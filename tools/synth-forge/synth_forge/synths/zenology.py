"""Roland Zenology adapter (stub — .sve patch envelope reserved)."""

from __future__ import annotations

from synth_forge.models import SynthParameters
from synth_forge.safety import clamp_for_hardware
from synth_forge.synths.base import SynthAdapter
from synth_forge.synths.util import embed_params_block, payload_to_params


class ZenologyAdapter(SynthAdapter):
    synth_id = "zenology"
    display_name = "Roland Zenology"
    stub = True
    SVE_SIZE = 1536
    PARAM_OFFSET = 96

    def pack(self, params: SynthParameters) -> bytes:
        safe = clamp_for_hardware(params)
        blob = bytearray(b"SVE\x00" + bytes(self.SVE_SIZE - 4))
        embed_params_block(blob, self.PARAM_OFFSET, safe)
        return bytes(blob)

    def unpack(self, data: bytes) -> SynthParameters:
        if len(data) != self.SVE_SIZE or data[:3] != b"SVE":
            raise ValueError("invalid Zenology .sve stub container")
        payload = data[self.PARAM_OFFSET : self.PARAM_OFFSET + 88]
        return clamp_for_hardware(payload_to_params(payload))
