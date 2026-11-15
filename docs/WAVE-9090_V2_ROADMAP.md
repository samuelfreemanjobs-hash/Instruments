# WAVE-9090 v2 UI/DSP roadmap (mockup sections not in v1)

Reference mockup: `Wave9090/Docs/design/wave9090-mockup-v1.png`. **v1 ships SYNTH MAIN only** with existing APVTS. Sections below need new parameters before UI controls are added.

## Deferred tabs (no chrome-only controls in v1)

| Mockup tab | Notes |
|------------|-------|
| WAVETABLE EDITOR | User import + draw tables |
| MODULATION MATRIX | Slots routing sources → targets |
| FX RACK | Post-voice FX chain |
| SEQUENCER / ARP | Step seq + arp patterns |

## OSC 2 / Transwave (amber right column)

| Proposed ID | Range | Purpose |
|-------------|-------|---------|
| `osc2Enable` | bool | Second oscillator on |
| `osc2WtPosition` | 0…1 | Transwave index |
| `osc2Formant` | 0…1 | Formant emphasis |
| `osc2PhaseMod` | 0…1 | Phase modulation depth |
| `osc2SubLevel` | 0…1 | OSC2 sub level |
| `osc2Detune` | cents | vs OSC1 |
| `osc2Level` | 0…1 | Mix |
| `osc2Waveform` | enum | Sine/Square/Pulse/Tri |

## FM / Ring / Link column

| Proposed ID | Range | Purpose |
|-------------|-------|---------|
| `fmOsc2To1` | 0…1 | FM depth |
| `ringModDepth` | 0…1 | Ring mod |
| `ringModEnable` | bool | Engaged |
| `noiseColor` | enum | White/Pink |
| `noiseLevel` | 0…1 | Noise oscillator |
| `hardSyncEnable` | bool | Sync OSC1→2 |

## VCF 2 (state-variable multimode)

| Proposed ID | Range | Purpose |
|-------------|-------|---------|
| `vcf2Enable` | bool | Second filter |
| `vcf2Cutoff` | 0…1 | Cutoff |
| `vcf2Res` | 0…1 | Q |
| `vcf2Mode` | enum | LP/BP/HP/Notch/Comb |
| `vcf2Damp` | 0…1 | Damping |
| `filterRouting` | enum | Serial/Parallel/Blend |

## ENV 3 (pitch/mod)

| Proposed ID | Range | Purpose |
|-------------|-------|---------|
| `env3Attack` … `env3Release` | time | Pitch/mod env |
| `env3Depth` | cents | Amount to pitch or wt |

## LFO 1 / LFO 2

| Proposed ID | Range | Purpose |
|-------------|-------|---------|
| `lfo1Rate`, `lfo2Rate` | Hz/sync | LFO speed |
| `lfo1Depth`, `lfo2Depth` | 0…1 | Amount |
| `lfo1Shape`, `lfo2Shape` | enum | Wave |
| `lfo1Phase` | 0…1 | Start phase |
| `lfo1Dest`, `lfo2Dest` | enum | Target param |

## Hardware macros M1–M4 (beyond `tapeWobble`)

| Proposed ID | Maps to (v2) |
|-------------|----------------|
| `macro1` | Multi-target morph |
| `macro2` | Resonance warp |
| `macro3` | Extends `tapeWobble` or companion |
| `macro4` | Space/reverb send |

## Performance / global

| Proposed ID | Purpose |
|-------------|---------|
| `panic` | All-notes-off + FX clear |
| `engineOversample` | 1x/2x |
| `voiceCount` | Max voices display target |
| `pitchBendRange` | Semitones |
| `modWheelDest` | Mod matrix quick route |

## Sequencer / arp (tab)

| Proposed ID | Purpose |
|-------------|---------|
| `arpEnable`, `arpRate`, `arpPattern`, `arpOctaves` | Arpeggiator |
| `seqSteps[16]`, `seqGate`, `seqSwing` | Step sequencer |

Implement in DSP first, then mirror mockup layout in `Source/UI/` without renaming existing v1 IDs.