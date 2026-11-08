# CHORD ENGINE — Product Requirements Document

**Version:** 0.1 (pre-alpha)  
**Master plan:** [DISKLORDZ_MASTER_PLAN.md](DISKLORDZ_MASTER_PLAN.md)  
**Full Cursor brief:** [DiskLordz_Audio_Master_Roadmap_for_Cursor.md](DiskLordz_Audio_Master_Roadmap_for_Cursor.md) (product + §5 detail)  
**Framework:** [DPF_TECHNICAL_ARCHITECTURE.md](DPF_TECHNICAL_ARCHITECTURE.md)

---

## Positioning

Create professional, genre-aware chord progressions in seconds for producers who do not want to study advanced theory.

**Signature:** Genre Intelligence — theory-based presets for Memphis Underground, Luxury Rap, Dark Trap, Soul/Quiet Storm, Drift Phonk (original rules; no copied artist progressions).

---

## Core features (v1)

| Module | Function |
|--------|----------|
| Key detection | Identify key from incoming MIDI (where host allows) |
| Scale engine | Major, minor, modes, custom |
| Chord library | Triads through altered extensions |
| Voicing engine | Inversions, spread, close |
| Progression builder | Up to 16 chords |
| Voice leading | Minimize leapiness between chords |
| Bass support | Matching bass line |
| MIDI export | File + live MIDI out |
| Randomizer | Controlled variations |
| Presets | Save/recall progressions |

---

## UI areas (v1)

1. **Key & Scale** — root + scale  
2. **Chord Browser** — types and extensions  
3. **Progression Grid** — arrange chords (≤16)  
4. **Voicing Studio** — inversion, octave, spread, bass  
5. **MIDI Export** — send to host / export file  

Prioritize reliability and speed over feature count in first release.

---

## Chord library (initial)

| Category | Types |
|----------|--------|
| Basic | maj, min, dim, aug |
| Seventh | maj7, min7, dom7, min7b5 |
| Extended | 9, maj9, min9, 11, 13 |
| Suspended | sus2, sus4, 7sus4 |
| Altered | b9, #9, #11, b13 |
| Special | add9, 6/9, slash chords |

---

## Example generation contract

**Input:** F minor · Dark Trap · 4 chords · Intermediate · Wide voicing  

**Output:** Valid scale degrees, chord qualities, inversions, voice-leading constraints → **actual MIDI note events** (not labels only).

Automated tests in `DPF-MIDI` must assert note sets and scale membership.

---

## Acceptance criteria (commercial readiness)

CHORD ENGINE is **not** shippable until all pass [executed] in CI or signed beta protocol:

- [ ] Musically valid chords and coherent progressions in all genre presets
- [ ] Honors selected key and scale
- [ ] Usable MIDI output in supported hosts
- [ ] DAW transport sync correct
- [ ] Plugin state save/restore
- [ ] MIDI export preserves notes and timing
- [ ] No audio dropouts/crashes under test matrix buffers/rates
- [ ] pluginval / real-world DAW smoke per matrix below
- [ ] Beta cohort (15–25) sign-off on workflow friction

---

## DAW test matrix

| DAW | Priority |
|-----|----------|
| FL Studio | Essential |
| REAPER | Essential |
| Ableton Live | High |
| Studio One | High |
| Cubase | High |
| Bitwig | Later |

**Risk:** VST3 MIDI generators behave differently per host — verify routing in each, do not assume parity.

---

## Automated tests (required)

- Unit: music theory (scales, chord spelling)
- Unit: progression generator invariants
- Integration: MIDI event counts and timestamps
- Plugin: scan, state restore, offline render where applicable
- Performance: CPU/memory benchmarks documented in release notes

---

## CHORD ENGINE Lite (post-launch)

Free tier: limited library, 4-chord progressions, basic key/scale, small genre set — no advanced voicing. Paid unlocks full feature set.

---

## Out of scope (v1)

- Audio processing FX chain
- macOS build (track separately)
- Copy protection beyond simple license keys
- Cloud account sync

---

## Delegation

| Work type | Owner |
|-----------|--------|
| C++ / JUCE implementation | Cursor Cloud (VST lane) |
| Business / sprint / beta roster | audio-business-copilot |
| CI regression | golden-wav-qa |
| Store / license server | ship-velocity + website when scoped |
