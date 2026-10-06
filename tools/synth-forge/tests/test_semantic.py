from synth_forge.semantic_engine import generate_from_prompt, infer_category, prompt_to_parameters
from synth_forge.models import PromptGenerateRequest
from synth_forge.safety import MAX_FILTER_RESONANCE


def test_infer_cardo():
    assert infer_category("Cardo luxury cruising pad") == "luxury_west_coast"


def test_infer_bounce():
    assert infer_category("Mannie Fresh bounce brass") == "bounce_brass"


def test_prompt_to_parameters_safe():
    params = prompt_to_parameters("aggressive phonk reese scream")
    assert params.filter_resonance <= MAX_FILTER_RESONANCE


def test_generate_from_prompt():
    req = PromptGenerateRequest(prompt="DX7 electric piano warm rhodes", count=3, synth_id="dx7")
    presets = generate_from_prompt(req)
    assert len(presets) == 3
    assert presets[0].synth_id == "dx7"
