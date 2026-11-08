from __future__ import annotations

from synth_forge.synths.base import SynthAdapter
from synth_forge.synths.dx7 import Dx7Adapter
from synth_forge.synths.micro_x import MicroXAdapter
from synth_forge.synths.microkorg import MicroKorgAdapter
from synth_forge.synths.minifreak import MiniFreakAdapter
from synth_forge.synths.minilogue_xd import MinilogueXdAdapter
from synth_forge.synths.ultranova import UltraNovaAdapter
from synth_forge.synths.zenology import ZenologyAdapter

ADAPTERS: dict[str, SynthAdapter] = {
    MinilogueXdAdapter().synth_id: MinilogueXdAdapter(),
    UltraNovaAdapter().synth_id: UltraNovaAdapter(),
    MicroXAdapter().synth_id: MicroXAdapter(),
    MicroKorgAdapter().synth_id: MicroKorgAdapter(),
    Dx7Adapter().synth_id: Dx7Adapter(),
    MiniFreakAdapter().synth_id: MiniFreakAdapter(),
    ZenologyAdapter().synth_id: ZenologyAdapter(),
}


def get_adapter(synth_id: str) -> SynthAdapter:
    adapter = ADAPTERS.get(synth_id)
    if adapter is None:
        raise KeyError(f"unknown synth_id: {synth_id}")
    return adapter


def list_synths() -> list[dict[str, object]]:
    return [
        {
            "id": a.synth_id,
            "name": a.display_name,
            "stub": a.stub,
        }
        for a in ADAPTERS.values()
    ]
