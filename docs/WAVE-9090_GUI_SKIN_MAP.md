# WAVE-9090 — GUI skin map (Sam v1 mockup → v1 APVTS)

Design reference:

- Mockup: [`Wave9090/Docs/design/wave9090-mockup-v1.png`](../Wave9090/Docs/design/wave9090-mockup-v1.png)
- Components: [`Wave9090/Docs/design/wave9090-component-sheet-v1.png`](../Wave9090/Docs/design/wave9090-component-sheet-v1.png)
- Tokens/spec: [`Wave9090/Docs/design/WAVE-9090_DESIGN_SYSTEM.md`](../Wave9090/Docs/design/WAVE-9090_DESIGN_SYSTEM.md)
- Stitch: https://stitch.google.com/projects/14288623377553782524/screens/51789caf2f05415fa837622f01f1bdab
- Unbuilt sections: [`WAVE-9090_V2_ROADMAP.md`](WAVE-9090_V2_ROADMAP.md)

## v1 layout (SYNTH MAIN tab only)

Code: `Wave9090/Source/UI/{DesignTokens,UiChrome,LayoutConstants,Wave9090LookAndFeel,*}.` + `PluginEditor.*`  
Assets: `Wave9090/Assets/skin/*.png` (BinaryData via `SkinAssets`)

## Mockup section → control → APVTS ID

| Mockup region (Sam) | v1 control in editor | APVTS / state ID | Host range |
|---------------------|----------------------|------------------|------------|
| **Header** logo | `brandTitle_` / `brandSub_` | — | — |
| **Header** preset browser | `presetPrev_` / `presetBox_` / `presetNext_` | `currentProgram`* | 0…7 programs |
| **Header** LED output | `ledMeter_` | (read `getOutputPeakDb()`) | dBFS display |
| **Header** PANIC | — | — | **v2** `panic` (see roadmap) |
| **Tabs** SYNTH MAIN | `tabSynthMain_` (active) | — | — |
| **Tabs** others | `tabFuture1_`, `tabFuture2_` (labels only) | — | no fake modules |
| **OSC 1** wavetable scope | `wtScope_` + `wtPosition` knob | `wtPosition` | 0…1 |
| **OSC 1** position / scan | knobs | `wtPosition`, `wtScan` | 0…1 |
| **OSC 1** sub | knob | `subLevel` | 0…1 |
| **OSC 1** detune / semi | knob | `detune` | −50…+50 ct |
| **OSC 2 Transwave** | — | — | **v2** `osc2*` |
| **VCF 1** cutoff / res | knobs | `filterCutoff`, `filterRes` | 0…1 |
| **VCF 1** drive / saturation | knobs | `drive`, `crush` | 0…1 |
| **VCF 1** destructive circuit | `circuitButton_` | `filterCircuit` | bool |
| **VCF 2** multimode | — | — | **v2** `vcf2*` |
| **ENV 1 Amp** ADSR | knobs + curve | `ampAttack`, `ampDecay`, `ampSustain`, `ampRelease` | times / 0…1 |
| **ENV 2 Filter** ADSR + depth | knobs + curve | `filtAttack`…`filtRelease`, `filtEnvAmt` | times / 0…1 |
| **ENV 3** pitch/mod | — | — | **v2** `env3*` |
| **LFO 1 / 2** | — | — | **v2** `lfo1*`, `lfo2*` |
| **Mod matrix / ring** | — | — | **v2** |
| **Macro M1 Tape** | knob (maps mockup M3) | `tapeWobble` | 0…1 |
| **Macros M1–M4** (other) | — | — | **v2** `macro1`…`macro4` |
| **Output / width** | knobs | `stereoWidth`, `outputGain` | 0…1, −24…+12 dB |
| **Performance** glide | knob | `glide` | 0…500 ms |
| **Performance** octave | `octaveButtons_` | — (keyboard range UI) |
| **TAPE / OUT** bay | Tape / Width / Output knobs | `tapeWobble`, `stereoWidth`, `outputGain` | 0…1, dB |
| **Keybed** | `keyboard_` | `MidiKeyboardState` | MIDI |

**Keyboard labels (FL / Image-Line):** `keyboard_.setOctaveForMiddleC(5)` so **MIDI 60 displays as C5** (FL piano roll). With octave `0`, lowest key **MIDI 36** labels as **C3** (808-friendly bass start). Each ±1 octave button shifts ±12 semitones. `Layout::octaveZeroLowestMidi` + `midiKeyboardMiddleCOctave`.

\*Preset index stored in plugin state XML; UI calls `applyFactoryPreset(index)`.

## Colour domains (v1)

| Mockup domain | Parameters | Knob halo colour |
|---------------|------------|------------------|
| Cyan | OSC1 + ENV1 amp | `#00f0d0` |
| Amber | VCF1 + ENV2 filter + tape macro | `#ff9900` |
| Blue | Output / width | `#3877ff` |

## Swap-in art (no DSP)

1. Replace `Wave9090/Assets/skin/*.png`
2. Tune `DesignTokens.h` / `Wave9090LookAndFeel.cpp`
3. Adjust bay rects in `PluginEditor::layoutControls()`