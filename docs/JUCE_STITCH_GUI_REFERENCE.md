# JUCE, VST3, and Google Stitch — repo reference

Stitch outputs HTML/CSS, tokens, and Figma — not JUCE C++. Disklordz uses two production paths:

| Path | Products | Ship target |
|------|----------|-------------|
| **Native skin** | JD Upgraded (now), APC plugins with `dl_ui` | `shared/disklordz_ui` + BinaryData PNGs |
| **WebView** | Optional future plugins with `NEEDS_WEBVIEW2` | Bundled HTML + `Web*Relay` APVTS bridge |

## Design system

| File | Role |
|------|------|
| [`.stitch/DESIGN.md`](../.stitch/DESIGN.md) | Anodized Studio Rack tokens (Stitch + `@google/design.md`) |
| [`DESIGN.md`](../DESIGN.md) | Pointer + lint/upload commands |
| [`docs/UI_DESIGN_HANDOFF.md`](UI_DESIGN_HANDOFF.md) | Sam’s Canva/Stitch export checklist |

```bash
npx -p @google/design.md designmd lint .stitch/DESIGN.md
stitch upload design .stitch/DESIGN.md
```

## Agents and skills

| Invoke | Path |
|--------|------|
| Disklordz VST UI lane | [docs/DISKLORDZ_VST_UI_AGENT.md](DISKLORDZ_VST_UI_AGENT.md), `@skill disklordz-vst-ui` |
| Stitch ↔ JUCE architect | `@skill juce-stitch-gui`, subagent `.cursor/agents/juce-stitch-gui.md` |
| Stitch CLI / Loop | `@skill stitch` → `.agents/skills/stitch/` |

## Toolchain map

| Surface | Install | Notes |
|---------|---------|-------|
| **CLI** | `bash scripts/install-stitch-cli.sh` | Loop, capture, `stitch upload design` |
| **MCP** | `stitch mcp install --target <editor>` | Optional Canvas in IDE; API key via login, not git |
| **Skills** | `stitch agent-skills add` | Bundled `/stitch` playbooks |
| **Lint** | `npx -p @google/design.md designmd` | DESIGN.md v0.4 alpha; contrast warnings may not fail exit code |

Remote MCP (alternative to stdio from `stitch mcp install`): `https://stitch.googleapis.com/mcp` with `X-Goog-Api-Key` — see `.cursor/mcp.json.example`.

Official sources: [stitch.withgoogle.com/docs](https://stitch.withgoogle.com/docs), Google Labs `stitch-skills`, `design.md` repos. Community mirror: stitchprompt.com (unofficial).

## End-to-end (native skin — JD Upgraded)

1. Lint and upload `.stitch/DESIGN.md`.
2. Stitch DESKTOP screens at **1020×780** (or Sam’s Canva export).
3. Export layer PNGs + knob filmstrips → `shared/disklordz_ui/skins/<skin>/assets/`.
4. Update `layout.json` control ids (must match APVTS / `componentID`).
5. `cmake --build build --target RenderJdEditor` → editor PNG artifact.

DSP smoke: `ctest -R JDUpgradedMidi`.

## End-to-end (WebView — when enabled)

1. Same DESIGN.md + Stitch DESKTOP canvas (default **960×600**).
2. Download HTML/screenshot; bundle with `WebBrowserComponent::Resource`.
3. Wire `WebSliderRelay`, `WebToggleButtonRelay`, `WebComboBoxRelay`.
4. CMake: `NEEDS_WEBVIEW2`, `withNativeIntegrationEnabled` for Linux `juce://` routes.

## Stitch prompting cheatsheet

- **Device:** `DESKTOP` only inside plugin window.
- **Variations:** `REFINE` (polish) vs `EXPLORE` (`LAYOUT`, `COLOR_SCHEME`).
- **Onboarding:** capture + `upload screen`, not `generate screen` (see stitch skill).

## CI / docs caveat

Stitch skills are Google Labs OSS, not a guaranteed Google product. DESIGN.md spec is alpha; enforce WCAG in CI by parsing lint JSON if you need hard gates.