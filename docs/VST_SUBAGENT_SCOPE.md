# VST subagents — repo-wide scope matrix

**PM Agent:** set `owner_agent` and product in WO title/body. Agent IDs are **stable** (automation/inbox); **scope is monorepo**.

| Agent ID | Display name | PM prefixes | Repo-wide? | Primary product |
|----------|--------------|-------------|------------|-----------------|
| `night-circuit-qa` | **VST Plugin QA** | `[Plugin][JUCE][QA]` | Yes | Night Circuit (deepest) |
| `night-circuit-sound-design` | **VST Sound Design** | `[Plugin][JUCE][Design]` | Yes | Night Circuit (deepest) |
| `vst-gui-designer` | **Senior VST GUI Designer** | `[Plugin][JUCE][GUI]` | Yes | All `PluginEditor.*` |
| `cursor-vst-plugin-factory` | **VST Plugin Factory** | `[Plugin][JUCE]` | Yes | Per WO |

## Product routing (WO body or paths)

| Path prefix | Product | QA doc | Sound design doc |
|-------------|---------|--------|------------------|
| `ProphetRev2Trap/` | Night Circuit | [VST_PLUGIN_QA_AGENT.md](VST_PLUGIN_QA_AGENT.md) | [VST_SOUND_DESIGN_AGENT.md](VST_SOUND_DESIGN_AGENT.md) |
| `Wave909/` | WAVE-909 | Same QA agent | Same design agent |
| `Source/` (JD) | JD Upgraded | Same QA agent | Same design agent |
| `MyFirstPlugin/` | Template | WO-only | WO-only |

## Inbox

`disklordz/vst-factory/inbox/HO-*.json` → `to_agent` may be any row above. Agents must read `work_order` + `context_paths` and apply the **product** from WO title (e.g. `[Plugin][JUCE][QA][Wave909]`).

## Out of lane

- `[SaaS]` → cursor-cloud / website agents  
- `[Plugin][HISE]` → antigravity-hise  

## Related

- [AGENT_TEAM_ROSTER.md](AGENT_TEAM_ROSTER.md)
- [VST_PLUGIN_FACTORY_SCHEDULE.md](VST_PLUGIN_FACTORY_SCHEDULE.md)
