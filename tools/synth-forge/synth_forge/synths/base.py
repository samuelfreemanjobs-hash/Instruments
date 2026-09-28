from __future__ import annotations

from abc import ABC, abstractmethod

from synth_forge.models import SynthParameters


class SynthAdapter(ABC):
    synth_id: str
    display_name: str
    stub: bool = False

    @abstractmethod
    def pack(self, params: SynthParameters) -> bytes:
        raise NotImplementedError

    @abstractmethod
    def unpack(self, data: bytes) -> SynthParameters:
        raise NotImplementedError

    def pack_sysex(self, params: SynthParameters) -> bytes:
        """Default: wrap raw pack in generic SysEx envelope."""
        body = self.pack(params)
        return bytes([0xF0, 0x7F, 0x01]) + body + bytes([0xF7])

    def roundtrip(self, params: SynthParameters) -> SynthParameters:
        return self.unpack(self.pack(params))
