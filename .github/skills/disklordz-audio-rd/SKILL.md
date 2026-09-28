---
name: disklordz-audio-rd
description: Experiment design for ML drums; partners with ddsp-ml-engineer and audio-plugin-coder (JUCE/APC).
when_to_use: Spikes, ablations, promote/kill decisions, RD log entries before production DSP.
disable-model-invocation: false
context: fork
paths:
  - "docs/RD_EXPERIMENT_LOG.md"
  - "docs/DDSP_TRAP_PHONK_AGENT_PROMPTS.md"
  - "tools/drum-synth-blueprint/**"
  - "disklordz/agents/profit/audio-rd/**"
  - "disklordz/agents/profit/ddsp-ml-engineer/**"
  - "disklordz/agents/profit/audio-plugin-coder/**"
---

# Skill — Audio R&D

## Role split

| Agent | Owns |
|-------|------|
| **audio-rd** (you) | Hypothesis, metrics, ablation plan, `RD_EXPERIMENT_LOG.md` |
| **ddsp-ml-engineer** | PyTorch/NumPy training, mel loss, ONNX export prep |
| **audio-plugin-coder** | JUCE C++, APC phases, realtime ONNX threading |

## Prerequisites

```bash
python3 disklordz/integrations/agents/audio_rd_agent.py --self-test
python3 disklordz/integrations/agents/ddsp_ml_engineer.py --self-test
python3 disklordz/integrations/agents/audio_plugin_coder_agent.py --self-test
```

## Success criteria

- [ ] New row or section in `docs/RD_EXPERIMENT_LOG.md` with promote/kill/park
- [ ] Handoff explicitly names ddsp-ml-engineer and/or audio-plugin-coder
- [ ] No ship claims — R&D does not merge plugin PRs
