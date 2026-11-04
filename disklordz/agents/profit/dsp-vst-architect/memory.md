# Memory — DSP & VST Factory Architect

File-tier persistence (no DB). Layout under repo or `~/.claude/projects/disklordz/memory/`.

| File | Type | Purpose |
|------|------|---------|
| `MEMORY.md` | index | ≤200 lines; pointers only |
| `dsp-vst-architect_learnings.md` | feedback | Validated runbooks |
| `dsp-vst-architect_incidents.md` | project | Dated outages / misconfigs |

## Session index (always load)

| Topic | Path |
|-------|------|
| Agent system prompt | `disklordz/agents/profit/dsp-vst-architect/agent.md` |
| Coordinator skill | `disklordz/agents/profit/dsp-vst-architect/skill.md` |
| **JUCE coding** | `disklordz/agents/profit/dsp-vst-architect/skills/juce-coding.md` |
| **C++ realtime** | `disklordz/agents/profit/dsp-vst-architect/skills/cpp-realtime-coding.md` |
| Loop SOP | `disklordz/agents/profit/dsp-vst-architect/loop.md` |
| JD Upgraded DSP | `docs/ARCHITECTURE.md` |
| Formats (VST3/CLAP) | `docs/INTEGRATION.md` |
| QA pipeline | `vst-testing-ops/ARCHITECTURE.md` |

## Quick constants (Instruments)

- JUCE **8.0.15**, C++ **20**, Linux CI **g++-12**
- Control-rate divisor example: **32** (`JDUPGRADED_CONTROL_RATE_DIVISOR`)
- Golden programs: `tests/golden/manifest.tsv`

## Frontmatter template

```yaml
---
name: DSP & VST Factory Architect incident 2026-11-03
description: One-line for LLM recall
type: feedback
---
```

## Recall

Tier 1: always load MEMORY.md index + companion skill paths for implementation tasks. Tier 2: LLM picks ≤5 files from manifest using query + tool history.

## Do not memorize

Anything derivable from git log, `ARCHITECTURE.md`, or build scripts — retrieve instead.
