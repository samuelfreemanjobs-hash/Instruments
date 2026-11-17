# project.md — DRUMSMITH (Drum Sample & Kit Product Designer + DSP Audio Engineer Agent)

**Updated:** 2026-11-17 · **Phase:** **agent build** (not a commercial kit release) · **In-repo:** **v2.0** (merged from `drumsmith-v2.0.zip`)

## Scope

Four-layer agent + CALIPER + Python factory + **native C++20 renderer** + optional JUCE 8 plugin/standalone **source**. Current goal: working agent package in the monorepo with green CI. Storefront / commercial masters deferred.

**Monorepo path:** `claude/drumsmith/`

## Done

- [x] Landed v1.0 → v1.3 lineage; workbench HTML; Tail Light Static W1 notes
- [x] **Merge drumsmith-v2.0.zip** (native engine, juce sources, `native_factory.py`, `test_v20.py`, `tls-v20-native-cxx` example)
- [x] Cloud verify: Python **46/46** (28+10+6+2) with `CXX=g++-12`
- [x] Native `cmake` + `ctest` (`native_dsp` PASS); `drumsmith-render` builds
- [x] `native_factory.py` smoke → technical PASS, `release_ready=false`
- [x] Packaged `examples/tail-light-static/tls-v20-native-cxx` verify **64/64** PASS
- [x] CI: Python suites + native build/ctest + factory smoke; `test_v20` prefers `g++-12` (default `c++` lacks libstdc++ on this image)

## Lineage

| Version | Status |
|---------|--------|
| v1.3 | Superseded tree (capabilities retained) |
| **v2.0** | **Canonical** |

### v2.0 surface

- Eight C++ voice types; 32-voice poly engine; headless WAV renderer
- Five genre profiles (Python) preserved
- `native_factory.py` batch + QA + diversity
- JUCE VST3/standalone **source** under `native/juce/` — **not compiled** on Cloud (needs JUCE + typically Windows VS)

## Agent ops (v2.1)

- [x] Cursor skill router `.cursor/skills/drumsmith/SKILL.md` + `agent.yaml` 2.1 skill list
- [x] CALIPER Cursor path `docs/CALIPER_CURSOR.md`
- [x] Workbench loop `docs/WORKBENCH_LOOP.md`
- [x] Scripts: `learn_midi_groove.py`, `trap_loop_architect.py` + `test_groove_loop.py`
- [x] Agent memory id `drumsmith` (L-0001/L-0002, I-0001)
- [x] W1–W5 exercise log `docs/W1_W5_TLS_EXERCISE.md` (W4 blocked on HITL; listening stub)

## To do (agent build)

- [ ] Human listening notes + optional W5 recipe tweak
- [ ] Optional Windows: `-DDRUMSMITH_BUILD_JUCE=ON` + DAW smoke (separate machine)
- [ ] Optional waveform editor / kit manager (announced not implemented)
- [ ] Deferred commercial: ear review, device profiles, release gate

## Quick verify

```bash
cd claude/drumsmith
python3 scripts/self_test.py && python3 scripts/test_upgrade.py && python3 scripts/test_v13.py
CXX=g++-12 python3 scripts/test_v20.py
cmake -S native -B native/build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-12
cmake --build native/build -j && ctest --test-dir native/build --output-on-failure
```
