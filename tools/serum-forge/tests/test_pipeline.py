from serum_forge.guardrails import clamp_patch
from serum_forge.models import PatchArchetype, SerumSymbolPatch
from serum_forge.pipeline import run_pipeline


def test_clamp_reduces_hot_levels():
    p = clamp_patch(
        SerumSymbolPatch(
            name="Hot",
            archetype=PatchArchetype.bass_sub,
            osc_a_level=0.95,
            osc_a_unison_voices=8,
        )
    )
    assert p.osc_a_level < 0.95
    assert p.fx_reverb_wet <= 0.4


def test_run_pipeline_writes_json(tmp_path):
    result = run_pipeline("lofi pad", out_dir=tmp_path, archetype=PatchArchetype.pad_ambient)
    assert result.intermediate_json and result.intermediate_json.is_file()
