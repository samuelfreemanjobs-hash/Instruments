# PlaySynth 6-pack — shared UI + performance contract (v2)

## APVTS (all six plugins)

| ID | Role |
|----|------|
| `macro1` … `macro8` | Plugin-specific macro fan-out (control rate, 16 samples) |
| `layer_blend` | A/B equal-power (also driven by `xy_x`) |
| `xy_x` | 0..1 blend override |
| `xy_y` | 0..1 cutoff + space axis (per-plugin mapping below) |
| `dice_trigger` | Momentary; UI thread only |
| `macro_lock` | When true, `setCurrentProgram` keeps current macro values |
| `arp_enable` | Tempo-sync arp on held notes |
| `arp_mode` | Up, Down, UpDown, Random |
| `arp_rate` | 1/8, 1/16 |
| `chord_mode` | Off, Octave, Minor7, Minor9, DarkTriad |
| `glide_ms` | Portamento (phonk SKU: default higher, legato-aware) |

## XY pad (WebView → processor)

On drag: set `xy_x`, `xy_y`. Processor maps:

- `effectiveBlend = mix(xy_x, layer_blend)` (see template `effectiveBlend()`)
- `xy_y` scales ladder cutoff + reverb/delay send per plugin table

### `xy_y` mapping (implement in each engine)

| Plugin | Y low → high |
|--------|----------------|
| AnalogDreams84 | Cutoff darker → plate verb wet |
| CloudSupplyTrap | Cutoff → shimmer wet |
| LoFiGlowTape | Tape LPF → spring/hall |
| PhonkCowbell94 | Dark LPF → crypt verb |
| FortyKeysSoul | Underwater LPF → late-night space |
| SoulSessions70 | Tone LPF → spring reverb |

## Performance engine (block edge, not inside `renderNextBlock`)

1. Optionally expand held MIDI with `chord_mode` before voice allocation.
2. If `arp_enable`, output arp pattern at host BPM for held note set.
3. Apply `glide_ms` on overlapping legato note changes.

No heap alloc in this path (fixed note stack, max 8 held notes).

## Dice randomizer (message thread)

`randomizePatchBounded(juce::Random&)`:

- Randomize: layer wave/preset indices, filter cutoff (log), macro1–8 within [0.15, 0.85]
- **Never** change: `master_level`
- Clamp peak by previewing macro Dirt/Warmth caps
- Push APVTS via `setValueNotifyingHost`

## WebView live scope

- Processor writes mono peak per block into `ScopeRingBuffer` (`templates/.../ScopeRingBuffer.h`).
- Editor timer (~30 Hz) calls `readScopePeaks()` and paints HTML canvas — **UI thread only**.

## Oversampling

Nonlinear paths (Tube Dirt, cowbell FM, tape sat): `juce::dsp::Oversampling<float>` **2x** in `prepareToPlay`.

## Velocity → macros (optional)

Scale macro 5–6 depth by `midiVelocity/127` at control rate when note-on fires (not per-sample).

## Factory batch

- Compact queue: `disklordz/vst-factory/data/play_synth_6pack_master.json`
- Contract block: `disklordz/vst-factory/data/play_synth_factory_contract.md`
- Windows runner: `disklordz/vst-factory/scripts/run-factory-play6.ps1`