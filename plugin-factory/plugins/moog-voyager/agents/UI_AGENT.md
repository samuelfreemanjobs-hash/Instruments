# UI Agent — V Voyager (`v-voyager-ui-agent`)

**Fleet agent:** `v-voyager-ui-designer`

## Your visual spec (source of truth)

**[../docs/UI_CONCEPT_VOYAGER-V.png](../docs/UI_CONCEPT_VOYAGER-V.png)** — Systems Engineering Audio **VOYAGER-V** panel (wood rails, metal face, large filter, performance surface).

Structured checklist: [`../docs/UI_CONCEPT.md`](../docs/UI_CONCEPT.md)

## Mission

Close the gap between the **mockup** and [`SynthEditor.cpp`](../SynthEditor.cpp) without changing DSP contracts unless paired with `cursor-v-voyager`.

## Current vs mockup (priority backlog)

| Mockup zone | Status | UI Agent next |
|-------------|--------|----------------|
| Wood rails + metal gradient | ✓ | Fine-tune proportions vs PNG |
| Large filter cutoff | ✓ | Match dial size/position to mockup |
| Four-stack + badge | ✓ | Align switch styling to PNG |
| Oscillator mix **faders** | ✓ | Match fader cap art from mockup |
| ADSR **vertical sliders** | ✓ | Match spacing/labels from mockup |
| **Performance** keyboard + wheels + XY | ✓ (wave 1) | Wire wheels/XY to mod bus (optional) |
| Modulation **bus panel** layout | ✓ (click matrix) | Wire matrix → APVTS (optional) |
| **Waveform** rotary switches | ✓ | SAW/SQR/TRI/PLS strips |
| **Filter KB track** stepped UI | ✓ | OFF / 1/3 / 2/3 / FULL |
| **Knob + fader** styling | ✓ | VoyagerPanelLookAndFeel |
| Large filter cutoff | ✓ (enlarged) | Position vs mockup |
| **OSC 2/3 SYNC** | ✓ | Hard-sync DSP |
| Oversample **2× / 4×** switches | ✓ | Segmented header buttons |
| XY / wheels **→ modulation** | ✓ | XY→LFO; M→filter env; P→osc fine |
| Patch **prev/next** flanking display | ✓ | ◀ PATCH ▶ centered |
| Oversample / meter / scope | ✓ | Master knob by scope; wood LEDs |

## Loop

1. Open mockup PNG and capture a **diff screenshot** vs Standalone editor.
2. File small, visual-only PRs into `SynthEditor.cpp` / `SynthEditor.h`.
3. Record before/after in `/opt/cursor/artifacts/` for review.
4. Do **not** edit `SynthProcessor` without `cursor-v-voyager` acceptance.

## Work order

Airtable: **`WO-2026-VVOY-005`** · owner **`v-voyager-ui-agent`**
