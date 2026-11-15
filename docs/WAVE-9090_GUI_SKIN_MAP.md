# WAVE-9090 — GUI skin map (for Canva / Stitch)

The editor is a **placeholder skin** only. DSP lives in `Wave9090/Source/Engine/` and `Wave9090/Source/DSP/`. Skin lives under `Wave9090/Source/UI/` + `Wave9090/Assets/skin/` (BinaryData).

## Swap-in workflow (no DSP edits)

1. Replace PNGs in `Wave9090/Assets/skin/` (keep filenames or update `CMakeLists.txt` + `SkinAssets.cpp`).
2. Tune colours in `Wave9090LookAndFeel.cpp` (or replace with image-only draws).
3. Adjust rects in `LayoutConstants.h` / `PluginEditor::layoutControls()` to match Sam’s layout grid.

## Editor size (baseline)

| Constant | Value |
|----------|-------|
| Width | 960 (`Layout::editorWidth`) |
| Height | 640 (`Layout::editorHeight`) |
| Header | 56 px |
| Footer meter | 48 px |
| On-screen keyboard | 108 px |

## Controls Sam must place (APVTS IDs unchanged)

| Design slot | Widget type | APVTS parameter ID | Display name | Range (host) |
|-------------|-------------|-------------------|--------------|--------------|
| header.title | Label (static) | — | WAVE-9090 | — |
| header.preset | Preset browser / combo | `currentProgram`* | (factory name) | 0…7 programs |
| osc.wtPosition | Rotary + value | `wtPosition` | Wavetable | 0…1 |
| osc.wtScan | Rotary + value | `wtScan` | Scan | 0…1 |
| osc.sub | Rotary + value | `subLevel` | Sub | 0…1 |
| osc.detune | Rotary + value (bipolar) | `detune` | Detune | −50…+50 ct |
| osc.glide | Rotary + value | `glide` | Glide | 0…500 ms |
| filter.cutoff | Rotary + value | `filterCutoff` | Cutoff | 0…1 |
| filter.res | Rotary + value | `filterRes` | Res | 0…1 |
| filter.drive | Rotary + value | `drive` | Drive | 0…1 |
| filter.crush | Rotary + value | `crush` | Crush | 0…1 |
| filter.circuitToggle | Toggle | `filterCircuit` | Destructive | bool |
| env.ampAttack | Rotary + value | `ampAttack` | Amp A | 1 ms…2 s |
| env.ampDecay | Rotary + value | `ampDecay` | Amp D | 1 ms…2 s |
| env.ampSustain | Rotary + value | `ampSustain` | Amp S | 0…1 |
| env.ampRelease | Rotary + value | `ampRelease` | Amp R | 1 ms…5 s |
| env.filtAttack | Rotary + value | `filtAttack` | Filt A | 1 ms…2 s |
| env.filtDecay | Rotary + value | `filtDecay` | Filt D | 1 ms…2 s |
| env.filtSustain | Rotary + value | `filtSustain` | Filt S | 0…1 |
| env.filtRelease | Rotary + value | `filtRelease` | Filt R | 1 ms…5 s |
| env.filtAmt | Rotary + value | `filtEnvAmt` | F Env | 0…1 |
| macro.tape | Rotary + value | `tapeWobble` | Tape | 0…1 |
| out.width | Rotary + value | `stereoWidth` | Width | 0…1 |
| master.output | Rotary + value | `outputGain` | Output | −24…+12 dB |
| footer.outputMeter | Meter / bar | (read-only peak from processor) | OUTPUT | peak dBFS |
| play.keyboard | MIDI keyboard | — (routes to `MidiKeyboardState`) | — | full range |

\*`currentProgram` is stored in plugin state XML, not a public APVTS float; preset UI calls `applyFactoryPreset(index)`.

## Section groupings (placeholder columns)

| Column | Label | Parameter IDs |
|--------|-------|----------------|
| 1 | OSCILLATOR | `wtPosition`, `wtScan`, `subLevel`, `detune`, `glide` |
| 2 | FILTER | `filterCutoff`, `filterRes`, `drive`, `crush`, `filterCircuit` |
| 3 | ENVELOPES | `ampAttack`…`ampRelease`, `filtAttack`…`filtRelease`, `filtEnvAmt` |
| 4 | MACRO / OUTPUT | `tapeWobble`, `stereoWidth`, `outputGain` |

## BinaryData asset keys

| File | Role |
|------|------|
| `panel_bg.png` | Body background tile |
| `knob_dial.png` | Rotary face (optional; LAF draws pointer) |
| `header_strip.png` | Top bar strip |

## Factory program names (for preset UI copy)

1. Trap 808  
2. Phonk Lead  
3. Crisp Pluck  
4. Dark Bell  
5. Phonk Cowbell  
6. Memphis Keys  
7. Underground Stab  
8. Tape Pad  