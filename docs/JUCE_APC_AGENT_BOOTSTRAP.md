# JUCE + APC agent bootstrap (Windows & monorepo)

Step-by-step for **Cursor Cloud Agents** and local IDE agents building VST3 plugins. Combines this repo's **CMake/JUCE 8** layout with the open-source **[Audio Plugin Coder (APC)](https://github.com/Noizefield/audio-plugin-coder)** phase workflow.

## Prerequisites (Windows 11)

1. Visual Studio 2022 Build Tools (C++ desktop)
2. CMake 3.22+
3. Git with submodule support
4. Optional: clone APC alongside this repo for `/apc-dream` … `/apc-ship` slash workflows

```powershell
git clone --recursive https://github.com/Noizefield/audio-plugin-coder.git
cd audio-plugin-coder
npx github:Noizefield/audio-plugin-coder
# follow /apc-setup wizard
```

## Monorepo path (Instruments — preferred for JD / Wave9090 / TrapForge)

Agents **must** read root [`ARCHITECTURE.md`](../../ARCHITECTURE.md) and the product `ARCHITECTURE.md` before editing DSP.

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target JDUpgraded_VST3 Wave9090_VST3 TrapForge_VST3
python3 vst-testing-ops/run_business.py --profile ci
./scripts/sync-compile-commands.sh
```

Linux CI matches [`build.yml`](../.github/workflows/build.yml).

## APC phase map → this repo

| APC phase | Agent deliverable here |
|-----------|-------------------------|
| **Dream** | Issue + [`PRD_TEMPLATE.md`](templates/PRD_TEMPLATE.md) one-pager |
| **Plan** | Product `ARCHITECTURE.md` delta + CMake target name |
| **Design** | Parameter layout, factory presets, UI shell |
| **Implement** | `Source/` or `disklordz/*/plugin/Source/` + tests |
| **Ship** | Green CI, golden WAV policy, draft PR only |

## Audio Plugin Coder agent

Fleet id: **`audio-plugin-coder`**. Skill: `.github/skills/disklordz-audio-plugin-coder/SKILL.md`.

Do **not** duplicate JUCE under random folders — extend root `CMakeLists.txt` or APC `plugins/` with a documented bridge in PRD.

## Realtime rules

- No heap alloc, locks, or logging on the audio thread
- Document block size assumptions in product `ARCHITECTURE.md`
- After C++ DSP edits: `run_business.py --profile ci`

## Related agents

- **code-project-planner** — PRD before Implement
- **ddsp-ml-engineer** — Python reference curves in `tools/drum-synth-blueprint/`
- **golden-wav-qa** — regression gates
