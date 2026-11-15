# Nebula6 — design spec (APC Design)

**UI framework:** `webview`  
**Theme:** `nebula_obsidian_cyan_violet` v1 (contract: `plugins/PLAY_SYNTH_SHARED_UI.md`)  
**Inputs:** [creative-brief.md](creative-brief.md) · [parameter-spec.md](parameter-spec.md) · [architecture.md](architecture.md)

Build layout: **repo root** `Nebula6/` (fork `templates/juce8-synth-base` at impl).

## 1. Window

| Property | Value |
|----------|-------|
| Size | 920 × 620 px, fixed (resizable OFF v1) |
| Background | `bg.chassis` role → obsidian gradient (Assets CSS) |
| Padding | 12 px |
| UI scale | APVTS `ui_scale` 0.85–1.25 (default 1.0) |
| Keyboard | `EDITOR_WANTS_KEYBOARD_FOCUS TRUE` (WebView captures drag; host MIDI still on) |

## 2. Layout

```text
┌────────────────────────────────────────────────────────────────────────────┐ 920
│  NEBULA-6          [ Cloud ▾ ]  [♪ MIDI]  [🎲]     Master ──○──────── 0.78  │ 52
├──────────────┬─────────────────────────────────────┬─────────────────────────┤
│ CHORUS       │         NIGHT SKY XY                │  PERFORMANCE            │
│  I II Drift  │    Y Sky (chorus/shimmer/nebula)    │  Scale: Cm ▾           │
│  [Frozen]    │              ·  ●                   │  Arp ☑  Mode UpDown    │
│  Depth ──○   │    X Darkness (cutoff/hollow)      │  Chord Min7 ▾          │
│              │                                     │  Glide ──○  120ms       │
├──────────────┼─────────────────────────────────────┼─────────────────────────┤
│ 106 CORE     │  MACROS 1–4 (Sky Drift Hollow Neb) │  MACROS 5–8             │
│ Saw ──○      │  ┌──┬──┬──┬──┐                     │  Tone Body Space Weight │
│ Pulse ──○    │  │1 │2 │3 │4 │  SVG knobs          │  ┌──┬──┬──┬──┐          │
│ Sub ──○      │  └──┴──┴──┴──┘                     │  │5 │6 │7 │8 │          │
│ Cutoff ──○   │                                     │  └──┴──┴──┴──┘          │
│ Res ──○      │  PRESET ◀ Cloud Nine Juno ▶  3/13 │  Plane blend ──○        │
├──────────────┴─────────────────────────────────────┴─────────────────────────┤
│ ▁▂▃▄▅▆▇█ scope FIFO (mono peak, 30 Hz poll)                               │ 48
└────────────────────────────────────────────────────────────────────────────┘ 620
```

### 2.1 Regions

| Region | Rect (x,y,w,h) | Role |
|--------|----------------|------|
| Header | 0,0,920,52 | Logo, preset tag filter, MIDI export, Dice, master |
| Chorus strip | 0,52,200,220 | **Remarkable** I/II/Drift/Frozen + depth/rate |
| XY pad | 200,52,420,220 | `xy_x`, `xy_y` |
| Performance | 620,52,300,220 | scale, arp, chord, glide |
| Core osc/filter | 0,272,200,300 | DCO + filter ADSR summary |
| Macros A | 200,272,420,180 | macro1–4 |
| Macros B | 620,272,300,180 | macro5–8 |
| Preset bar | 200,452,720,120 | program name, tag badge, plane blend |
| Scope | 0,572,920,48 | canvas from `ScopeRingBuffer` |

## 3. Control map (high-signal)

| Widget | APVTS ID | Notes |
|--------|----------|-------|
| Chorus mode buttons | `chorus_mode` | enum 0–3 |
| Frozen toggle | `frozen_latch` | latched LED cyan |
| XY pad | `xy_x`, `xy_y` | pointer drag |
| Macro knobs 1–8 | `macro1`…`macro8` | Play SVG knob |
| Preset step | `setCurrentProgram` | host program change |
| Tag filter (UI only) | — | filters Dice + preset browser |
| MIDI export | `midi_export_trigger` | momentary |
| Dice | `dice_trigger` | tag-bounded random |
| Master | `master_level` | never randomized |

Full DCO/filter ids in parameter-spec use secondary “Advanced” drawer v1.1 (not in v1 surface).

## 4. States

| State | Visual |
|-------|--------|
| Normal | Obsidian panel, cyan accent on active chorus mode |
| Frozen on | Frozen button glow + subtle XY grid freeze animation |
| Arp on | Performance panel violet border pulse at host BPM (UI timer) |
| Macro lock | Padlock icon on macro row |

## 5. WebView bridge

- `postMessage` from JS: parameter id + 0–1 value → APVTS `setValueNotifyingHost`
- C++ timer: push scope peaks + chorus mode to JS
- No inline imports in processor; Assets `app.js` owns UI logic

## 6. Accessibility / interaction

- Tab order: chorus strip → XY → performance → macros → master
- Tooltips: macro names **Sky, Drift, Hollow, Nebula** + one-line genre hint
- Chorus strip always visible (hero motion discoverability)

## 7. Assets deliverables (impl)

| File | Content |
|------|---------|
| `Assets/index.html` | Regions above |
| `Assets/style.css` | Obsidian/cyan/violet tokens |
| `Assets/app.js` | XY, chorus strip, dice bounds by `preset_tag` |
| `Assets/presets/factory.xml` | 13 programs + tag metadata |

## 8. Acceptance (design gate)

- Wireframe shows **Frozen** and **Chorus I/II/Drift** without menu diving
- Every `macro1`–`macro8` mapped
- Hero motion path documented in header tooltip: “Hold chord → Frozen → Drift”