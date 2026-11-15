# JD Upgraded — full panel mockup v1 (Sam)

Target **shipping layout** for the native `dl_ui` skin (Canva/Stitch export). DSP/APVTS ids are fixed; this doc maps regions to `componentID` / parameter ids.

**Reference:** `shared/disklordz_ui/skins/jdupgraded_default/assets/reference/jd-upgraded-workstation-mockup-v1.jpg`

## Frame

| Property | Value |
|----------|--------|
| Art direction | Dark rack + wood end cheeks, orange active (`~#FF5500`) |
| Reference image size | **1024×819** px (`jd-upgraded-workstation-mockup-v1.jpg`) |
| `layout.json` target | Set `window` to **1024×819** when Sam’s `background.png` is committed (scaffold today: 1020×780) |

**Stitch Canvas:** [Project 4458054159206462354](https://stitch.google.com/projects/4458054159206462354) (pinned in `.stitch.json` + `.stitch/manifest.json`).

Stitch: `deviceType: DESKTOP`, prompt cites `.stitch/DESIGN.md` + this file.

## Header

| UI | APVTS / `componentID` |
|----|------------------------|
| Patch name + number | `presetName`, factory program index (editor) |
| ◀ ▶ | `presetPrev`, `presetNext` |
| IMPORT / EXPORT | `importPreset`, `exportPreset` (editor buttons) |
| MASTER GAIN | `masterGain` |
| Stereo LED meters | `outputMeter` (code-driven) |
| BEND RNG | `pitchBendRange` |
| EXP → FLT | `expressionDepth` |

## TONES (×4)

| Per tone | IDs |
|----------|-----|
| ON / MUTE | `tone{N}Enable` or `tone{N}Mute` (polarity per wiring) |
| Level fader | `tone{N}Level` |
| Wave mini display | `tone{N}Wave` (graphic optional PNG overlay) |
| PALETTE / wave label | `tone{N}Wave` combo |
| MULTISAMPLE | `tone{N}Multisample` |
| CUTOFF / RES | `tone{N}Cutoff`, `tone{N}Resonance` |

## STRUCTURE / FILTER

| UI | ID |
|----|-----|
| COUPLING MODE | `couplingMode` |
| FILTER RES | `filterResonance` |
| LINK FILTER RES | `filterLink` |

## ENVELOPES

| UI | ID |
|----|-----|
| LINK ENV | `envelopeLink` |
| Tone tabs 1–4 | highlight when editing per-tone env (editor) |
| AMP ADSR | `ampAttack`, `ampDecay`, `ampSustain`, `ampRelease` (+ per-tone when unlinked) |
| FILTER ADSR | `filterAttack`, `filterDecay`, `filterSustain`, `filterRelease` |

## EFFECTS

| UI | ID |
|----|-----|
| Group A ON | `groupAEnable` |
| DRIVE / PHASER | `groupADrive`, `groupAPhaser` |
| Group B ON | `groupBEnable` |
| CHORUS / DELAY / SPACE | `groupBChorus`, `groupBDelay`, `groupBMix` |

## Performance / keyboard

| UI | ID / behavior |
|----|----------------|
| PITCH / MOD wheels | Optional v2 (not APVTS today); pitch via host MIDI |
| OCTAVE ± | `OnScreenKeyboardStrip` |
| Piano | `onScreenKeyboard` → `MidiKeyboardState` |

## Export list (for Sam)

1. `background.png` — full frame incl. wood cheeks (no live text).
2. `header_bar.png` — or bake into background.
3. Per-section chrome: `tones_panel.png`, `fx_panel.png`, `env_panel.png`.
4. Controls from [component library v1](UI_DESIGN_HANDOFF.md#jd-upgraded--hardware-component-library-v10-sam).
5. Optional: wood cheeks as separate `mat_wood_left.png` / `mat_wood_right.png`.

After export, update `layout.json` bounds to match 1:1 px coordinates and run `RenderJdEditor`.