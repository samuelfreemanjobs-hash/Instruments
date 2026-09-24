# Night Circuit — DESIGN.md (Stitch / agent export)

Agent-friendly design tokens for [Google Stitch](https://stitch.withgoogle.com) and Cursor MCP. Source of truth for layout: [NIGHT_CIRCUIT_GUI_SPEC.md](NIGHT_CIRCUIT_GUI_SPEC.md), [NightCircuit-GUI-Concept.svg](NightCircuit-GUI-Concept.svg).

## Canvas

| Token | Value |
|-------|--------|
| Reference size | 1280 × 820 px |
| Minimum size | 1120 × 760 px |
| Product | Desktop VST3 synth editor (JUCE, not web) |

## Color

| Role | Hex |
|------|-----|
| Graphite shell | `#10101A` |
| Panel card | `#272236` |
| Brand / knobs | `#B77AEC` |
| Primary text | `#F5EFFA` |
| Muted text | `#AA9DBB` |

Category accents (preset browser stripe): Bass `#E8A035`, Synth `#3ECFD4`, Lead `#F0C040`, Pad `#9B7ED9`, Pluck/Keys `#5ECF8A`.

## Regions

1. **Preset browser band** (full width): category, preset, search, prev/next, star/favorites, featured/favorites filters, active name + Modified badge, count line (`N Category presets · 1,028 total`), save-to-user.
2. **Oscillators & filter** card: osc levels, detune, mix, cutoff, res, filter env, key track, filter drive.
3. **Amp / filter envelopes** cards: ADSR rows.
4. **Performance & circuit** card: Poly 16 / Mono label, output, mono/legato/unison, circuit drive, spread, glide.

## Stitch prompt seed

> Dark graphite VST3 synth UI, violet accent `#B77AEC`, full-width preset browser on top with category dropdown, preset dropdown, search, prev/next, favorite star, featured and favorites toggles, large active preset title with optional Modified badge. Below: three left stacked cards (osc+filter, amp ADSR, filter ADSR) and right performance column. Minimum 1120×760, reference 1280×820. Trap/vaporwave Night Circuit branding — not generic DAW chrome.

Setup MCP: [../docs/STITCH_MCP.md](../docs/STITCH_MCP.md).
