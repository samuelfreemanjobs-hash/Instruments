# Sub-agents — DSP & VST Factory Architect

Coordinator/worker split per Claude Code coordinator mode.

| Sub-agent | Model | Tools | Role |
|-----------|-------|-------|------|
| `dsp-vst-architect-explore` | haiku | Read, Grep, Glob | Trace signal flow, find APVTS/DSP call sites, collect pluginval logs |
| `dsp-vst-architect-verify` | inherit | Read, Bash | Configure/build targets, run_business profiles, ctest — no source edits |
| `dsp-vst-architect-implement` | inherit | Read, Edit, Write | Minimal C++/CMake diff after coordinator plan |

## Delegation rules

1. Coordinator owns **architecture choice** (control rate, buffer strategy, state layout) — do not delegate design to implement worker.
2. Pass exact **CMake target names** and **test filters** (`ctest -R`) in worker prompts.
3. `dsp-vst-architect-verify` must run `dsp_vst_architect.py --self-test` before long builds when docs/CMake changed.
4. Never parallelize two writers on the same processor/DSP file.
5. Do **not** run `verify-integrations.sh` unless the task touches `disklordz/website/` routes.

## Specialist handoffs

| Signal | Hand off to |
|--------|-------------|
| User only wants golden triage, no code | `golden-wav-qa` |
| SysEx / hardware export | `hardware-preset-designer` |
| License on ML engines | `license-compliance` |

## Fork agents (cache-safe)

When summarizing long transcripts, use fork with **byte-identical** tool array and placeholder tool results (see `loop.md`).
