import pytest

from synth_forge.models import SynthParameters
from synth_forge.synths import ADAPTERS


@pytest.mark.parametrize("synth_id", list(ADAPTERS.keys()))
def test_adapter_roundtrip(synth_id: str):
    adapter = ADAPTERS[synth_id]
    original = SynthParameters(
        filter_resonance=0.99,
        delay_feedback=0.9,
        master_volume=0.95,
        category="roundtrip",
    )
    safe = original.model_copy()
    from synth_forge.safety import clamp_for_hardware

    original = clamp_for_hardware(original)
    restored = adapter.roundtrip(original)
    for name in original.model_fields:
        if name == "category":
            assert restored.category == original.category
        else:
            assert getattr(restored, name) == pytest.approx(getattr(original, name), abs=1e-5)


def test_minilogue_sysex_framing():
    adapter = ADAPTERS["minilogue_xd"]
    sysex = adapter.pack_sysex(SynthParameters())
    assert sysex[0] == 0xF0 and sysex[-1] == 0xF7
    assert sysex[1:6] == bytes([0x42, 0x30, 0x00, 0x01, 0x51])


def test_dx7_voice_size():
    adapter = ADAPTERS["dx7"]
    blob = adapter.pack(SynthParameters())
    assert len(blob) == 128
