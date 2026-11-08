# Synth & DSP primer (VST Preset Agent)

Quick reference for programming factory banks in this monorepo.

## Oscillators

| Engine | Model | Key params |
|--------|--------|------------|
| **WAVE-9090** | Wavetable morph + scan | `wtPosition`, `wtScan`, `subLevel`, `detuneCents` |
| **Plugg VA** | Dual polyBLEP + unison | `osc1Wave`, `osc2Octave`, `unisonAmount`, `subLevel` |
| **JD Upgraded** | 4× PCM tones + coupling | `FactoryToneLayer`, `ToneCouplingMode` |

**Trap bass:** favor sub oscillator, low `filterCutoff`, slow attack, high sustain, light drive/crush for 808 tails.

**Plugg pluck:** near-zero sustain, fast decay, filter envelope amount high, short glide (25–45 ms).

## Filters

- WAVE / Plugg: normalized cutoff 0–1 into ZDF or biquad ladder; `filterRes` 0.2–0.5 for plucks, lower for subs.
- **`filterDestructive` (WAVE):** phonk/Memphis lane — combine with `crush` and `drive`.

## Envelopes

- Amp: trap leads use moderate sustain; plucks `sustain ≈ 0`.
- Filter env: positive `filtEnvAmt` with short decay = pizzicato pluck.

## Wavetable design (WAVE-9090)

- Tables **0–63:** classic harmonic morph (bright → harsh).
- Tables **64–113:** trap archetypes (808 sine stack, reese, cowbell FM, drill stab, etc.).
- `wtPosition` 0–1 spans the full bank; trap presets often sit **≥ 0.5** to favor new tables.

## Effects metaphor

| User term | Map to |
|-----------|--------|
| Chorus / wide plugg | `chorus`, `stereoWidth`, `unisonSpreadCents` (Plugg) |
| Tape / screw | `tapeWobble` (WAVE) |
| Bitcrush phonk | `crush`, `drive`, `filterDestructive` |

## SP-1200 kits

Programs tune **filter + 16 pad** level/tune/decay — not ROM swap. Memphis/trap kits: long decay on pads 0–3, tuned kicks down 1–2 octaves.

## Quality bar

Every bank expansion ships with **verify_factory_preset_counts.py** and product `ctest` green.

**Daily batch (8 per synth):** role-locked gold templates — 2×808, 2×pluck, reese, lead, phonk/drill (WAVE) — scored **≥ 88** before promotion (`scripts/vst/preset_quality.py`). No random jitter; producer-style names only.
