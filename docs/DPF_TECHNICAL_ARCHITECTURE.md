# DiskLordz Plugin Framework (DPF) — Technical Architecture

**Status:** Target layout for CHORD ENGINE and follow-on SKUs.  
**Business context:** [DISKLORDZ_MASTER_PLAN.md](DISKLORDZ_MASTER_PLAN.md)  
**Product requirements:** [CHORD_ENGINE_PRD.md](CHORD_ENGINE_PRD.md)

---

## Purpose

DPF is the **shared proprietary codebase** for all DiskLordz Audio plugins. Product logic (theory, DSP, genre rules) must remain **independent of JUCE** so the host framework can change without rewriting musical engines.

---

## Layer diagram

```text
┌─────────────────────────────────────────────────────────┐
│ PRODUCT INTERFACE SYSTEM (branding, browsers, grids)   │
├─────────────────────────────────────────────────────────┤
│ DPF-Core: parameters · presets · state · licensing UI  │
├──────────────┬──────────────────────────────────────────┤
│ DPF-MIDI     │ DPF-DSP (saturation, crush, filters…)    │
│ theory·MIDI  │                                          │
├──────────────┴──────────────────────────────────────────┤
│ JUCE adapter (VST3/AU later) — thin I/O + timing glue   │
└─────────────────────────────────────────────────────────┘
```

---

## Target repository layout

```text
DiskLordz-Audio/          # future product repo or monorepo subtree
├── DPF-Core/
│   ├── PluginFramework/
│   ├── UserInterface/
│   ├── PresetManager/
│   ├── ParameterSystem/
│   ├── StateManager/
│   ├── Licensing/
│   └── Utilities/
├── DPF-MIDI/
│   ├── MusicTheory/
│   ├── ChordEngine/
│   ├── VoicingEngine/
│   ├── ProgressionEngine/
│   ├── GenreEngine/
│   └── MIDIProcessor/
├── DPF-DSP/
│   ├── Saturation/
│   ├── Distortion/
│   └── …
├── Products/
│   ├── ChordEngine/
│   ├── ArpMachine/
│   └── …
├── Tests/
│   ├── MusicTheoryTests/
│   ├── MIDI_Tests/
│   └── PluginTests/
└── Build/
    ├── Windows/
    └── macOS/
```

Until the dedicated repo exists, Milestone 001 may live under this monorepo’s JUCE tree with a migration plan documented in weekly reviews.

---

## Stack

| Technology | Role | Cost |
|------------|------|------|
| C++ | Plugin + engines | $0 |
| JUCE | VST3 shell, UI host | Starter tier ($0 until revenue cap) |
| CMake | Build | $0 |
| VS Community | Windows toolchain | $0 eligible |
| Python | Theory tests, tooling | $0 |
| JSON | Presets, genre rules | $0 |

---

## CHORD ENGINE module map

| Module | Responsibility |
|--------|----------------|
| Theory Engine | Notes, keys, scales, intervals |
| Chord Engine | Formulas, extensions |
| Voicing Engine | Inversions, spread, voice leading |
| Progression Engine | Sequences, generation |
| Genre Engine | Original style rules (Memphis, Dark Trap, …) |
| MIDI Engine | Events, timing, export, host sync |
| Interface Engine | Key/scale, browser, grid, voicing studio |

CHORD ENGINE is **MIDI-generating** (not audio FX): MIDI in/out, transport sync, automation, preset persistence.

---

## Engineering separation

| Class | I/O | Examples |
|-------|-----|----------|
| MIDI products | MIDI in → MIDI out | CHORD ENGINE, ARP MACHINE |
| Audio FX | Audio in → Audio out | 808 SATURATOR, MEMPHIS GRIT |

Test each class in FL Studio, REAPER, Ableton, Studio One, Cubase per QA matrix in CHORD ENGINE PRD.

---

## Build & test (this monorepo today)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j
python3 vst-testing-ops/run_business.py --profile ci
```

New CHORD ENGINE targets will extend CMake when Milestone 001 lands.

---

## Extension points

- Add genre packs as JSON under `GenreEngine` without recompiling theory core.
- New SKU = new `Products/<Name>/` + thin JUCE wrapper reusing DPF-Core.
- macOS: duplicate `Build/macOS` signing pipeline when year-one gate opens.

---

## Related docs

- [docs/ARCHITECTURE.md](ARCHITECTURE.md) — JD Upgraded / existing JUCE patterns
- [vst-testing-ops/ARCHITECTURE.md](../vst-testing-ops/ARCHITECTURE.md) — pluginval CI
- [docs/DISKLORDZ_PLUGIN_TRACKS.md](DISKLORDZ_PLUGIN_TRACKS.md) — Cursor vs Antigravity lanes
