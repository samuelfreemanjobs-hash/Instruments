# Groove Intelligence Engine (Milestone 3)

## Plan

**Starting point (Phase 0).** 48 passing tests. Rhythm lived inline in `generator.py`: two fixed electric-piano hits per bar, root + optional fifth in the bass, three fixed lead slots, and one `swing * 0.25` offset applied per note. No drums, no shared timing, no analysis.

**Design.** One shared `GrooveClock` per project turns *grid positions* (beats) into *performed positions*. Every generator asks the clock where an event lands, so all tracks share swing, accents, per-role pocket, section feel, and humanization. A drum *plan* (grid positions per voice) is computed first; bass reads its kick positions, so bass and kick land in the same slots.

| Module | Responsibility |
|--------|----------------|
| `groove.py` | `GrooveProfile` data model, seven profiles, section grooves, humanize presets, `GrooveClock` |
| `drums.py` | Drum plan (kick/snare/hats/clap/rim/perc/toms/crash), repetition, variation bars, fills, rendering to GM notes |
| `rhythm.py` | Bass (kick-locked, anticipations, passing tones), pad (sustain, anticipation, delayed entry), EP stabs |
| `melody.py` | Motif-based lead: 2-bar call + response, repeated per section kind, scale-locked, chord tones on strong beats |
| `analysis.py` | Groove analyzer (displacement, swing ratio, grid %, velocity spread, density, kick/snare/bass relationships, repetition, section variation, profile identification) |

**Compatibility.** Harmony, voicing, reharm, MIDI export, MPC500 export, and schema-v2 JSON are unchanged in shape. `Project.groove` is a new optional field (defaults load old files). Drums are a fifth track only when `tracks.drums` is true, so 4-track configs keep their track order and channels. Rhythmic output of existing configs changes (that is the point of this milestone); seeded determinism is preserved and tested.

**Risks.** Swing/humanize can push notes across bar, chord, or section boundaries → every offset is clamped to a fraction of the grid step and to the arrangement; notes are re-snapped to the PPQ grid. MPC500 export re-quantizes to 96 PPQ, so sub-tick microtiming is coarsened there (documented).

## Timing model

All positions are quarter-note beats. For an event at grid position `g` on role `r`:

1. **Quantize pull:** `g' = g + (snap(g) - g) * quantize_strength` (generated events are already on the grid).
2. **Swing:** if `g'` is the off-beat of a swing pair (`pair = 2 * swing_step`), it moves to `pair_start + pair * swing_percent / 100` (50 % = straight, 66.7 % = triplet).
3. **Deviation:** `role_offset[r] + section_push + humanize`, where humanize = `preset_timing * role_intensity * section_intensity * (0.7 * shared_slot_noise + 0.3 * role_noise)`. Shared noise is seeded by `(seed, 16th slot)`, so events in the same slot across tracks move together.
4. **Clamp:** `|deviation| <= min(0.4 * grid_step, 0.08)` beats; then `final = swung + deviation * (1 - quantize_strength)`, clamped to `[0, end)`.

Humanize presets (max timing in beats / velocity spread / duration variation): `tight` 0/2/0, `subtle` 0.008/4/0.03, `natural` 0.015/7/0.06, `loose` 0.025/10/0.10, `live` 0.04/14/0.15.
