---
name: vst-engineering
description: Guides JUCE VST/VSTi implementation, debugging, build, and QA in the Instruments monorepo. Use when editing PluginProcessor, DSP engines, APVTS, CMake plugin targets, running pluginval, APC lifecycle gates, or when the user asks for VST engineering, audio plugin coding, or JUCE realtime rules.
---

# VST Engineering

Practical engineering handbook for **JUCE plugins in this repo**. For multi-step greenfield work and subagent routing, start with skill `audio-plugin-coder`; use **this skill** for session discipline, realtime rules, lifecycle gates, builds, and ship criteria.

## Session start

1. Read `AGENTS.md` and `docs/CLAUDE_VST_ENGINEER.md` (RT rules, SmoothParam, QA).
2. Read root `ARCHITECTURE.md`, then the product `ARCHITECTURE.md` / handoff for the plugin you touch.
3. Identify **layout**:
   - **APC scaffold:** `plugins/<Name>/` (specs) + `vst/<Name>/` (code)
   - **Legacy / standalone target:** e.g. `ProphetRev2Trap/`, `Wave9090/`, SP-1200 tree — follow that product doc, not `vst/` only
4. Before implement on a **new** APC plugin: `bash tools/apc-gate.sh <Name> impl` and confirm `plugins/<Name>/status.json` validation flags.

## Lifecycle (mandatory for new VST/VSTi)

Do not create or edit `vst/<Name>/` DSP until Dream + Plan + Design exist under `plugins/<Name>/` unless the user says **skip to impl** (hotfix).

| Phase | Command | Agent skill (`.agents/skills/`) |
|-------|---------|--------------------------------|
| Dream | `/apc-dream` | `dream` |
| Plan | `/apc-plan` | `plan` |
| Design | `/apc-design` | `design` |
| Implement | `/apc-impl` | `impl` + `.cursor/skills/juce-plugin-scaffold` |
| Test | `/apc-test` | `test` + `plugin-build-verify` |
| Ship | `/apc-ship` | `ship` → `release/<Name>/` |

Details: `docs/vst-lifecycle.md`, rule `10-vst-lifecycle-mandatory.mdc`.

**Shipped definition:** `tools/package-vst.sh` output — not “cmake succeeded” alone.

## Real-time rules (audio thread)

Apply to `processBlock`, per-sample loops, and engine `process*` called from them:

| Forbidden | Use instead |
|-----------|-------------|
| `new`, `malloc`, vector growth on audio thread | Preallocate in `prepareToPlay` |
| Mutex, blocking I/O | Atomics, lock-free queues UI → audio |
| `DBG`, `printf`, logging | Debug on UI thread or offline tests |
| Raw APVTS jumps into filters | `SmoothParam` (see `docs/CLAUDE_VST_ENGINEER.md` §7) |

Embedded **logue** targets add: no heap, no printf in production; see skill `logue-hardware-port` — do not mix JUCE GUI into `src/oscillators/`.

## Code layout (JUCE)

| Layer | Files | Responsibility |
|-------|-------|----------------|
| Program | `Source/Dsp/**`, engine `*Engine.h`, `processBlock` body | All audio math |
| Surface | `PluginEditor.*`, APVTS layout, presets | UI only — no DSP |
| Wrapper | `PluginProcessor.cpp` | MIDI, bus layout, param → engine, call engine |

Prefer **one engine header** per product for DSP state (project convention). Keep Dsp headers free of JUCE GUI modules.

**New plugin from template:** `./tools/new-vst.sh <Name> --type synth|effect` — unique `PLUGIN_MANUFACTURER_CODE` and `PLUGIN_CODE` in `CMakeLists.txt` (DAW scan).

## DSP work routing

| Task | Skill |
|------|--------|
| Tier / scope | `dsp-complexity-assessment` |
| `params.json` / preset schema | `dsp-global-state` |
| Curves & smoothing | `dsp-parameter-mapping` |
| Voices, filters, FX order | `dsp-program-chain` → `docs/dsp-architecture-specification.md` |
| Editor layout | `dsp-surface-chain` |
| After code change | `plugin-build-verify` |

Orchestrated workflows: `audio-plugin-coder`.

## Build & QA

**Single plugin under `vst/`:**

```bash
./tools/build-vst.sh <Name>
```

**Monorepo / SP-1200 / multi-target:** see [reference.md](reference.md).

**Before push** after C++ DSP/UI edits:

```bash
python3 vst-testing-ops/run_business.py --profile ci          # full CI parity
python3 vst-testing-ops/run_business.py --profile dsp-only  # faster golden/DSP
```

Intentional output change: `tests/golden/refresh_golden.sh`, then commit WAVs.

**pluginval:** `python3 scripts/vst/run_pluginval.py` (see `docs/CLAUDE_VST_ENGINEER.md` §5).

Compile-only sweep: `./scripts/vst/autopilot-compile.sh` → `vst-testing-ops/error_log.txt`.

## Bugfix vs greenfield

| Situation | Path |
|-----------|------|
| Shipped plugin regression | `/apc-debug` → fix program or surface chain → `/apc-test` → `/apc-ship` |
| Zipper / param noise | `dsp-parameter-mapping`, SmoothParam in engine |
| CMake / JUCE API | `plugin-build-verify`; submodule `git submodule update --init vst/JUCE` |
| Multi-agent ownership | Product handoff in `docs/*_HANDOFF.md` — add files before editing another agent’s paths |

## DiskLordz Audio (CHORD ENGINE)

Factory roadmap: `docs/DiskLordz_Audio_Master_Roadmap_for_Cursor.md`. Windows CI: `DiskLordz-Audio/BUILD.md`. Do not conflate with generic `vst/template` APC flow.

## Agent memory

Non-trivial work: `python3 scripts/agent_memory.py recall --agent vst-factory-director <keywords>` at start; `add` + `check` before reporting done when you learn something verified.

## Deliverables checklist

- [ ] Correct lifecycle gate for new work (or explicit hotfix path)
- [ ] DSP changes covered by build + chosen QA profile
- [ ] `ARCHITECTURE.md` updated if pipeline or modules changed
- [ ] No secrets in code; generic client errors for any HTTP surfaces

## Additional resources

- Build matrix, products, handoffs: [reference.md](reference.md)
- DSP architecture: `docs/dsp-architecture-specification.md`
- Template rule: `.cursor/rules/06-vst-vsti-template.mdc`
