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

PR: **#50** (`cursor/sp1200-standalone-build-1a4f` → `main`).

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

## Merge checklist (human)

1. Review PR #50 diff + `SP1200/ARCHITECTURE.md`  
2. Run manual pass on [SP1200_QA_CHECKLIST.md](SP1200_QA_CHECKLIST.md) (1–2 h)  
3. Merge to `main`; tag release optional (`sp1200-v0.1.0`)  
4. Close tracking issue if any  

## Related docs branch

Spec-only updates may live on `cursor/sp1200-standalone-spec-1a4f`; merge or cherry-pick into `main` after build PR lands if still diverged.
