# DDSP / Trap-Phonk drum generator — AI agent engineering prompts

Use with **`ddsp-ml-engineer`**, **`audio-plugin-coder`**, and **`code-project-planner`** agents. Read [`juce_onnx_pipeline_guide.txt`](../tools/drum-synth-blueprint/docs/juce_onnx_pipeline_guide.txt) for the full pipeline diagram.

## Planner agent (PRD excerpt)

```markdown
## Goal
ML-backed Trap/Phonk 808 generator: acoustic hit in → mel-matched synthetic 808 out → optional ONNX params in JUCE.

## Success criteria
- [ ] `pytest tools/drum-synth-blueprint/tests -v` green
- [ ] `feature_difference_loss` documented and used for param search
- [ ] JUCE doc: audio thread never calls ONNX Run
- [ ] Draft PR + golden WAV policy if shipping plugin DSP
```

## DDSP / ML engineer agent

```markdown
Implement or tune the Python reference before C++/ONNX:

1. Read `drum_synth_blueprint/synth_808_generator.py` — exponential pitch 350→45 Hz, tanh drive.
2. Fit [pitch_decay, amp_decay, drive] minimizing `mel_loss.loss_for_synth_params`.
3. Distill labels into `encoder_stub.infer_params_numpy` or train `build_torch_encoder`.
4. Export encoder ONNX only; keep oscillator in C++ per juce_onnx_pipeline_guide.txt.
5. Attach pytest log + example loss before/after tuning [executed].
```

## Audio Plugin Coder (JUCE) agent

```markdown
Port `synthesize_808` to `Source/DSP/Synth808.h` (inline, no alloc in processBlock).

ONNX Runtime on background thread only; atomic Synth808Params on audio thread.

Follow docs/JUCE_APC_AGENT_BOOTSTRAP.md and run `run_business.py --profile ci` after C++ edits.

Reference: tools/drum-synth-blueprint/docs/juce_onnx_pipeline_guide.txt
```

## Cursor Cloud structured task (copy-paste)

```markdown
## Goal
Wire mel feature-loss 808 blueprint into training smoke + document ONNX→JUCE threading.

## Context
- Read: docs/COMPANY_MEMORY_INDEX.md, tools/drum-synth-blueprint/ARCHITECTURE.md
- Agents: ddsp-ml-engineer, audio-plugin-coder

## Requirements
1. Keep synth_808_generator.py as pure math core (exp pitch + tanh).
2. mel_loss.py for feature difference loss (not waveform MSE).
3. juce_onnx_pipeline_guide.txt accurate for async ONNX.

## Success criteria
- [ ] pytest drum-synth-blueprint
- [ ] python -m drum_synth_blueprint.synth_808_generator prints ok
```
