# Phonk factory UI — design tokens (Stitch v1)

Derived from **Stitch mockups** in [references/gui/v1-stitch/](../references/gui/v1-stitch/). Primary reference frame: **LIVEN LOFI-12 layout** (`phonk-sequencer-v1-lofi12-layout.png`).

## Color

| Token | Hex (approx) | Use |
|-------|----------------|-----|
| `--bg-deep` | `#0a0c10` | App background |
| `--bg-panel` | `#12151c` | Module cards |
| `--bg-rack` | `#1a1d24` | Sidebar / chrome |
| `--accent-cyan` | `#00e5ff` | Primary actions, active steps, titles |
| `--accent-mint` | `#7dffb3` | Secondary active steps, glow accents |
| `--text-primary` | `#e8eef2` | Labels |
| `--text-dim` | `#6b7280` | Hints, inactive |
| `--border-glow` | `rgba(0, 229, 255, 0.35)` | Panel borders |

## Typography

| Token | Style |
|-------|--------|
| `--font-display` | Wide geometric sans (mockup: “PHONK FACTORY” title) — fallback: `Orbitron`, `Rajdhani`, system-ui |
| `--font-ui` | Monospace / LCD for BPM (`138.0`) — fallback: `IBM Plex Mono`, `ui-monospace` |
| `--font-label` | Small caps labels on faders (`FILTER`, `CRUSH`) |

## Shape & effects

- **Corner radius:** 12–16px modules; 8px step pads  
- **Glow:** `box-shadow: 0 0 12px var(--accent-cyan)` on active pad / PLAY  
- **Step grid:** 4×4 for 16 steps (large touch targets)  
- **Faders:** Vertical hardware sliders with LED column (optional CSS)

## Motion (future)

- Playhead pulse on active step  
- Waveform idle animation in Factory deck  

## Alternate frame (rack VST)

Second mockup (`phonk-sequencer-v1-rack-vst-layout.png`): rack ears, 20-step row, rotary knobs — use for **plugin/VST fantasy skin** or Phase 3, not v1 web sequencer (see [GUI_MOCKUP_SPEC.md](GUI_MOCKUP_SPEC.md)).
