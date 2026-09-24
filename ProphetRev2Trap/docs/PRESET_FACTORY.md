# Night Circuit — preset factory (1,028 programs)

## Honest scope

| Layer | Count | Meaning |
|-------|------:|---------|
| **Foundation patches** | 21 | Hand-authored starting points (name + category + `SynthParams`) |
| **Variations** | 1,007 | Deterministic mutations of a foundation (not individually auditioned) |
| **Total factory bank** | **1,028** | Built at first use via `PresetFactory::buildLibrary()` |

This is a **reproducible starting library**, not 1,028 unique sound-design sessions.

## Code map

| File | Role |
|------|------|
| `FoundationPresets.cpp` | 21 foundation patches |
| `PresetFactory.cpp` | Variation rules + naming (`Foundation · V001`) |
| `FactoryPresets.cpp` | Lazy singleton `getFactoryPresets()` |

## Variation rule (summary)

- Seed: `foundationIndex * 1009 + variationIndex * 7919 + 0x0C1C`
- Perturbs cutoff, resonance, detune, mix, envelopes, drive (bounded clamps)
- Distribution: first **20** foundations → **48** variations each; foundation **21** → **47** variations  
  (`20×48 + 47 + 21 foundations = 1028`)

## User bank

Separate **User** category on disk (`UserPresetStore`); not counted in 1,028.

## QA expectation

- `NightCircuitTests` asserts count, foundation tally, deterministic rebuild
- Musical QA: spot-check foundations + random variation indices before calling a preset “ship grade”
