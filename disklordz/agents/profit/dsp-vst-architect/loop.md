# Loop SOP — DSP & VST Factory Architect

## Primary verify commands

```bash
python3 disklordz/integrations/agents/dsp_vst_architect.py --self-test

cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target JDUpgraded_VST3 JDUpgraded_Standalone

python3 vst-testing-ops/run_business.py --profile dsp-only
# before merge / large DSP change:
python3 vst-testing-ops/run_business.py --profile ci

ctest --test-dir build -R SP1200 --output-on-failure
ctest --test-dir build -R Wave9090 --output-on-failure
```

Golden refresh (intentional timbre change only):

```bash
tests/golden/refresh_golden.sh
# commit updated WAVs + manifest.tsv
```

## Control loop

```text
READ ARCHITECTURE.md + product ARCHITECTURE.md
→ LOAD skills/juce-coding.md and/or skills/cpp-realtime-coding.md for the task type
→ REPRODUCE (offline render | failing test | pluginval log)
→ CLASSIFY thread (audio vs message vs UI)
→ MINIMAL FIX in DSP / APVTS / CMake
→ VERIFY (self-test → build target → run_business profile)
→ DOCUMENT extension point in ARCHITECTURE.md
→ HAND OFF golden narrative to golden-wav-qa if regression-only
```

## Terminals

`completed` | `tests_failed` | `pluginval_failed` | `golden_drift` | `preset_break_r10` | `needs_human_host_repro`

## Invariants

- No “done” without `[executed]` verify command for non-trivial DSP diffs.
- `error_log.txt` from vst-testing-ops included on any pipeline failure.
- Preset/state breaking changes stop at R10 until user confirms.
- SIMD changes must compile on both AVX2 and non-AVX2 paths (or explicit ARM NEON branch).

## Sub-agent fan-out

| Phase | Sub-agent | Allowed work |
|-------|-----------|--------------|
| Map codebase | `dsp-vst-architect-explore` | Read/Grep/Glob only |
| Prove fix | `dsp-vst-architect-verify` | Bash cmake/ctest/run_business |
| Apply patch | `dsp-vst-architect-implement` | Edit after coordinator plan |

Max 3 workers per turn unless user approves.
