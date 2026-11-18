# Factory preset banks (500 programs)

See **[PRESET_FACTORY_200.md](PRESET_FACTORY_200.md)** for UI/browser notes; counts below are **500**.

## JD Upgraded

| Range | Bank | Count |
|------:|------|------:|
| 0–199 | Legacy (EP → Lo-Fi) | 200 |
| 200–249 | Synthwave | 50 |
| 250–299 | House | 50 |
| 300–349 | Afro | 50 |
| 350–399 | Jersey | 50 |
| 400–449 | Ambient | 50 |
| 450–499 | MPC Pack | 50 |

**Dedup:** `FactoryPatchDedup.cpp` — unique names + unique tone fingerprints (auto-nudge waves on collision).

**QA:** `ctest -R QAFactoryPresetsJD`

## WAVE-9090

10 categories × **50** programs: TRAP, PHONK, DRILL, BELL, PAD, LEAD, PLUCK, KEYS, SYNTH, MPC.

**Dedup:** in `FactoryPresets.cpp` (`deduplicatePresetStore`).

**QA:** `ctest -R Wave9090PresetQuality`

## Quality agent

Run before release:

```bash
cmake --build build -j --target QAFactoryPresets Wave9090PresetQualityTests
ctest --test-dir build -R "QAFactoryPresetsJD|Wave9090Preset" --output-on-failure
```

Artifacts: `/opt/cursor/artifacts/qa-preset-quality-report.md` (from Cloud QA agent).
