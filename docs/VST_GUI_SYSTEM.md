# VST GUI design system (monorepo)

**Owner:** `vst-gui-designer`  
**Applies to:** All JUCE `AudioProcessorEditor` implementations in this repository.

## Principles

1. **DSP / parameters / UI separation** — editors attach to APVTS only; no synthesis on the message thread beyond host notifications.
2. **Host-first** — layouts must survive FL Studio, MPC Software, and common DAW minimum plugin window sizes (document host matrix per product).
3. **Resizable by default** — new products declare reference + minimum size; fixed-size only with pm-agent approval.
4. **Accessible baseline** — named controls, readable contrast (WCAG AA target for primary text on panel), visible keyboard focus.
5. **Spec before pixels** — each product ships `design/*GUI_SPEC.md` + optional `DESIGN.md` (Stitch/agent export).

## Shared tokens (default dark synth)

| Token | Hex | Use |
|-------|-----|-----|
| `shell/graphite` | `#10101A` | Editor background |
| `panel/elevated` | `#272236` | Section cards |
| `accent/brand` | `#B77AEC` | Primary accent (product may override) |
| `text/primary` | `#F5EFFA` | Titles, values |
| `text/muted` | `#AA9DBB` | Hints, counts |

Night Circuit category accents remain product-specific (see `PRESET_BROWSER_GUI.md`).

## Preset browser pattern (when product has factory bank)

Required for multi-hundred+ preset products:

- Category + preset selection, text search, prev/next
- Count line: `{N} {Category} presets · {total} total`
- User bank on disk; favorites local; **Modified** when APVTS ≠ loaded preset baseline
- Featured filter optional (sound-design owns list)

## File conventions

| Artifact | Path |
|----------|------|
| Spec sheet | `{Product}/design/{PRODUCT}_GUI_SPEC.md` |
| Stitch/agent export | `{Product}/design/DESIGN.md` |
| Concept art | `{Product}/design/*Concept*.svg` |
| Audit log | `docs/reports/VST_GUI_AUDIT_*.md` |

## Review gate

`vst-gui-designer` signs off on GUI PRs using rubric in [VST_GUI_DESIGNER_AGENT.md](VST_GUI_DESIGNER_AGENT.md). Factory merges editor changes without sign-off only for emergency hotfixes with follow-up GUI WO.

## Tools

- **Google Stitch MCP** — mock iteration ([STITCH_MCP.md](../ProphetRev2Trap/docs/STITCH_MCP.md))
- **JUCE** — shipping UI
