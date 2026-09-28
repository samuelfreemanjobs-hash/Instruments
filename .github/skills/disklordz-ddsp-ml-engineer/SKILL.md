---
name: disklordz-ddsp-ml-engineer
description: PyTorch/DDSP drum synthesis, mel loss, ONNX export prep.
when_to_use: Train/fit 808 params, mel_loss, encoder_stub, blueprint code changes.
disable-model-invocation: false
context: fork
paths:
  - "tools/drum-synth-blueprint/**"
  - "tools/serum-forge/serum_forge/perceptual.py"
  - "docs/DDSP_TRAP_PHONK_AGENT_PROMPTS.md"
  - "docs/RD_EXPERIMENT_LOG.md"
  - "disklordz/agents/profit/ddsp-ml-engineer/**"
  - ".github/workflows/drum-synth-blueprint.yml"
---

# Skill — DDSP / ML Engineer

Partner: **audio-rd** (experiment plan) · **audio-plugin-coder** (JUCE ship)

```bash
python3 disklordz/integrations/agents/ddsp_ml_engineer.py --self-test
```
