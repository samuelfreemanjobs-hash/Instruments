# Subagent delegation guide

Use when the orchestrator splits work across Task subagents or human-readable phases.

## Handoff contract (every subagent)

**Input packet must include:**

- Plugin name and path (`vst/MyPlugin/` or `src/oscillators/foo/`)
- Target(s): VST only | logue mkI | mkII | multi
- Complexity tier (L0–L3) from assessment skill
- `params.json` or manifest parameter list (if exists)

**Output packet must include:**

- Files changed (paths)
- Program chain order (one line)
- Open risks (RT safety, untested build)

---

## Program Chain subagent

**Skill:** `.cursor/skills/dsp-program-chain/SKILL.md`  
**Task tool:** `generalPurpose`, thoroughness: medium

**Prompt template:**

```text
You are the Program Chain subagent for Audio Plugin Coder.
Read and follow: .cursor/skills/dsp-program-chain/SKILL.md
Architecture: docs/dsp-architecture-specification.md §4

Plugin: <path>
Tier: <L0-L3>
Implement only audio-thread DSP in Source/Dsp/ and processBlock/OSC_CYCLE.
Do not touch PluginEditor except includes if unavoidable.
```

---

## Surface Chain subagent

**Skill:** `.cursor/skills/dsp-surface-chain/SKILL.md`  
**Task tool:** `generalPurpose`

**Prompt template:**

```text
You are the Surface Chain subagent.
Read: .cursor/skills/dsp-surface-chain/SKILL.md
Architecture: docs/dsp-architecture-specification.md §5

Wire APVTS, editor attachments, preset load/save. No DSP in paint/processBlock.
Plugin: <path>
Parameter IDs: <from params.json>
```

---

## Global & Schema subagent

**Skill:** `.cursor/skills/dsp-global-state/SKILL.md`  
**Task tool:** `explore` for manifest discovery, then `generalPurpose` to write files

---

## Parameter Mapping subagent

**Skill:** `.cursor/skills/dsp-parameter-mapping/SKILL.md`  
**Task tool:** `generalPurpose`

Often runs **after** Global, **before** Program Chain smoothing code.

---

## Complexity Assessment subagent

**Skill:** `.cursor/skills/dsp-complexity-assessment/SKILL.md`  
**Task tool:** `explore` — read existing code, score dimensions, no code changes unless asked

**Runs first** before large implementations.

---

## JUCE Scaffold subagent

**Skill:** `.cursor/skills/juce-plugin-scaffold/SKILL.md`  
**Task tool:** `generalPurpose`

Runs `./tools/new-vst.sh` equivalent steps; unique PLUGIN_CODE.

---

## Build Verify subagent

**Skill:** `.cursor/skills/plugin-build-verify/SKILL.md`  
**Task tool:** `generalPurpose` with shell access

---

## logue Hardware Port subagent

**Skill:** `.cursor/skills/logue-hardware-port/SKILL.md`  
**Task tool:** `generalPurpose`

Ports float DSP from VST/`shared/dsp` to logue q31 or mkII float `osc.h`.

---

## Parallelism rules

| Safe in parallel | Must be sequential |
|------------------|-------------------|
| Surface + Program after params.json exists | Scaffold → Global → Mapping → Program |
| Build verify after Program+Surface compile | Complexity before architecture lock |
| explore (read-only) anytime | logue port after VST DSP stable |
