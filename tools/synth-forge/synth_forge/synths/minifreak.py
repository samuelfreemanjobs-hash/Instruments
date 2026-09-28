"""Arturia MiniFreak adapter (stub — .mnfk container layout reserved)."""

from __future__ import annotations

from synth_forge.models import SynthParameters
from synth_forge.safety import clamp_for_hardware
from synth_forge.synths.base import SynthAdapter
from synth_forge.synths.util import embed_params_block, payload_to_params


class MiniFreakAdapter(SynthAdapter):
    synth_id = "minifreak"
    display_name = "Arturia MiniFreak"
    stub = True
    MNFK_SIZE = 2048
    PARAM_OFFSET = 128

    def pack(self, params: SynthParameters) -> bytes:
        safe = clamp_for_hardware(params)
        blob = bytearray(b"MNFK\x01\x00" + bytes(self.MNFK_SIZE - 6))
        embed_params_block(blob, self.PARAM_OFFSET, safe)
        return bytes(blob)

    def unpack(self, data: bytes) -> SynthParameters:
        if len(data) != self.MNFK_SIZE or data[:4] != b"MNFK":
            raise ValueError("invalid MiniFreak .mnfk stub container")
        payload = data[self.PARAM_OFFSET : self.PARAM_OFFSET + 88]
        return clamp_for_hardware(payload_to_params(payload))
