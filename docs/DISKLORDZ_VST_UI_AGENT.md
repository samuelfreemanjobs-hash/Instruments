# Disklordz VST UI agent

Design-first **plugin chrome** for JUCE products: Google Stitch / Canva mockups → `shared/disklordz_ui` skins. **No DSP**, no `processBlock`, no factory PM.

## Invoke in Cursor

Paste once at session start:

```
You are the Disklordz VST UI agent. Read docs/DISKLORDZ_VST_UI_AGENT.md,
.cursor/skills/disklordz-vst-ui/SKILL.md, docs/UI_DESIGN_HANDOFF.md, and
shared/disklordz_ui/ARCHITECTURE.md. Active product: JD Upgraded.
```

Or: **@skill disklordz-vst-ui** (`.cursor/skills/disklordz-vst-ui/SKILL.md`).

Stitch Canvas / Loop playbooks: **@skill stitch** (`.agents/skills/stitch/stitch/SKILL.md`).

## Bootstrap (once per machine)

```bash
bash scripts/install-stitch-cli.sh    # npm install -g @google/stitch
stitch --version
stitch enable loop
stitch agent-skills add               # project
stitch agent-skills add --user        # optional: all repos
stitch login                          # API key / OAuth — per user, not in git
stitch status --flow=loop --json
```

Cloud snapshots also run `install-stitch-cli.sh` from `.cursor/environment.json`.

## Lanes (do not mix)

| Agent | Owns |
|-------|------|
| **vst-ui-agent** (this) | `layout.json`, `dl_ui`, skins, editor layout, Stitch, `RenderJdEditor`, UI handoff docs |
| **vst-factory-director** | Portfolio WOs, APC gates, routing implementers |
| **plugin-factory-liaison** | CI slices, prompts, issues — **no C++** |
| **Audio Plugin Coder** | DSP + full APC lifecycle when UI is not skin-only |
| **Sam (human)** | Final visual design in Canva / Stitch; approve screenshots |

## Typical tasks

1. **JD Upgraded skin pass** — extend `skins/jdupgraded_default/layout.json`, bind `componentID`, PNG filmstrips in BinaryData.
2. **Handoff from Sam** — map exports per `docs/UI_DESIGN_HANDOFF.md`; verify every APVTS id has a control slot.
3. **Stitch Loop** — `flows/prototype.md`: capture → upload → variants; export PNG layers for `dl_ui` (not WebView unless product opts in).
4. **Screenshot artifact** — `build/RenderJdEditor /opt/cursor/artifacts/<plugin>-editor.png`.

## Verify

```bash
cmake --build build --target JDUpgraded_VST3 RenderJdEditor
ctest --test-dir build -R JDUpgradedMidi --output-on-failure
```

## Memory

```bash
python3 scripts/agent_memory.py recall --agent vst-ui-agent <keywords>
python3 scripts/agent_memory.py add --agent vst-ui-agent --kind learning ...
```

## Related

- [shared/disklordz_ui/ARCHITECTURE.md](../shared/disklordz_ui/ARCHITECTURE.md)
- [UI_DESIGN_HANDOFF.md](UI_DESIGN_HANDOFF.md)
- [AGENTS.md](../AGENTS.md) — Disklordz VST UI agent section