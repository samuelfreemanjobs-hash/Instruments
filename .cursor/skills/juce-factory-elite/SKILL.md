---
name: juce-factory-elite
description: Implement and ship JUCE plugin C++ (JD Upgraded, Wave909, tools) with CI gate. Use when editing Source/, Wave909/, tools/*.cpp, CMakeLists, golden WAVs, or vst-testing-ops.
---

# Skill: JUCE factory elite

Read [docs/JUCE_FACTORY_AGENT.md](../../../docs/JUCE_FACTORY_AGENT.md) and apply [.cursor/rules/juce-factory-elite.mdc](../../rules/juce-factory-elite.mdc).

## Workflow

1. `/ARCHITECTURE.md` → `docs/ARCHITECTURE.md` or `Wave909/ARCHITECTURE.md`
2. Minimal diff; processor before editor
3. **No** audio-thread alloc/lock
4. Verify:

```bash
cmake --build build -j
python3 vst-testing-ops/run_business.py --profile ci
```

5. Draft PR on `cursor/<feature>-62b1`

## Subagents

| Situation | Subagent |
|-----------|----------|
| Find code across repo | `explore` (medium/thorough) |
| Reproducible native bug | `debug` |
| Plugin UI manual test | `computerUse` |
| CI failure on PR | `ci-investigator` |

## Cloud prompt template

[docs/prompts/juce-factory-worker.cloud.md](../../../docs/prompts/juce-factory-worker.cloud.md)
