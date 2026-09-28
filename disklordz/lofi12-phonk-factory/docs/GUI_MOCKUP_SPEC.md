# GUI mockup spec — Phonk Factory + Step Sequencer

**Status:** **LOCKED v1-stitch** (2026-09-28 product decisions)  
**SOP:** [docs/GUI_DESIGN_SOP.md](../../../docs/GUI_DESIGN_SOP.md) · **GUI Agent:** [docs/GUI_AGENT_PLAYBOOK.md](../../../docs/GUI_AGENT_PLAYBOOK.md)  
**Tokens:** [GUI_TOKENS.md](GUI_TOKENS.md)  
**Mockups:** [references/gui/v1-stitch/](../references/gui/v1-stitch/)

---

## Stitch frames (your deliverable)

| Frame | File | Role |
|-------|------|------|
| **A — Primary (web app target)** | [phonk-sequencer-v1-lofi12-layout.png](../references/gui/v1-stitch/phonk-sequencer-v1-lofi12-layout.png) | Sidebar + factory deck + vertical FX + 6 track tabs + 4×4 step matrix |
| **B — Alternate (VST fantasy)** | [phonk-sequencer-v1-rack-vst-layout.png](../references/gui/v1-stitch/phonk-sequencer-v1-rack-vst-layout.png) | Rack unit, 20-step row, rotaries — **not** v1 scope unless you choose it |

**Recommendation:** Implement **Frame A** first; borrow rack fader/meter styling from B if desired.

---

## Frame A — control map (mockup → behavior)

### Global header

| Mockup element | Maps to (current app / API) | Notes |
|----------------|----------------------------|--------|
| BPM `138.0` | `#bpm` | Mockup shows 138; Memphis presets often **84–86** — keep 60–190 range |
| PATTERN `01/16` | Future: pattern slot | **Phase 2** — store multiple JSON patterns |
| SWING `50%` | Future / `phonk_groove` swing | Optional slider later |
| **PLAY / STOP** | `#play` / `#stop` | |
| Output `LIVEN LOFI-12 (USB)` | `#midiOut` + Refresh | Web MIDI port name |
| Title + subtitle | Header copy | “STEM EXPORT, CASSETTE/BIT CRUSH, ZIP PACK” |

### Left sidebar (new chrome)

| Nav item | v1 behavior |
|----------|-------------|
| **STEP SEQUENCER** | Default view (current page) |
| SAMPLE CHOPPER | Placeholder → disabled or “Coming soon” |
| LOFI ENGINE | Link to FX panel scroll or sub-view |
| FX MATRIX | Same as Loop FX module |
| SYSTEM SETTINGS | BPM limits, MIDI, download toggles |
| MIDI MAP | Slot/note defaults per track |
| BANK PRESETS | `#stylePreset` + Apply |
| Bottom waveform `32.1 kHz` | Decorative or last-generated sample preview |

Sidebar is **navigation shell** only in v1 — no new backend until Phase 2.

### Phonk Factory deck

| Mockup | App id / API |
|--------|----------------|
| Preset `Memphis Insanity` | `#stylePreset` (map to `memphis_trinity` or custom label) |
| Waveform display | Visual only; optional: last loop waveform **Phase 2** |
| **GENERATE DRUM LOOP** | `#genNewLoop` → `/api/render_loop` |
| AUTO WAV EXPORT | `#autoDownloadWav` |
| LOOP LOOP | `#backingOn` (loop backing with transport) |
| *(not shown)* | `#genSamplePack` → `/api/generate_bank` — add button near Generate |
| *(not shown)* | `#groovePrompt`, Load groove, Export stems — keep in deck or System |

### Loop FX — vertical faders (global mix bus)

| Mockup label | App / `LoopFxParams` |
|--------------|----------------------|
| FILTER | `fxFilter` → `filter_cutoff` |
| CRUSH | `fxBitcrush` → `bitcrush` |
| DRIVE | `fxDrive` → `drive` |
| CASSETTE | `fxCassette` → `cassette` |
| GAIN | **New** — output trim post-FX (implement `gain` 0–1 in `phonk_loop_fx.py`) |
| MODE 2: PHONK | Preset indicator (Juicy/Paul/Toomp) |
| ANALOG GLITCH (ACTIVE) | Status when cassette+crush > threshold |

**Reverb / Wobble:** mockup A omits; keep in **FX MATRIX** sub-panel or “Advanced” row (Frame B has REVERB + WOBBLE).

### Track tabs

| Tab | Index | Stem id |
|-----|-------|---------|
| KICK [1] | 0 | kick |
| SNARE [2] | 1 | snare |
| HAT [3] | 2 | hat |
| COWBELL [4] | 3 | cowbell |
| PERC [5] | 4 | perc |
| OPENHAT [6] | 5 | openhat |

### Step sequencer matrix (per track)

| Mockup | Implementation |
|--------|----------------|
| 4×4 grid, 16 steps | `#singleTrackGrid` — CSS grid 4×4 |
| Step labels `1·110` | Slot·velocity on pad |
| VELOCITY slider 75% | `#editVel` + slider UI |
| APPLY TO STEP | `#applyEdit` |
| EXPORT TRACK (GATE) / (SAMPLE) | `#exportTrackGrid` / `#exportTrackFactory` |
| ALL TRACKS (6×16 OVERVIEW) | `<details>` master grid — fix copy: **6×16** not 8×16 |

### Footer hints

- Keyboard shortcuts line — match [GUI_AGENT](../sequencer/static/app.js) (Shift/Alt/1–6).

---

## Frame B — differences (do not blindly port)

| Element | Issue | Decision |
|---------|--------|----------|
| **20 steps** | Lofi-12 = **16** steps | Stay 16 for v1 |
| **TRACK / STACK** labels | Unclear vs 6 lanes | Defer |
| Knobs LEVEL/SLOT/MORPH | No API yet | Phase 2 or map to preset/variation |
| “STORE” | Save pattern? | Map to `#saveJson` |
| 7 faders incl. WOBBLE | Matches backend | Use in Advanced FX row |

---

## Visual system

See [GUI_TOKENS.md](GUI_TOKENS.md): charcoal base, **cyan + mint** accents, glow, LCD BPM.

---

## Product decisions (locked)

| # | Decision |
|---|----------|
| 1 | **Frame A** — primary web layout |
| 2 | **Sidebar** — disabled “coming soon” items; only Step sequencer active |
| 3 | **GAIN** — on mix bus + vertical fader |
| 4 | **Reverb / Wobble** — under **Advanced FX** |
| 5 | **Generate drum loop** — **no auto-play**; user presses Play (slow laptops) |
| 6 | **Preset display names** — fictional only (e.g. **Memphis Insanity**); **no real artist names** in UI. Internal prompt/lane ids unchanged for sound engine. |

---

## API contract (unchanged)

| Method | Path |
|--------|------|
| GET | `/api/presets` |
| POST | `/api/render_loop` |
| POST | `/api/render_pattern_stem` |
| POST | `/api/generate_bank` |
| POST | `/api/groove_pattern` |

---

## Implementation checklist (GUI Agent)

- [x] Layout shell: sidebar + main columns per Frame A  
- [x] Apply [GUI_TOKENS.md](GUI_TOKENS.md) in `sequencer/static/style.css`  
- [x] Factory deck + vertical FX faders + Advanced FX  
- [x] 4×4 step matrix for active track  
- [x] `gain` on mix bus  
- [ ] Screen recording + before/after screenshots in PR  

---

## Legacy wireframe (v0.2 code-first)

<details>
<summary>Pre-Stitch ASCII layout</summary>

Single-column web app: Transport → Factory → FX sliders → track tabs → pads. Still valid functionally; superseded visually by Frame A.

</details>
