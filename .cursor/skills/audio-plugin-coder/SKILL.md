---
name: audio-plugin-coder
description: Orchestrates VST/VSTi and cross-target DSP work using the DSP architecture spec, subagent skills, and repo templates. Use when building or reviewing audio plugins, synths, effects, parameter systems, or ports between JUCE and logue SDK.
---

# Audio Plugin Coder (orchestrator)

## Authority

- **Architecture:** `docs/dsp-architecture-specification.md`
- **VST template:** `vst/template/`, rule `06-vst-vsti-template.mdc`
- **logue units:** `src/oscillators/`, mkII `src/mkii/oscillators/`
- **Agent map:** `.cursor/agents/audio-plugin-coder/SUBAGENTS.md`

## Workflow

1. **Clarify target** — VST | logue mkI | mkII | multi-target.
2. **Assess complexity** — invoke skill `dsp-complexity-assessment`; publish tier L0–L3.
3. **Scaffold** — if new VST: skill `juce-plugin-scaffold` → `vst/<Plugin>/`.
4. **Global schema** — skill `dsp-global-state` → `params.json` / align `manifest.json`.
5. **Parameter mapping** — skill `dsp-parameter-mapping` → smoothing + curves.
6. **Program chain** — skill `dsp-program-chain` → `Source/Dsp/`, `processBlock`.
7. **Surface chain** — skill `dsp-surface-chain` → Editor, APVTS, presets.
8. **Verify** — skill `plugin-build-verify` (or document toolchain gap).
9. **Hardware port** (optional) — skill `logue-hardware-port`.

## Decision table

| User asks for | Start with |
|---------------|------------|
| New synth VST | juce-plugin-scaffold → complexity → global → program |
| New effect VST | scaffold (effect template when added) → program |
| More knobs / preset | dsp-global-state → dsp-surface-chain |
| Sounds wrong / clicks | dsp-program-chain + dsp-parameter-mapping (smoothing) |
| NTS-1 unit | logue-hardware-port; do not use JUCE in `src/oscillators` |
| Same sound VST + NTS-1 | shared/dsp float core + two glue layers |

## Output format for the user

1. **Tier** and score breakdown  
2. **Program chain** (ordered list)  
3. **Parameter table** (logical ID, range, smoothing)  
4. **Files touched**  
5. **Build command** or Docker fallback  

## Subagent invocation

When work is large, spawn Task subagents using prompt templates in
`.cursor/agents/audio-plugin-coder/SUBAGENTS.md`. Pass the handoff packet
(plugin path, tier, param list).

## Non-goals

- Do not replace JUCE with another framework unless user insists.
- Do not merge VST and logue into one CMake target.
- Do not skip realtime rules on embedded targets.
