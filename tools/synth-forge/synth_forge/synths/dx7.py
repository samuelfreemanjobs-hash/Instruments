from __future__ import annotations

import struct

from synth_forge.models import SynthParameters
from synth_forge.safety import clamp_for_hardware
from synth_forge.synths.base import SynthAdapter
from synth_forge.synths.util import PARAM_FLOATS, payload_to_params, params_to_payload


class Dx7Adapter(SynthAdapter):
    synth_id = "dx7"
    display_name = "Yamaha DX7 / Volca FM2"
    VOICE_SIZE = 128
    BANK_SIZE = 4096
    PARAM_OFFSET = 16

    def pack_voice(self, params: SynthParameters) -> bytes:
        safe = clamp_for_hardware(params)
        blob = bytearray(bytes([0xF0, 0x43, 0x00, 0x00, 0x01, 0x1B]) + bytes(self.VOICE_SIZE - 6))
        payload = params_to_payload(safe)[: self.VOICE_SIZE - self.PARAM_OFFSET - 1]
        blob[self.PARAM_OFFSET : self.PARAM_OFFSET + len(payload)] = payload
        blob[-1] = 0xF7
        return bytes(blob)

    def pack(self, params: SynthParameters) -> bytes:
        return self.pack_voice(params)

    def unpack(self, data: bytes) -> SynthParameters:
        if len(data) not in (self.VOICE_SIZE, self.BANK_SIZE):
            raise ValueError("invalid DX7 voice/bank size")
        if len(data) == self.BANK_SIZE:
            data = data[: self.VOICE_SIZE]
        if data[0] != 0xF0:
            raise ValueError("invalid DX7 SysEx header")
        payload = data[self.PARAM_OFFSET : self.PARAM_OFFSET + 88]
        return clamp_for_hardware(payload_to_params(payload))

    def pack_bank(self, voices: list[SynthParameters]) -> bytes:
        if len(voices) != 32:
            raise ValueError("DX7 bank requires exactly 32 voices")
        chunks = [self.pack_voice(v)[self.PARAM_OFFSET : self.VOICE_SIZE - 1] for v in voices]
        flat = b"".join(chunks)
        if len(flat) != self.BANK_SIZE - 128:
            pad = self.BANK_SIZE - 128 - len(flat)
            flat += bytes(pad)
        header = struct.pack(">4sI", b"DX7B", 32)
        return header + flat[: self.BANK_SIZE - len(header)]
