---
name: disklordz-audio-plugin-coder
description: JUCE/VST3 via Noizefield APC phases mapped to monorepo CMake plugins.
when_to_use: New VST3, JUCE processor/editor, APC dream→ship, or plugin CI failures.
disable-model-invocation: false
context: fork
paths:
  - "Source/**"
  - "Wave9090/**"
  - "disklordz/trap-forge/plugin/**"
  - "docs/JUCE_APC_AGENT_BOOTSTRAP.md"
  - "CMakeLists.txt"
  - ".github/workflows/build.yml"
  - "vst-testing-ops/**"
  - "disklordz/agents/profit/audio-plugin-coder/**"
---

# Skill — Audio Plugin Coder (APC)

## Prerequisites

- Read [`docs/JUCE_APC_AGENT_BOOTSTRAP.md`](../../../docs/JUCE_APC_AGENT_BOOTSTRAP.md) and product `ARCHITECTURE.md`
- PRD from **code-project-planner** for greenfield targets

```bash
python3 disklordz/integrations/agents/audio_plugin_coder_agent.py --self-test
cmake --build build -j --target JDUpgraded_VST3
python3 vst-testing-ops/run_business.py --profile ci
```

## Success criteria

- [ ] Plugin CI green or scoped target builds
- [ ] Realtime rules documented in ARCHITECTURE.md
- [ ] No duplicate JUCE trees without PRD note
