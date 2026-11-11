from __future__ import annotations

from vst_factory import modules


def test_catalog_loads() -> None:
    cat = modules.load_catalog()
    assert cat["schema_version"] == "factory-modules/v1"
    assert len(cat["plugin_classes"]) == 4


def test_build_spec_midi() -> None:
    spec = modules.build_spec_from_modules(
        plugin_name="MemphisPads",
        plugin_class="midi",
        prompt="Memphis chord pads with voicing",
    )
    assert spec["schema_version"] == "factory-spec/v2"
    assert spec["track"] == "apc_greenfield"
    assert "midi.chord-pads" in spec["dsp_modules"]


def test_render_creative_brief() -> None:
    spec = modules.build_spec_from_modules(
        plugin_name="TestPlugin",
        plugin_class="synth",
        prompt="Three oscillator synth",
    )
    text = modules.materialize_creative_brief("synth", "TestPlugin", "Three oscillator synth", spec)
    assert "TestPlugin" in text
    assert "Three oscillator synth" in text
