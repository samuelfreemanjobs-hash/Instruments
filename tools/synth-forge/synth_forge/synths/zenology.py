"""Roland Zenology adapter — .svz librarian via ZenologySvzEngine (verify in ZENOLOGY Pro)."""

from __future__ import annotations

from synth_forge.models import SynthParameters
from synth_forge.safety import clamp_for_hardware
from synth_forge.synths.base import SynthAdapter
from synth_forge.synths.zenology_svz import ZenologySvzEngine


class ZenologyAdapter(SynthAdapter):
    synth_id = "zenology"
    display_name = "Roland Zenology"
    stub = True
    librarian_svz = True

    def __init__(self) -> None:
        self._engine = ZenologySvzEngine()

    def pack(self, params: SynthParameters) -> bytes:
        safe = clamp_for_hardware(params)
        return self._engine.pack_svz(safe, safe.category[:16] or "FRG Patch")

    def unpack(self, data: bytes) -> SynthParameters:
        return clamp_for_hardware(self._engine.unpack_parameters(data))

    def pack_bank(self, presets: list[tuple[SynthParameters, str]]) -> bytes:
        return self._engine.pack_svz_bank(presets)
