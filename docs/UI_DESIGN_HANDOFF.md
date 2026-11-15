# UI design handoff (Canva / Google Stitch → JUCE)

Sam designs the **visual** UI in Canva or Google Stitch. Engineering drops assets into `shared/disklordz_ui/skins/<plugin>/` and updates `layout.json`. **No DSP or APVTS ID changes** are required for a skin refresh.

## Google Stitch Loop (optional)

For multi-screen DESKTOP mockups and codebase-aware prototyping (not committed skin PNGs):

1. Install CLI (Node 20+): `npm install -g @google/stitch` (or user prefix `~/.npm-global` on Cloud Agents).
2. Enable Loop experiment: `stitch enable loop` (sets `yolo.loop: true` in `~/.stitch.json`; adds `flows/prototype.md` to the bundled `/stitch` skill).
3. Project playbooks (pick one):
   - `stitch agent-skills add` → `.agents/skills/stitch/` (Cursor / Agent Skills)
   - `stitch agent-skills add --user` → `~/.agents/skills/` (every repo on this machine)
   - `stitch agent-skills add --dir .claude/skills/stitch` → Claude Code in this repo
   This repo already vendors `.agents/skills/stitch/`.
4. Sign in and run onboarding: `stitch status --flow=loop --json` → follow `.agents/skills/stitch/stitch/flows/prototype.md`.
5. Native JUCE skin path here remains **`shared/disklordz_ui`** + Canva/Stitch **exported PNGs** per sections below; Loop is for canvas iteration before export.

**JD Upgraded Stitch project:** [Canvas 4458054159206462354](https://stitch.google.com/projects/4458054159206462354) · `.stitch/manifest.json`

MCP (Cursor): add `stitch` server in `.cursor/mcp.json` with `X-Goog-Api-Key` from [Stitch settings](https://stitch.withgoogle.com) (no keys in git).

## Canvas / frame

| Export | JUCE mapping |
|--------|----------------|
| **Frame size** (`width` × `height`, px) | `layout.json` → `window.width` / `window.height` |
| **Background** (full panel, no controls) | `window.background` asset name + PNG in `skins/.../assets/` |
| **Safe margin** | Leave ~8–12 px from frame edges (layout can inset controls) |

Default JD Upgraded scaffold: **1020 × 780** (`skins/jdupgraded_default/layout.json`).

## JD Upgraded — Hardware Component Library v1.0 (Sam)

**Full panel mockup v1:** [JD_UPGRADED_UI_MOCKUP_V1.md](JD_UPGRADED_UI_MOCKUP_V1.md) · image `shared/disklordz_ui/skins/jdupgraded_default/assets/reference/jd-upgraded-workstation-mockup-v1.jpg`

**Component library v1.0** (knobs/meters/buttons):  
`shared/disklordz_ui/skins/jdupgraded_default/assets/reference/jd-upgraded-component-library-v1.jpg`

This is the **export target** for Canva/Stitch: skeuomorphic rack hardware, orange active glow (~`#FF8C00`–`#FF9A2E`; align with `.stitch/DESIGN.md` primary `#D98838` if you want one token set), dark navy chassis, brushed metal knobs.

| Region on sheet | Export as (PNG) | `layout.json` / code notes |
|-----------------|-----------------|---------------------------|
| **Knobs** Master 48px / Param 38px / Trim 28px | `knob_master_48_strip.png`, `knob_param_38_strip.png`, `knob_trim_28_strip.png` (64-frame filmstrips + default/hover/active stills) | Map **Master** → `masterGain`; most rotaries → 38px param; small trims → 28px |
| **Preset browser** recessed LCD | `display_preset_lcd.png` (static bezel) + optional dot-font atlas | Live text stays JUCE (`presetName` label); export bezel only |
| **Scope / analyzer** (if used) | `display_scope_bezel.png` | Optional; JD scaffold uses level meter not scope today |
| **Vertical faders** | `fader_track.png`, `fader_cap.png`, `fader_scale.png` | Tone levels today are vertical sliders — match 56×120 slots in layout |
| **Jog / pitch / mod wheels** | optional for v2 | Not in current APVTS; skip unless product adds performance controls |
| **LED meters** L/R | `meter_led_strip.png` (green→amber→red segments) | `outputMeter` uses code-driven level; PNG is backing scale |
| **Pushbuttons** ON/BYPASS/MUTE/SOLO + slope pills | `btn_toggle_off.png`, `btn_toggle_on.png`, `btn_filter_slope_*.png` | Map toggles to `groupAEnable`, mutes, `filterLink`, etc. |
| **Materials** wood cheek, felt, anodized trim | `mat_wood.png`, `mat_felt.png`, `mat_brushed_alu.png` | Optional panel accents in `background.png` composite |

**States to export per interactive family:** default, active (orange indicator), hover (outer glow), inactive/grayed (see fader “INACTIVE” on sheet).

## Layer exports (PNG)

Export **one PNG per static layer** (no embedded text that JUCE will draw live):

1. `background.png` — full panel art (optional until first skin pass).
2. Section chrome (optional): `section_osc.png`, `section_filter.png`, etc.
3. **Meter graphics** (optional): scale background; live value still computed in code.
4. **Keyboard well** (optional): area under keys; keys remain `MidiKeyboardComponent`.

Use **sRGB**, 1× resolution (or provide `@2x` siblings and set scale in a future layout field).

SVG: supported when added to BinaryData; prefer PNG filmstrips for knobs until SVG path is enabled in `SkinAssetCache`.

## Knobs (rotary)

Choose one pattern per control family:

| Pattern | Export | `layout.json` |
|---------|--------|----------------|
| **Filmstrip** (recommended) | Single vertical PNG, N frames stacked | `"filmstrip": "knob_master_strip.png"`, `"filmstripFrames": 64` |
| **Single image + code** | One knob cap PNG | `"filmstrip": "knob_cap.png"`, `"filmstripFrames": 1` |

Filmstrip height = `frameHeight × filmstripFrames`. Value 0 = bottom/off, max = top/full.

## Fonts

- List families in handoff (e.g. Inter, JetBrains Mono).
- If Sam’s design uses a custom font, ship **OFL/TTF** under `shared/disklordz_ui/skins/<skin>/assets/fonts/` (mirror `vst/factory-lib/gui/fonts`).
- Body copy **≥ 12 px**; knob legends **≥ 11 px**.

## `layout.json` control entries

Each control needs:

- **Key** = JUCE `componentID` (must match APVTS / widget binding).
- `type`: `knob` | `slider` | `button` | `combo` | `label` | `meter` | `keyboard` | `presetBar`
- `x`, `y`, `w`, `h` (px, origin top-left)
- `legend` (optional on-canvas hint; live value text stays in JUCE for now)
- `tooltip` (required for user-facing parameters)
- `filmstrip` / `filmstripFrames` for `knob` types

CMake embeds skin files via `dl_ui_attach_skin(...)` → `JDUpgradedSkinBinaryData` (see `shared/disklordz_ui/CMakeLists.txt`).

## JD Upgraded — parameter / control IDs

Use these IDs in Canva overlays and in `layout.json`. APVTS strings must stay **unchanged** for saved projects.

### Master / preset / FX

| ID | UI label (suggested) | Notes |
|----|----------------------|-------|
| `masterGain` | Master | Output level |
| `presetPrev` / `presetNext` / `presetName` | Program | Factory program browser (not APVTS) |
| `filterResonance` | Filter Res | Global resonance |
| `filterLink` | F-Link | `filterLink` APVTS |
| `couplingMode` | Coupling | Choice parameter |
| `groupADrive` | Grp A Drive | |
| `groupBMix` | Grp B Space | |
| `groupBChorus` | Chorus | `groupBChorus` |
| `groupAEnable` | Group A On | |
| `groupBEnable` | Group B On | |
| `groupAPhaser` | Phaser | |
| `groupBDelay` | Delay | |
| `expressionDepth` | Expression | CC1 → filter |
| `pitchBendRange` | Bend Range | Semitones |
| `onScreenKeyboard` | Keyboard | Click-to-play, octave ± |

### OSC (tones 1–4)

| ID | Tone |
|----|------|
| `tone{N}Level` | Level |
| `tone{N}Wave` | Wave index |
| `tone{N}Multisample` | Multisample set |
| `tone{N}Mute` | Mute |
| `tone{N}Cutoff` | TVF cutoff |
| `tone{N}Resonance` | TVF resonance |

`N` = 1…4.

### AMP ENV (global + per-tone when unlinked)

| Global ID | Per-tone ID pattern |
|-----------|---------------------|
| `ampAttack`, `ampDecay`, `ampSustain`, `ampRelease` | `tone{N}AmpAttack`, … (via `toneEnvelopeParamId`) |

### FILTER ENV

| Global ID | Per-tone pattern |
|-----------|------------------|
| `filterAttack`, `filterDecay`, `filterSustain`, `filterRelease` | `tone{N}FilterAttack`, … |

### Links

| ID | Meaning |
|----|---------|
| `envelopeLink` | Single envelope set for all tones |
| `filterLink` | Global filter resonance to all TVFs |

### ROM / preset I/O (editor only, optional on skin)

`exportPreset`, `importPreset`, ROM browser widgets — label in mockups only unless given IDs in a future layout revision.

## After export

1. Drop PNGs in `shared/disklordz_ui/skins/jdupgraded_default/assets/`.
2. Register filenames in `layout.json` (`background`, `filmstrip` fields).
3. Linux: `./build/RenderJdEditor /opt/cursor/artifacts/jd-upgraded-editor.png`
4. CI: `ctest -R JDUpgradedMidi` + pluginval strictness 5 (Windows job uploads VST3).

## Reuse (SP-1200, etc.)

Add `skins/sp1200_default/layout.json`, call `dl_ui_attach_skin(SP1200 ...)` from that product’s CMake, and link `dl_ui`. Same handoff rules apply.