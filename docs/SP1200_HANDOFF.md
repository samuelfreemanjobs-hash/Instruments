# SP-1200 Drumulator — v1 handoff

Product spec: [SP1200_STANDALONE_SPEC.md](SP1200_STANDALONE_SPEC.md)  
Architecture: [SP1200/ARCHITECTURE.md](../SP1200/ARCHITECTURE.md)  
Manual QA: [SP1200_QA_CHECKLIST.md](SP1200_QA_CHECKLIST.md)

## Status (implementation)

| Phase | Scope | Status |
|-------|--------|--------|
| **P0** | 26.04 kHz / 12-bit / 16 voices / 7:00 pool; WAV + record; `.sp12p` | **Done** |
| **P1** | Console shell; pads/faders; SQ-1 + keyboard; transport | **Done** |
| **P2** | MOD 11 chop; pattern model; console step record | **Done** |
| **P2b** | MOD 20 piano roll + stacks; song tab; choke groups | **Done** |
| **P3** | MOD 15 SSM; MOD 30; LCD/keypad scrub | **Done** |
| **P4** | QA checklist; optional VST3 | **Automated QA in CI**; manual checklist open; **VST3 built** |

**Shipped on `main`:** merged **2026-10-06** via PR [#50](https://github.com/samuelfreemanjobs-hash/Instruments/pull/50) (merge commit `0a32432879c2a8f691758a72cd31ece89ae316b2`).

## Build & verify

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target SP1200_Standalone SP1200_VST3 \
  SP1200MemoryTests SP1200SequencerTests SP1200ProjectFileTests
ctest --test-dir build -R SP1200 --output-on-failure
```

Run standalone:

```bash
./build/SP1200/SP1200_artefacts/Release/Standalone/SP-1200\ Drumulator
```

Optional smoke import (no file dialog): set `SP1200_AUTO_IMPORT=/path/to/sample.wav` before launch. Drag-and-drop WAV/AIFF/FLAC onto the editor also imports.

## Automated tests (CI)

- `SP1200Memory` — 12-bit pack, pool cap, combine segments, bank quota  
- `SP1200Sequencer` — patterns, song END, MIDI clock, **96 PPQN swing** tick math  
- `SP1200ProjectFile` — `.sp12p` round-trip  

Monorepo **Build / cmake** job builds all targets (including SP1200 VST3) and runs SP1200 ctests.

## Deferred / non-blocking (spec §10)

- Full **01–34** module directory parity (core modules 10–15, 20, 24, 30 implemented as tabs)  
- **AUTO CORRECT**, repeat resolution UI, ARM VU on LCD  
- Embedded sample compression codec  
- Windows VST3 packaging (Linux standalone + VST3 validated in CI build)  
- pluginval harness for SP1200 (JD Upgraded uses `ci-verify`; SP1200 uses unit tests only)

## Post-merge checklist (human)

1. ~~Review PR #50 and merge to `main`~~ **Done** (2026-10-06)  
2. ~~Confirm **Build / cmake** green on `main`~~ **Done** — run [37502018473](https://github.com/samuelfreemanjobs-hash/Instruments/actions/runs/37502018473) (Build + **SP1200 unit tests** success)  
3. Manual pass on [SP1200_QA_CHECKLIST.md](SP1200_QA_CHECKLIST.md) (1–2 h) — **partial smoke 2026-10-06** ([report](SP1200_SMOKE_TEST_2026-10-06.md)): standalone launch; tabs 10/11/12–14/20/24/15/SETUP; Space transport; keys 1–4 banks (no sample import / MIDI hardware in smoke pass)  
4. Optional: tag release `sp1200-v0.1.0` (after full P4 checklist)  
5. Close obsolete combine PR [#68](https://github.com/samuelfreemanjobs-hash/Instruments/pull/68) manually if still open  
6. ~~Console GUI overlap (LCD vs faders)~~ **Fixed** on `main` via PR [#90](https://github.com/samuelfreemanjobs-hash/Instruments/pull/90)  

## Branch hygiene

Legacy implementation branches `cursor/sp1200-standalone-build-*` and spec branches `cursor/sp1200-standalone-spec-*` are superseded by **`main`** + `SP1200/`. Cherry-pick doc-only commits from open PRs if still useful; do not re-merge full feature branches.
