---
name: juce-dsp-plugin-ci
description: "Change JUCE/C++ DSP safely with CMake build and vst-testing-ops CI profile. Use after edits under Source/, Wave909/, or tools linked to plugin audio."
---

# JUCE DSP + plugin CI

## Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j
```

## QA (required before push)

```bash
python3 vst-testing-ops/run_business.py --profile ci
```

On failure: read `vst-testing-ops/error_log.txt`.

## Intentional DSP output change

```bash
tests/golden/refresh_golden.sh
```

Commit updated golden WAVs with explicit PR note.

## Realtime rules

No alloc/block on audio thread — see product ARCHITECTURE.md threading sections.

## Subagent

Use `ci-investigator` for GitHub Actions failures on plugin workflows.
