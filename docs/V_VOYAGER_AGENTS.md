# V Voyager — agent roster (Preset + UI + Implementer)

You asked for a **Preset Agent** and a **UI Agent** alongside the build agent. Here is where they live and how they connect to your **VOYAGER-V mockup**.

## Roster

| Role | Agent ID | Where defined | What it owns |
|------|----------|---------------|--------------|
| **Implementer** | `cursor-v-voyager` | [docs/V_VOYAGER_PM.md](V_VOYAGER_PM.md) · `.cursor/rules/cursor-v-voyager-agent.mdc` | JUCE DSP, ship, pluginval, CI |
| **Preset Agent** | `v-voyager-preset-agent` | [plugin-factory/.../agents/PRESET_AGENT.md](../plugin-factory/plugins/moog-voyager/agents/PRESET_AGENT.md) | 300 factory programs, generator, stack/pink tags |
| ↳ Fleet backing | `hardware-preset-designer` | [disklordz/agents/profit/hardware-preset-designer/agent.md](../disklordz/agents/profit/hardware-preset-designer/agent.md) | Same preset lane + SynthForge for hardware |
| **UI Agent** | `v-voyager-ui-agent` | [plugin-factory/.../agents/UI_AGENT.md](../plugin-factory/plugins/moog-voyager/agents/UI_AGENT.md) | Editor visual parity with mockup |
| ↳ Fleet backing | `v-voyager-ui-designer` | [disklordz/agents/profit/v-voyager-ui-designer/agent.md](../disklordz/agents/profit/v-voyager-ui-designer/agent.md) | `SynthEditor` only |

## Your UI mockup (was missing from git — now in repo)

**[plugin-factory/plugins/moog-voyager/docs/UI_CONCEPT_VOYAGER-V.png](../plugin-factory/plugins/moog-voyager/docs/UI_CONCEPT_VOYAGER-V.png)**

The UI Agent treats this file as the **visual contract**. Implementation checklist: [UI_CONCEPT.md](../plugin-factory/plugins/moog-voyager/docs/UI_CONCEPT.md).

Previously only linked in markdown; the PNG is now committed so Cloud Agents and Copilot can load it.

## Preset Agent — run now

```bash
python3 disklordz/integrations/agents/v_voyager_preset_agent.py --regenerate --qa
```

## Cursor Cloud automations (paste into Automations)

**Preset Agent**

```markdown
You are v-voyager-preset-agent on Instruments.
Read plugin-factory/plugins/moog-voyager/agents/PRESET_AGENT.md and PRESETS.md.
Regenerate with scripts/generate_factory_presets.py; run v_voyager_preset_agent.py --qa.
Do not change SynthParameterIds without coordinating cursor-v-voyager.
```

**UI Agent**

```markdown
You are v-voyager-ui-agent (v-voyager-ui-designer).
Read plugin-factory/plugins/moog-voyager/docs/UI_CONCEPT_VOYAGER-V.png and agents/UI_AGENT.md.
Edit SynthEditor only; match the mockup. Screenshot before/after in artifacts.
```

## Airtable work orders

Import / update via [disklordz/airtable/seed/v-voyager-2026.json](../disklordz/airtable/seed/v-voyager-2026.json):

- **WO-2026-VVOY-004** — Preset Agent (`hardware-preset-designer`)
- **WO-2026-VVOY-005** — UI Agent (`v-voyager-ui-agent`) → mockup parity

## Handoff inbox (optional)

Templates: `disklordz/cursor-cloud/inbox/CO-VVOY-PRESET-*.json`, `CO-VVOY-UI-*.json` (create when Airtable moves WOs to **Ready for Cursor Cloud**).
