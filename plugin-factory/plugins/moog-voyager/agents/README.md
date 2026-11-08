# V Voyager — dedicated agents

| Agent ID (Airtable / Cursor) | Fleet profit agent | Scope |
|------------------------------|-------------------|--------|
| **`v-voyager-preset-agent`** | [`hardware-preset-designer`](../../../../disklordz/agents/profit/hardware-preset-designer/agent.md) | 300-program bank, stack/pink tags, genre curation, audition QA |
| **`v-voyager-ui-agent`** | [`v-voyager-ui-designer`](../../../../disklordz/agents/profit/v-voyager-ui-designer/agent.md) | Match **[UI mockup](../docs/UI_CONCEPT_VOYAGER-V.png)** in `SynthEditor` |

**Implementer (JUCE ship):** `cursor-v-voyager` — builds DSP + wires agent deliverables.

Run preset lane QA:

```bash
python3 disklordz/integrations/agents/v_voyager_preset_agent.py --qa
```

Full roster: [docs/V_VOYAGER_AGENTS.md](../../../../docs/V_VOYAGER_AGENTS.md)
