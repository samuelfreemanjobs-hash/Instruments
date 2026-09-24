# Milestone 1 — The playable instrument

**Product:** Night Circuit · **Format:** desktop **VST3**  
**Hosts (sign-off):** FL Studio, MPC Software (desktop)

## Intent (not a Rev2 clone)

Milestone 1 is **the playable instrument**, not a forensic **Prophet Rev2** emulation.

| In scope | Out of scope for M1 |
|----------|---------------------|
| **Subtractive workflow inspired by Rev2-class synths** (dual osc → filter → VCA, ADSRs, mono/glide) | Matching Rev2 hardware sysex, exact oscillator mix law, or “golden” A/B to real units |
| **Night Circuit identity:** synthetic **trap / vaporwave** timbres | Acoustic or sample-based presets (piano, guitar, strings, brass emulations) |
| **Factory roles:** Bass, Lead, Pad, Pluck/Keys, Synth (poly beds) | Orchestral or “real instrument” marketing |
| Playable in host: MIDI, presets, stable enough to write music | Full modulation matrix, third osc, CLAP, store packaging |

Sound-design and QA should judge presets on **synthetic usefulness in trap workflows**, not on “how Prophet-like” a patch is.

## Technical scope (M1 = verification Phases 1–2)

Aligned with [../../docs/NIGHT_CIRCUIT_VERIFICATION.md](../../docs/NIGHT_CIRCUIT_VERIFICATION.md):

1. **Baseline** — reproducible build, 1,028 bank integrity, VST3 loads, editor usable, Gate 1 host listen on real OS (not VM-only).
2. **Dependable** — voice/MIDI behavior, recall, safe realtime, no critical DSP defects; user-preset import hardened.

Preset **musical** curation (featured 50–100) starts during M1 listening but **ships as M1 complete** only when Gate 1–2 pass **and** sound-design signs featured foundations.

## Preset policy

- **All factory programs are oscillator-only synthesis** — no samples, no wavetables of acoustic sources.
- **Categories map to production roles**, not instrument families:
  - **Bass** — sub and low synthetic weight
  - **Lead** — hooks, mono-forward lines
  - **Pad** — slow synthetic atmosphere
  - **Pluck/Keys** — short stabs and key lines
  - **Synth** — poly chords and melodic beds (vaporwave / stack chords)

## Success statement (M1 done when)

- [ ] Producer can load **Night Circuit.vst3** in FL or MPC, browse presets, play MIDI, save/recall a session without crashes or stuck notes.
- [ ] Automated proof: CI + Night Circuit integrity job green on pinned JUCE/Linux baseline.
- [ ] No known silent/broken factory programs in **featured** set.
- [ ] Docs and GUI concept aligned with synthetic identity ([PRESET_CATEGORIES.md](PRESET_CATEGORIES.md), [NIGHT_CIRCUIT_GUI_SPEC.md](../design/NIGHT_CIRCUIT_GUI_SPEC.md)).

## After M1 (later milestones)

| Milestone | Focus |
|-----------|--------|
| **M2** | Preset quality at scale, GUI migration to spec, favorites/modified, performance routing |
| **M3** | Distribution — installers, signing, supported OS/DAW matrix, release checklist |

Legacy labels in older notes: “Milestone C/D” → see [NIGHT_CIRCUIT_PLAN.md](NIGHT_CIRCUIT_PLAN.md) mapping.
