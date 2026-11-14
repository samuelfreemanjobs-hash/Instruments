import pytest

from synth_forge.models import SynthParameters
from synth_forge.synths.zenology import ZenologyAdapter
from synth_forge.synths.zenology_svz import ZenologySvzEngine
from synth_forge.zen_forge_presets import apply_zen_profile, procedural_name


def test_svz_pack_unpack_roundtrip():
    engine = ZenologySvzEngine()
    params = apply_zen_profile("memphis_lead", __import__("random").Random(3))
    blob = engine.pack_svz(params, procedural_name("memphis_lead", 1))
    assert blob[:4] == b"SVZ\x00"
    restored = engine.unpack_parameters(blob)
    assert restored.filter_cutoff == pytest.approx(params.filter_cutoff, abs=1e-4)


def test_svz_bank_zip():
    engine = ZenologySvzEngine()
    presets = [
        (apply_zen_profile("sub_808"), procedural_name("sub_808", 1)),
        (apply_zen_profile("rnb_pad"), procedural_name("rnb_pad", 2)),
    ]
    bank = engine.pack_svz_bank(presets)
    assert bank[:2] == b"PK"


def test_zenology_adapter_pack():
    adapter = ZenologyAdapter()
    blob = adapter.pack(SynthParameters(category="test"))
    assert blob[:4] == b"SVZ\x00"
    assert adapter.librarian_svz is True
