import pytest

from synth_forge.generator import generate_batch, morph, resolve_archetype
from synth_forge.models import SynthParameters
from synth_forge.safety import MAX_FILTER_RESONANCE, MAX_MASTER_VOLUME


def test_generate_batch_count():
    presets = generate_batch("minilogue_xd", 5, category="phonk_reese", seed=42)
    assert len(presets) == 5
    assert all(p.synth_id == "minilogue_xd" for p in presets)


def test_generated_params_are_safe():
    presets = generate_batch("ultranova", 10, category="hyperpop_lead", seed=1)
    for p in presets:
        assert p.parameters.filter_resonance <= MAX_FILTER_RESONANCE
        assert p.parameters.master_volume <= MAX_MASTER_VOLUME


def test_morph_average():
    a = SynthParameters(filter_cutoff=0.2)
    b = SynthParameters(filter_cutoff=0.8)
    m = morph([a, b])
    assert m.filter_cutoff == pytest.approx(0.5, abs=1e-6)


def test_resolve_archetype_aliases():
    assert resolve_archetype("cardo pad") == "luxury_west_coast"
    assert resolve_archetype("phonk drift") == "phonk_reese"
