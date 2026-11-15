---
name: juce-stitch-gui
description: Design and build JUCE/VST3 plugin GUIs with Google Stitch, DESIGN.md, WebView relays, or native dl_ui skins.
---

# JUCE / VST3 Google Stitch GUI

You connect **Google Stitch**, **`.stitch/DESIGN.md`**, and JUCE UI targets.

## Pre-flight

1. Read **`.stitch/DESIGN.md`** (root `DESIGN.md` points here). No unapproved colors or #000000.
2. **Stitch Loop:** `stitch status --flow=loop --json` — CLI only for Loop/capture (see `.agents/skills/stitch/stitch/SKILL.md`).
3. **MCP (optional):** For Canvas in IDE, use `stitch mcp install --target cursor`; discover tool prefix via client `list_tools` when using MCP.
4. Lint: `npx -p @google/design.md designmd lint .stitch/DESIGN.md`.

## Integration modes (pick per product)

| Mode | When | Engineering path |
|------|------|------------------|
| **Native skin (Disklordz default)** | `JUCE_WEB_BROWSER=0`, JD Upgraded, factory scaffolds | Stitch/Canva → PNG + `layout.json` → `shared/disklordz_ui` ([docs/UI_DESIGN_HANDOFF.md](../../../docs/UI_DESIGN_HANDOFF.md)) |
| **WebView** | Product enables `juce::WebBrowserComponent`, `NEEDS_WEBVIEW2` | Stitch HTML → bundle + `WebSliderRelay` / `WebToggleButtonRelay` / `WebComboBoxRelay` |

Do not enable WebView on products explicitly on native skin unless the user requests migration.

## Stitch prompting (DESKTOP)

- `deviceType: DESKTOP`; fixed size (960×600 default, or product layout.json size).
- Prompt = **Theme** (cite DESIGN.md) + **Content** (sections, controls, bypassed states) + optional reference image.
- One major section per `edit_screens` / refine turn.
- `generate_variants`: `REFINE` for polish; `EXPLORE` + `LAYOUT` / `COLOR_SCHEME` for layout forks.

## WebView rules

- `Options::withResourceProvider`; `withNativeIntegrationEnabled` for `juce://` on Linux.
- Relays map 1:1 to APVTS ids; `WebControlParameterIndexReceiver` when indexing is required.
- CSS: `overflow: hidden`, `user-select: none`, `font-variant-numeric: tabular-nums`.

## Native C++ rules

- Treat Stitch output as **pixel + token spec** only; implement in `dl_ui` / `PluginEditor` without changing DSP.
- Confirm binding before `stitch config set project` or batch uploads.

## Delegation

Heavy variant loops and HTML dumps → `.cursor/agents/juce-stitch-gui.md` subagent. Parent receives: project/screen ids, DESIGN.md deltas, relay map or skin asset list.

## Related skills

- `disklordz-vst-ui` — Disklordz lane owner
- `stitch` — CLI playbooks