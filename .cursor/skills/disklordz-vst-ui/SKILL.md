# Disklordz VST UI agent

Orchestrate **skinnable JUCE editor** work and **Google Stitch** design loops. DSP and audio correctness belong to Audio Plugin Coder or the product implementer.

## Read first

| Doc | When |
|-----|------|
| `.stitch/DESIGN.md` | Colors, typography, density (lint before upload) |
| `docs/JUCE_STITCH_GUI_REFERENCE.md` | Stitch toolchain + native vs WebView |
| `docs/DISKLORDZ_VST_UI_AGENT.md` | Session bootstrap |
| `docs/UI_DESIGN_HANDOFF.md` | Canva/Stitch → PNG + control IDs |
| `shared/disklordz_ui/ARCHITECTURE.md` | `dl_ui`, CMake `dl_ui_attach_skin` |
| `.agents/skills/stitch/stitch/SKILL.md` | Stitch CLI / Loop (call CLI only, not MCP) |

## You may

- Edit `shared/disklordz_ui/**`, `**/skins/**/layout.json`, `Source/PluginEditor.*` **layout/LNF only**
- Add skin BinaryData targets (`JDUpgradedSkin` namespace — never clash ROM `BinaryData`)
- Run `stitch status --flow=loop --json`, capture/upload/generate per stitch playbooks (confirm before `config set project` / batch upload)
- Run `RenderJdEditor`, extend `docs/UI_DESIGN_HANDOFF.md` control lists from `createParameterLayout()`
- Wire `dl_ui::LayoutEditorShell`, `OnScreenKeyboardStrip`, `SkinnableLookAndFeel`

## You must not

- Change `processBlock`, voice pools, filters, or APVTS parameter **IDs/ranges** (layout metadata and `componentID` only)
- Ship marketing-polished UI when Sam owns final pixels — deliver skinnable scaffold + handoff
- Commit API keys; Stitch auth is `stitch login` / env `STITCH_API_KEY` locally

## Workflow

1. List APVTS + editor `componentID` values; diff against `layout.json` and handoff doc.
2. If Sam uses Stitch: check flow state, DESKTOP device, fixed viewport; export PNG layers → `skins/<skin>/assets/`.
3. Implement: `applyLayout` last in `resized()`; register filmstrips in `SkinAssetCache`.
4. Evidence: MIDI smoke if processor touched; else `RenderJdEditor` PNG.

## Memory

`python3 scripts/agent_memory.py recall --agent vst-ui-agent …` at start; `add` + `check` before done.