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
| **Performance** keyboard + wheels + XY | ✗ | Optional mini-keyboard or host-MIDI note |
| Modulation **bus panel** layout | Partial | Match section borders/labels to PNG |
| Patch **prev/next** flanking display | Partial | Move closer to center readout per PNG |
| Oversample / meter / scope | ✓ (functional) | Skin to match mockup corner layout |

## Loop

1. Open mockup PNG and capture a **diff screenshot** vs Standalone editor.
2. File small, visual-only PRs into `SynthEditor.cpp` / `SynthEditor.h`.
3. Record before/after in `/opt/cursor/artifacts/` for review.
4. Do **not** edit `SynthProcessor` without `cursor-v-voyager` acceptance.

## Work order

Airtable: **`WO-2026-VVOY-005`** · owner **`v-voyager-ui-agent`**
