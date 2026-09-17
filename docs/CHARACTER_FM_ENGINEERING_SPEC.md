# Character FM — engineering specification (Milestone 1)

Hand this document to implementation agents **before** writing DSP. Milestone 1 explicitly excludes Character Engine, AI, and premium UI.

## Milestone 1 definition of done

1. **CMake** product target `CharacterFM_VST3` (+ Standalone) under `products/CharacterFM/` or `Source/CharacterFM/` until kernel migration.  
2. **Six operators** per voice: ratio/fixed, level, ADSR, velocity sensitivity, key scale, detune, phase, feedback (where graph allows).  
3. **ModulationGraph** — algorithms are **data**, not 32 hard-coded switch statements.  
4. **Voice manager** — 16–32 voices, voice steal, zero heap alloc on audio thread.  
5. **APVTS** — stable parameter IDs, `SmoothedValue` for all modulation targets.  
6. **Preset schema v1** — JSON/XML canonical format; round-trip save/load.  
7. **Minimal editor** — algorithm #, master level, panic; enough to QA in a DAW.  
8. **Tests** — offline render smoke test, preset round-trip, graph validation unit tests.  

## Thread model

| Thread | Responsibility |
|--------|----------------|
| **Audio** | FM voice render, apply **precomputed** character coefficients (Milestone 2+), no locks, no alloc |
| **Message / MIDI** | Note on/off, pitch bend, CC → voice allocator |
| **UI** | APVTS attachments only; no direct DSP mutation |
| **Character (slow)** | 10–50 Hz (or block-rate): thermal, power, drift RNG, DNA → coefficient buffers |

Character Engine (Phase 6) writes `CharacterCoefficients` atomically or double-buffered; audio thread reads snapshot only.

## Signal flow

```text
MIDI → VoiceAllocator → FMVoice[N] → ModulationGraph (per voice)
                              ↓
                    OperatorChain (6 × osc + env)
                              ↓
                    Sum → (future: Output DSP) → out L/R
```

## Core classes (C++)

```text
Source/DSP/FMEngine/
  ModulationGraph.h      // edges: src op → dst op, amount; feedback taps
  AlgorithmData.h        // loads JSON / embedded defaults (32 DX7 routes)
  Operator.h             // phase accumulator, ratio, envelope, output
  FMVoice.h              // 6 operators + graph instance state
  FMEngine.h             // polyphony, renderBlock()
  VoiceAllocator.h

Source/Parameters/
  ParameterIds.h         // constexpr IDs
  ParameterLayout.cpp    // APVTS layout

Source/Presets/
  PresetSchema.h         // version, fm{}, metadata{}
  PresetManager.cpp

Source/Plugin/
  CharacterFMProcessor.*
  CharacterFMEditor.*    // Milestone 1 minimal
```

## Modulation graph data shape

```json
{
  "operators": 6,
  "connections": [
    { "source": 6, "destination": 5, "amount": 1.0 },
    { "source": 5, "destination": 4, "amount": 1.0 },
    { "source": 4, "destination": 1, "amount": 1.0 }
  ],
  "feedback": [{ "operator": 1, "amount": 0.0 }]
}
```

Validation rules: no cycles unless feedback-tagged; operator indices 1–6; amount ∈ [0, 1] for normalized mod depth (scale in DSP).

## Preset schema v1 (canonical)

```json
{
  "format": "DISKLORDZ_CHARACTER_FM_PRESET",
  "version": 1,
  "name": "Living Glass",
  "fm": {
    "algorithmId": 7,
    "graph": { },
    "operators": [ { "ratio": 1.0, "level": 99, "adsr": { } } ]
  },
  "character": { },
  "macros": { },
  "effects": { },
  "metadata": { "tags": ["bell", "80s"] }
}
```

Milestone 1: `character`, `macros`, `effects` may be empty objects. Version field is mandatory.

## DX7 compatibility (Phase 5 — design now)

```text
SysEx bytes → Dx7PatchParser → CanonicalPatch → FMEngine
CanonicalPatch → Dx7Encoder → .syx
```

AI and external tools emit **CanonicalPatch JSON** only; C++ validates ranges and packing.

## Character Engine (Phase 6 — interface stub in Milestone 1)

```text
CharacterEngine
  ThermalModel      // per-op temperature, ambient, inertia
  PowerModel        // sag, recovery, poly load
  ComponentModel    // tolerance per op / per voice
  OscillatorDrift   // Hz drift coefficients
  PhaseDrift
  FMDrift           // ratio/feedback micro drift
  CharacterDNA      // seed → unit id + trait vector
  AgingModel        // optional long-horizon (Living mode)
```

**Character DNA algorithm (deterministic):**

1. Input: 64-bit seed (user “Generate Unit” or hash of preset id + user salt).  
2. Expand with SplitMix64 → six trait scalars ∈ [0, 1]: thermal, power, osc, phase, fm, component, noise.  
3. Display `UNIT #` as first 5 hex chars of seed.  
4. Traits scale max drift depths defined by **Character Mode** preset (Pristine…Chaos).  
5. **Unit Lock** stores seed in preset `character.dnaSeed`.

Audio thread receives:

```cpp
struct CharacterCoefficients {
  std::array<float, 6> opCentsDrift;
  std::array<float, 6> opLevelBias;
  float powerSag;
  // updated at slow rate; smoothed per coeff
};
```

## Performance targets

- 16 voices × 6 ops @ 48 kHz, 128 sample buffer: CPU budget **< 15%** one core (profile on CI Linux).  
- Denormals off; idle operators skip when envelope silent.  
- SIMD optional on operator sum, not required Milestone 1.

## Parameter IDs (excerpt — implement in ParameterIds.h)

| ID | Range | Notes |
|----|-------|-------|
| `masterLevel` | 0–1 | |
| `algorithmSelect` | 0–31 | maps to graph preset |
| `opNRatio` | float | N = 1..6 |
| `opNLevel` | 0–1 | |
| `opNAttack` … `opNRelease` | time constants | |

Character params (Phase 6): `characterAmount`, `characterMode`, `dnaSeed`, `unitLock`, lab params behind advanced page.

## First implementation Work Orders (Airtable)

| WO | Owner role | Output |
|----|------------|--------|
| CFM-M1-01 | Architect | CMake target + empty processor loads in DAW |
| CFM-M1-02 | C++ Engineer | Operator + single voice sine FM smoke |
| CFM-M1-03 | C++ Engineer | ModulationGraph + 32 algorithms as JSON |
| CFM-M1-04 | C++ Engineer | Polyphony + steal |
| CFM-M1-05 | C++ Engineer | APVTS + preset schema round-trip |
| CFM-M1-06 | QA Engineer | OfflineRender-style test + CI job |
| CFM-M1-07 | Marketing Growth | SKU one-pager + CHARACTER ENGINE trademark checklist |

## Acceptance command (future)

```bash
cmake -B build -DCHARACTER_FM=ON ...
cmake --build build --target CharacterFM_OfflineRender
./build/CharacterFM_OfflineRender --preset tests/fixtures/init.json --note 60 --seconds 0.5
```
