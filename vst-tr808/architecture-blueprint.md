# TR-808 class drum machine — architecture blueprint

**Working title:** see [plugin-spec-mvp.md](plugin-spec-mvp.md) (shipping name TBD).  
**Stack:** iPlug2 + VST3 (same factory lane as Junova-X). **Windows-first.**

## Design intent

- **Sixteen voices**, circuit-class **synthesis engines** — **not sample playback**.
- MVP build order: **BD → SD → CH/OH → CP**, then metals, toms/congas, cymbal.
- **MIDI** in; **kit previous/next**; **CH/OH choke**; **no** onboard sequencer or pad grid in v1.

## Engine model (four circuit-class families)

Engines are shared abstractions; each voice maps to parameters and routing:

1. **Trigger + envelope** (percussive voices)
2. **Noise / blend** paths (snare, hats)
3. **Resonant / metallic** modes (cymbal, rim)
4. **Tonal / tom** decay chains

DSP Engineering outlines concrete classes per voice; Spec can advance MVP scope before every schematic is pasted.

## Plugin shell

- iPlug2 processor + minimal UI: kit name, prev/next, per-voice level trims (MVP).
- Preset = full kit parameter snapshot + kit index.

## QA (when code exists)

- Host smoke: Reaper + one other Windows DAW
- Choke: CH closes OH tail
- MIDI note map document in `docs/MIDI_MAP.md` (to add)

## Repo layout (target)

```text
vst-tr808/
├── architecture-blueprint.md
├── plugin-spec-mvp.md
├── README.md
└── plugin/          ← iPlug2 project (future)
```

## Priority vs Junova-X

**Junova-X:** first **host build + repo land** on `Instruments`.  
**TR-808:** Spec + DSP skeleton **in parallel**; no 808 implementation WO until Junova-X Release|x64 smoke passes or you explicitly override.
