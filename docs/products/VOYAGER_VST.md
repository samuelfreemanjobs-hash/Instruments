# V-Voyager VST (product index)

**Status:** Program doc restored to company memory — implementation spans multiple open PRs/branches. Treat this file as the **source of truth for agent context**, not chat history.

## Purpose

**V-Voyager** is the Instruments **explorer / rompler-style VST** lane: playable factory content, preset morphing, and ship-ready JUCE targets aligned with JD Upgraded quality bars. Name distinguishes the product from SaaS drum tools and from one-off sketch plugins.

## Intended stack

| Layer | Direction |
|-------|-----------|
| DSP / voice | JUCE 8+ in monorepo CMake (same pattern as JD Upgraded, Wave9090) |
| Content | Factory ROM/wavetable banks + SysEx/preset import where applicable |
| Agent workflow | **code-project-planner** PRD → **audio-plugin-coder** APC phases → **golden-wav-qa** |
| ML optional | **ddsp-ml-engineer** for timbre clone targets into wavetable or noise layers |

## Build & run (when target lands in root CMake)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target Voyager_VST3   # target name TBD in PRD
python3 vst-testing-ops/run_business.py --profile ci
```

Until `Voyager_*` exists in `CMakeLists.txt`, agents should reference **JD Upgraded** and **Wave9090** as structural templates and log gaps in the active PRD.

## Related branches / WIP (historical)

Git remote branches (may be stale vs `main`):

- `cursor/vmpc-phase1-juce-foundation-*` — VMPC-style hosting foundation
- `cursor/vst-plugin-factory-*` — factory OS experiments
- `cursor/novadrum-juce-handoff-*` — drum rompler handoffs

Merge status changes frequently — always `git fetch` and read PR descriptions before implementing.

## PRD gate

Before new Voyager C++:

1. Copy [PRD_TEMPLATE.md](../templates/PRD_TEMPLATE.md) → `docs/products/VOYAGER_VST_PRD.md` (or section in this file)
2. Link PRD from root [ARCHITECTURE.md](../../ARCHITECTURE.md) product table
3. Assign **audio-plugin-coder** for Implement phase

## Related docs

- [JUCE_APC_AGENT_BOOTSTRAP.md](../JUCE_APC_AGENT_BOOTSTRAP.md)
- [COMPANY_MEMORY_INDEX.md](../COMPANY_MEMORY_INDEX.md)
- [docs/ROADMAP.md](ROADMAP.md)
