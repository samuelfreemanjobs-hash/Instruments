# SysEx and patch import

JD Upgraded accepts **Roland JD-800 / JD-990**-style patch data for parameter import. No Roland ROM or wave dumps are loaded — only performance-related bytes are mapped to the plugin APVTS.

## Supported input

1. **Full SysEx** (`F0 41 … F7`) with model ID `0x3D` (JD-800) or `0x57` (JD-990) and command `12`, `23`, or `24`, containing a **384-byte** patch block.
2. **Raw 384-byte patch RAM** (JUCE MIDI SysEx without `F0`/`F7` wrappers).

Patch layout follows documented JD-800 structure:

| Region | Size |
|--------|------|
| Patch Common | 50 |
| Patch Effect | 46 |
| Patch Tone A–D | 72 × 4 |

## Mapped parameters

| JD source | APVTS |
|-----------|--------|
| Common patch level | `masterGain` |
| Common layer tone mask | `toneNMute` (inverted bit) |
| Effect distortion drive | `groupADrive` |
| Effect reverb level | `groupBMix` |
| Effect chorus level | `groupBChorus` |
| Effect phaser mix | `groupAPhaser` |
| Effect delay left level | `groupBDelay` |
| Group A distortion block switch | `groupAEnable` |
| Group B reverb block switch | `groupBEnable` |
| Tone waveform MSB/LSB | `toneNWave` (scaled to 0–255) |
| Tone TVA level | `toneNLevel` |
| Tone TVF resonance (max) | `filterResonance` |
| Tone pitch coarse | internal voice pitch |
| Per-tone TVF cutoff / resonance | `toneNFilterCutoff`, `toneNFilterResonance` |
| Each tone TVA / TVF envelope times & sustain | `toneN*` ADSR; globals mirror tone A; **Link** off after import |
| Tone pitch fine | per-tone fine cents (internal) |
| Tone pitch envelope L0–L2, T1–T3 | per-tone pitch multiplier envelope |
| Tone LFO1 rate + pitch sens | per-tone vibrato (triangle/sine family waveforms) |

Unmapped JD fields (coupling, multisample, LFO2) keep current values or factory defaults.

## Internal blob (`JDPR`)

`ApvtsBridge` also supports a small internal preset blob (`JDPR` magic) for host-side preset exchange experiments.

## Legal

Ship only the clean-room `JDUPGROM` in installers. User-provided SysEx is processed locally; you are responsible for having rights to any hardware dumps you import.
