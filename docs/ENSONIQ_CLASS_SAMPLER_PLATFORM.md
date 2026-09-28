# Ensoniq-class sampler platform (shared B + C)

**Status:** architecture only — no CMake targets yet

Two customer SKUs share one factory strategy:

| SKU doc | Machine inspiration | HISE sketch folder | Engine profile |
|---------|-------------------|--------------------|----------------|
| [EPS_CLASS_ROMPLER.md](EPS_CLASS_ROMPLER.md) | EPS-16 Plus | `hise-sketch/EpsClass/` | `otis5505` (ES5505-like) |
| [ASR_CLASS_ROMPLER.md](ASR_CLASS_ROMPLER.md) | ASR-10 | `hise-sketch/AsrClass/` | `otto5506` + ESP-class FX |

## Shared (implement once)

- **Zone maps** — note, velocity, loop points, root key (generalizes JD `RomBank` ideas).
- **Preset container** — JSON or binary blob: `{ profile, layers[], maps[], macroFx{} }`.
- **Voice pool** — 32 voices, note steal, no audio-thread allocation (JD `VoicePool` patterns).
- **Offline QA** — `OfflineRender` + per-SKU golden directories under `tests/golden/`.
- **Legal** — no Ensoniq ROM, OS images, or trademark boot text; user disk import reserved for class A efforts only.

## Profile-specific (two code paths or one enum)

| Module | `otis5505` | `otto5506` |
|--------|------------|------------|
| Phase / interpolation / loops | EPS B spec | OTTO deltas ([ASR doc](ASR_CLASS_ROMPLER.md#b1--voice-specification-otto-profile)) |
| Filter topology | Four-pole K-style | Same family; coefficients tuned per profile |
| Post-voice chain | Minimal (EQ optional) | **B2** ESP-class insert required for ASR identity |

## Recommended build order

1. **Marketing:** decide one SKU first (ASR often broader appeal for hip-hop/electronic; EPS tighter for “classic Ensoniq sampler” niche) or dual HISE sketches in parallel.
2. **HISE Phase 1:** ship **C** for chosen SKU; bake FX in HISE for ASR.
3. **JUCE port WO:** land shared `EnsoniqClass` DSP + golden tests; fork UI strings and factory content per SKU.
4. **Second SKU:** mostly new **C** + profile toggle + ASR B2 if not done yet — avoid second full plugin rewrite.

## Class A (full emulation) — out of platform scope

| Machine | Community / reference |
|---------|------------------------|
| EPS-16 Plus | [mardlib/Ensoniq-EPS-16-Plus](https://github.com/mardlib/Ensoniq-EPS-16-Plus) |
| ASR-10 | [MAME `esqasr.cpp`](https://github.com/mamedev/mame/blob/master/src/mame/ensoniq/esqasr.cpp) (preliminary) |

Do not merge class A cores into this platform without a separate product line and legal review.

## Content authoring

How to record, edit, zone, and import samples: [ENSONIQ_SAMPLE_AUTHORING.md](ENSONIQ_SAMPLE_AUTHORING.md).

## Related

- Repo index: [ARCHITECTURE.md](../ARCHITECTURE.md)
- Plugin tracks: [DISKLORDZ_PLUGIN_TRACKS.md](DISKLORDZ_PLUGIN_TRACKS.md)
