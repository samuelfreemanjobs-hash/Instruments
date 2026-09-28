# Drum synth blueprint (NumPy + DDSP reference)

## Purpose

Reference **vectorized** Trap kick and **phonk 808** curves plus **mel feature-loss** training targets for the **ddsp-ml-engineer** agent and future JUCE/ONNX ports.

## Build & run

```bash
cd tools/drum-synth-blueprint
pip install -r requirements.txt
pip install -e .
pytest tests -v
python -m drum_synth_blueprint.synth_808_generator
```

## Pure math core

| Module | Role |
|--------|------|
| `synth_808_generator.py` | Exp pitch **350→45 Hz**, integrated phase, **np.tanh** drive |
| `trap_kick_808.py` | Layered trap kick + glide 808 (legacy API) |
| `mel_loss.py` | Log-mel **feature difference loss** (phase-robust) |
| `encoder_stub.py` | Sample → `[pitch_decay, amp_decay, drive]` (numpy; optional torch MLP) |

## DDSP → ONNX → JUCE

See [`docs/juce_onnx_pipeline_guide.txt`](docs/juce_onnx_pipeline_guide.txt).

Agent prompts: [`docs/DDSP_TRAP_PHONK_AGENT_PROMPTS.md`](../../docs/DDSP_TRAP_PHONK_AGENT_PROMPTS.md).

## Data flow

```text
acoustic WAV → encoder_stub → synth params
            → synthesize_808 → mel_loss vs target
            → (export) ONNX encoder + C++ oscillator in JUCE
```

## Related

- [tools/serum-forge/docs/SERUM_TEXT_TO_PRESET_PROMPTS.md](../serum-forge/docs/SERUM_TEXT_TO_PRESET_PROMPTS.md)
- [docs/JUCE_APC_AGENT_BOOTSTRAP.md](../../docs/JUCE_APC_AGENT_BOOTSTRAP.md)
