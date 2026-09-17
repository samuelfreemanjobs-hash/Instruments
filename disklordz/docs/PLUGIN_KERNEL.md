# DiskLordz Plugin Kernel + Plugin Generator

**Do not** let each product invent its own JUCE architecture. Products are **kernel + DSP module + UI skin + presets**.

## Plugin Kernel (shared engine)

Target tree (evolve from `MyFirstPlugin/`):

```text
plugin-kernel/
├── CMakeLists.txt           # juce_add_plugin wrapper
├── core/
│   ├── ParameterSystem.*    # APVTS, IDs, smoothing
│   ├── PresetSystem.*
│   ├── StateSerialization.*
│   ├── MidiRouting.*
│   └── Version.h
├── dsp/                     # reusable modules
│   ├── Saturation.*
│   ├── Bitcrusher.*
│   ├── FilterBank.*
│   └── ...
├── ui/                      # shared components
│   ├── DiskLordzLookAndFeel.*
│   ├── KnobGrid.*
│   └── WaveformDisplay.*
└── products/
    ├── DiskCrusher/         # thin product layer
    ├── TapeLord/
    └── 808Lord/
```

### Product module contract

Each product supplies:

1. `ProductManifest.json` — name, params, default preset, character tags  
2. `ProductDsp.h` — wires kernel modules  
3. `ProductEditor.*` — layout or declarative UI spec  
4. `presets/` — factory + demo presets  
5. `tests/` — product-specific audio fixtures  

## Plugin Generator (brief → package)

Input: structured **PRODUCT BRIEF** (see `plugin-kernel/generator/brief.template.json`).

Pipeline:

```text
Brief → Product Spec → DSP architecture → param defs
    → scaffold product/ under products/
    → UI from design system
    → presets (Preset Factory)
    → docs + tests
    → CI build (VST3 + Standalone)
    → QA Lab gate
    → Product Compiler (installer zip)
```

Agents **fill the framework**, not greenfield `PluginProcessor.cpp` every time.

## Licensing stack (architecture constraint)

| Component | Policy |
|-----------|--------|
| **VST 3 SDK** | Use SDK ≥ 3.8 (MIT); commercial binary distribution permitted under MIT terms ([Steinberg licensing FAQ](https://steinbergmedia.github.io/vst3_dev_portal/pages/FAQ/Licensing.html)) |
| **VST 2** | Do not target; new VST2 licenses are not available |
| **JUCE** | Starter tier while revenue within applicable limits; track revenue vs [JUCE plans](https://juce.com/get-juce/); proprietary value is **your** DSP, DNA, UI system, automation—not JUCE itself |
| **Ship gate** | QA + provenance pass before release |

## Preset Factory integration

Presets are outputs of the compiler, not an afterthought:

- Factory / demo / expansion / random presets from kit DNA + plugin params  
- Versioned alongside plugin semver (`preset_pack_version` in manifest)

## Reference implementation status

| Piece | Status |
|-------|--------|
| APVTS processor patterns | `MyFirstPlugin/` |
| Shared kernel split | Planned → `plugin-kernel/` |
| Generator CLI | Planned → `plugin-kernel/generator/` |
