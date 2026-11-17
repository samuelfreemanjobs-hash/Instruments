# DRUMSMITH architecture (v2.0 in-repo)

## Purpose

Agent package for Disklordz drum kit design + offline/native synthesis + technical QA. **Phase = agent build**, not commercial release. CALIPER is read-only QA. FORGE receives JSON (and later JUCE) handoffs.

## Build & run

```bash
cd claude/drumsmith

# Python (46 tests: 28 + 10 + 6 + 2)
python3 scripts/self_test.py
python3 scripts/test_upgrade.py
python3 scripts/test_v13.py
CXX=g++-12 python3 scripts/test_v20.py

# Native C++20 renderer (no JUCE required)
cmake -S native -B native/build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-12
cmake --build native/build -j
ctest --test-dir native/build --output-on-failure

# Native 64-hit kit (uses plan inventory)
python3 scripts/native_factory.py \
  --out /tmp/Native_TLS \
  --renderer native/build/drumsmith-render \
  --genre drift-phonk --count 64 --sku TLS --plan tail-light-static
```

JUCE VST3/standalone (optional, needs JUCE 8):

```bash
cmake -S native -B native/build-juce -DDRUMSMITH_BUILD_JUCE=ON ...
```

Not compiled/verified on this Cloud Linux agent by default.

## Data flow

```text
INTAKE / factory genre recipes
  → Python render OR native drumsmith-render
  → verify_kit + pitch_check + diversity_audit
  → export_forge.json / (optional) JUCE plugin source
```

## Key modules

| Path | Role |
|------|------|
| `native/include/` | C++20 drum engine + WAV writer |
| `native/tools/render_cli.cpp` | Headless renderer |
| `native/juce/` | VST3 + standalone source (unverified build) |
| `scripts/native_factory.py` | Batch native kit + QA |
| `scripts/factory.py` / `genre_profiles.py` | v1.3 genre recipes |
| `scripts/recipe_engine.py` | v1.2 parametric recipes |
| `examples/tail-light-static/tls-v20-native-cxx` | Packaged native 64-hit example |
| `workbench/` | Single-file UI template |

## Related

- `project.md` — Rule #1 status
- `docs/WORKFLOW_V13.md`, release gate docs (deferred commercial)
