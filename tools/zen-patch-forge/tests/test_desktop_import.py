def test_zen_forge_presets_importable():
    from synth_forge.zen_forge_presets import ZEN_PROFILES

    assert "memphis_lead" in ZEN_PROFILES
