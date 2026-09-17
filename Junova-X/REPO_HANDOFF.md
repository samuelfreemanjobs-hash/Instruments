# Junova-X — monorepo handoff

**Canonical path:** `Junova-X/` (this folder).  
**Legacy iPlug2 scaffold:** import reference only; **ship target is JUCE** (Disklordz default stack).

## Identity

| Field | Value |
|-------|--------|
| Product | **Junova-X** |
| VST3 | `JunovaX.vst3` |
| CLAP | `JunovaX.clap` (MVP — WO-2026-002) |
| AU | **Not planned** (no Apple dev hardware) |
| Plugin ID | `JnvX` / manufacturer `SmFr` |
| Stack | **JUCE** + VST3 + CLAP (CMake, same factory as JD Upgraded CI patterns) |
| JD Upgraded kernel | **No shared kernel** |

## Status (honest)

- iPlug2 reference: scaffold + Main/Diag UI + DSP modules exist elsewhere; **not DAW-tested**
- **JUCE port:** in progress via Work Orders `WO-2026-001` … `003`
- GTM: **$29 → $49**, **Windows demo** required at launch; landing repo `junova-x-landing` (separate)

## Target layout

```text
Junova-X/
├── REPO_HANDOFF.md
├── CMakeLists.txt          ← JUCE plugin targets
├── Source/                 ← processor, DSP, UI (ported)
├── docs/                   ← spec, QA, gap analysis
├── presets/                ← factory + user (48 MVP, expand later)
└── gtm/
```

## Presets (Creative Director)

- **MVP ship bank:** **48** factory presets (WO-2026-003)
- **Roadmap:** additional banks post-MVP (target >48 total product story)

## Build (target)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target JunovaX_VST3 JunovaX_CLAP
```

(Exact target names finalized in WO-2026-001.)

## Related

- [docs/junova-x-mvp-gap-analysis.md](docs/junova-x-mvp-gap-analysis.md)
- [docs/DISKLORDZ_PLUGIN_TRACKS.md](../docs/DISKLORDZ_PLUGIN_TRACKS.md)
- Airtable WOs: `disklordz/airtable/seed/work-orders-junova-2026.json`
