# DISKLORDZ ROMPLER — Cursor Cloud Agent prompt

Paste this into a **Cloud Agent** task (optionally prepend the full factory from [PLUGIN_FACTORY_OS_PROMPT.md](PLUGIN_FACTORY_OS_PROMPT.md)). This prompt is **repository-oriented and incremental** — not a HISE dump or one-shot C++ generation.

## Goal

Build **DISKLORDZ ROMPLER** v1.0: a **sample-based** multi-tone rompler for trap / phonk / dark 90s–00s workflow, matching the approved UI mockups, as a **JUCE + CMake** product in this monorepo.

## UI reference (source of truth for layout)

| Asset | Description |
|-------|-------------|
| [assets/disklordz-rompler-main-ui.png](assets/disklordz-rompler-main-ui.png) | MAIN + Browser tab — macros, 4 tones, filter/amp/LFO, mod matrix, FX row, keyboard |
| [assets/disklordz-rompler-ui-guide.png](assets/disklordz-rompler-ui-guide.png) | Full UI guide — tabs, tone editor, patch matrix, perf, mod, FX, design tokens |

**Visual language:** dark navy panels (`#0B0F14` / `#121B21`), neon purple `#7861FF`, cyan `#0BE5FF`, magenta `#FF3ED9`. Typography: Orbitron (headers), Inter (labels) — use JUCE fonts or bundled equivalents.

## Context — read before writing code

1. [/ARCHITECTURE.md](../ARCHITECTURE.md) — monorepo index  
2. [AGENTS.md](../AGENTS.md) — build, QA, git  
3. [Wave909/ARCHITECTURE.md](../Wave909/ARCHITECTURE.md) — reference **JUCE product layout** (CMake subdirectory, APVTS, tests)  
4. [docs/HISE_ANTIGRAVITY_LANE.md](HISE_ANTIGRAVITY_LANE.md) — rompler *content* may overlap; **this product is JUCE**, not HISE export  
5. [.cursor/rules/architecture-documentation.mdc](../.cursor/rules/architecture-documentation.mdc)

**Suggested product path:** `DisklordzRompler/` (adjust after repo search if a better name exists). Register in root `CMakeLists.txt` and root `ARCHITECTURE.md` when the shell builds.

## PLUGIN IDEA (product)

```text
DISKLORDZ ROMPLER — sample-based instrument for dark trap, phonk, and 90s/00s rompler workflow.

Engine: up to 4 tone layers per patch (e.g. Memphis bell, chopped keys, pad, sub), each with
level/pan and sample mapping; global filter, amp, LFOs, modulation matrix, insert/send/master FX.

UX: Serum/Nexus-style dark UI with top tabs — BROWSER | MAIN | TONE | PATCH | PERF | MOD | FX | SETTINGS.
Eight performance macros (Character, Brightness, Drive, Space, Movement, Width, Decay, Crush).
Preset browser with instrument type, genre, mood, character filters and search.

Sonic target: phonk bells, dark keys, 808-friendly subs, lo-fi/grit options via macro Crush/Drive.
```

## Execution mode (mandatory)

Follow **PLUGIN FACTORY OS §0 Cursor Repository Workflow**. In order:

### Step 1 — Discover (commit: docs only)

- Search repo for rompler, sampler, multi-sample, `Synthesiser`, disk streaming patterns in `Source/`.
- Write `DisklordzRompler/Docs/PRODUCT_SPEC.md` (P0/P1/non-goals).
- Write `DisklordzRompler/ARCHITECTURE.md` (threading, tone layers, sample loading thread, signal flow).
- Write `DisklordzRompler/Docs/UI_SPEC.md` — map each tab to components; P0 = MAIN + minimal BROWSER.
- Document **sample pack contract** (folder layout, manifest format) under `Docs/` — no copyrighted samples in git.

### Step 2 — Build shell (commit: builds)

- `CMakeLists.txt` + empty `PluginProcessor` / `PluginEditor` → `DisklordzRompler_VST3` + Standalone.
- Prove: `cmake --build build -j --target DisklordzRompler_VST3`.

### Step 3 — Audio core (commit: test)

- One tone, one shot or loop, **synthetic placeholder sample** generated at prepare (or single embedded test wav in `Assets/` you own) until pack loader exists.
- MIDI note → voice → stereo out. `DisklordzRomplerTests` with peak/RMS sanity.

### Step 4 — Parameters & state

- APVTS layout from `Docs/PARAMETER_MAP.md` (macros + master first).
- Factory preset index 0: "Init" / "Midnight Bell" stub parameters only.

### Step 5 — UI vertical slices (one tab per commit when possible)

1. **MAIN** — 8 macros, tone layer bars (UI only if engine still single-layer), master meter/knob, keyboard strip optional P1.
2. **BROWSER** — preset list + category filters (data from factory preset table; no 125 fake presets).
3. **TONE / PATCH / MOD / FX / PERF / SETTINGS** — P1 unless spec moves them to P0.

After **each** slice: build, run `ctest -R DisklordzRompler` if tests exist, pluginval when VST3 exists.

## P0 feature boundary

| In P0 | P1+ |
|-------|-----|
| CMake VST3 + Standalone | Full 125-preset library |
| 1–4 tone architecture (≥1 tone audible) | HLAC / streaming from Disklordz SaaS packs |
| 8 macros mapped to real DSP params | Full mod matrix UI |
| Global filter + amp ADSR | All insert FX modules |
| Preset list (≥8 factory entries) | PERF multi-part, PATCH matrix editor |
| MAIN tab faithful to mock (simplified) | SETTINGS, full browser filters |

## Out of scope for agent unless user asks

- Shipping proprietary sample libraries in git  
- HISE project export as the implementation  
- SaaS integration / download auth (document hooks only)  
- macOS notarization / installers  

## Success criteria

- [ ] Product `ARCHITECTURE.md` + `PRODUCT_SPEC.md` in repo  
- [ ] `DisklordzRompler_VST3` builds from root CMake  
- [ ] `ctest -R DisklordzRompler` passes  
- [ ] pluginval strictness 5 pass on ROMPLER VST3  
- [ ] Screenshot or video of Standalone MAIN tab resembling mockup  
- [ ] Draft PR on `cursor/disklordz-rompler-*-9a2b`; do not merge  

## Security

- No API keys or sample pack licenses in repo.  
- If loading user samples from disk: validate paths, size limits, message-thread loading only.

## Deliverable

Commit + push incremental slices; update root `ARCHITECTURE.md` when the target is registered.
