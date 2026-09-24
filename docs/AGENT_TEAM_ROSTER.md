# Agent team roster — Instruments monorepo

Canonical names, owners, and handoff paths for humans and automations. **PM Agent** assigns work; **Workflow Automation Agent** fires scheduled and event-driven runs; implementers stay in non-conflicting lanes.

| Agent ID | Role | Primary stack | Handoff / trigger |
|----------|------|---------------|-------------------|
| **pm-agent** | Product & work orders | Airtable Disklordz OS | Opens WOs, GitHub issues, sets `owner_agent` |
| **workflow-automation-agent** | Schedules & integrations | GitHub Actions, Zapier, Cursor Automations | Cron, `repository_dispatch`, subscription timers |
| **cursor-vst-plugin-factory** | JUCE / VST3 / CLAP factory | `Source/`, `Wave909/`, `ProphetRev2Trap/`, root CMake | `[Plugin][JUCE]` WOs → [VST factory inbox](../disklordz/vst-factory/inbox/) |
| **night-circuit-qa** | **VST Plugin QA** (repo-wide; Night Circuit lead) | All JUCE products — [VST_SUBAGENT_SCOPE.md](VST_SUBAGENT_SCOPE.md) | `[Plugin][JUCE][QA]` · [VST_PLUGIN_QA_AGENT.md](VST_PLUGIN_QA_AGENT.md) |
| **night-circuit-sound-design** | **VST Sound Design** (repo-wide; Night Circuit lead) | Presets/curation per product — [VST_SOUND_DESIGN_AGENT.md](VST_SOUND_DESIGN_AGENT.md) | `[Plugin][JUCE][Design]` · Thu automation |
| **vst-gui-designer** | **Senior VST GUI** (repo-wide specs & review) | All `PluginEditor.*`, `*/design/*GUI*`, [VST_GUI_SYSTEM.md](VST_GUI_SYSTEM.md) | **`[Plugin][JUCE][GUI]`** · Fri audit · PR review gate |
| **cursor-cloud** (generic) | SaaS & cross-cutting | `disklordz/website/` | `[SaaS]` WOs — see [DISKLORDZ_SAAS_AGENT_LANES.md](DISKLORDZ_SAAS_AGENT_LANES.md) |
| **antigravity-hise** | HISE sketch lane (Windows) | `hise-sketch/` | `[Plugin][HISE]` → [Antigravity inbox](../disklordz/antigravity/inbox/) |
| **factory-manager** | WIP & capacity (human + Airtable) | Policy | Max **2** active Cursor JUCE WOs |
| **business-planner** | SKU / GTM gate | Human | New customer-facing plugin SKUs |
| **marketing** | Briefs & launch copy | Human | Required before store-facing SKUs |

## PM Agent — routing rules

1. Every implementation task gets an **Agent Work Order** with `work_order_id` (`WO-…`).
2. Title prefix sets the lane:
   - **`[Plugin][JUCE]`** → `owner_agent: cursor-vst-plugin-factory`
   - **`[Plugin][JUCE][QA]`** → `owner_agent: night-circuit-qa` (any product; name product in title e.g. `[Wave909]`)
   - **`[Plugin][JUCE][Design]`** → `owner_agent: night-circuit-sound-design` (any product)
   - **`[Plugin][JUCE][GUI]`** → `owner_agent: vst-gui-designer` (spec, audit, review; Factory implements)
   - **`[Plugin][JUCE][GUI][Review]`** → `owner_agent: vst-gui-designer` (PR review-only, docs/report)
   - **`[Plugin][HISE]`** → `owner_agent: antigravity-hise`
   - **`[SaaS]`** → Cursor Cloud on `disklordz/website/` (does not count against JUCE WIP)
3. **GUI gate:** PRs that change `PluginEditor.*` should reference a GUI spec and request **vst-gui-designer** sign-off (or linked `[Plugin][JUCE][GUI][Review]` WO).
4. PR titles and GitHub issues must include the WO id.
5. Before opening a **new customer-facing** JUCE SKU, confirm **Business Planner + Marketing** (same gate as [DISKLORDZ_PLUGIN_TRACKS.md](DISKLORDZ_PLUGIN_TRACKS.md)). New SKUs require **`[Plugin][JUCE][GUI]`** spec WO before Factory UI WIP.
6. Respect **Factory Manager** WIP: no more than **two** `[Plugin][JUCE]` WOs **In Progress** for Cursor unless policy is explicitly extended in Airtable. **`[Plugin][JUCE][GUI]`** docs-only specs do not count toward the two unless paired with implementation in the same WO.

### PM → VST Plugin Factory handoff

```bash
cd disklordz/automation
python3 scripts/wo_to_vst_factory_handoff.py --work-order-id WO-2026-JUCE-001 --branch main
```

Or Airtable button → GitHub `repository_dispatch` event `airtable-vst-factory-handoff` (see [disklordz/automation/VST_PLUGIN_FACTORY.md](../disklordz/automation/VST_PLUGIN_FACTORY.md)).

## Workflow Automation Agent — responsibilities

| Cadence | Automation | Outcome |
|---------|------------|---------|
| **Mon 09:00 UTC** | [vst-plugin-factory-schedule.yml](../.github/workflows/vst-plugin-factory-schedule.yml) `plugin-ci-health` | Monorepo plugin CI artifact |
| **Mon 09:30 / 10:00 UTC** | Cursor Automations | Factory health triage · **night-circuit-qa** Gate 1 |
| **Wed 10:00 UTC** | `night-circuit-integrity` job | Night Circuit build, bank tests, pluginval artifact |
| **Daily 11:00 UTC** | [nightly-qa.yml](../.github/workflows/nightly-qa.yml) | Full monorepo QA |
| **Fri 17:00 / 17:30 UTC** | `doc-sync` + Cursor | ARCHITECTURE checklist · factory doc PR |
| **Fri 17:25 UTC** | `fri-vst-gui-audit` handoff | **vst-gui-designer** repo GUI audit / report |
| **Thu 16:00 UTC (biweekly)** | Cursor Automation | **night-circuit-sound-design** batch |
| **On WO assign** | `airtable-vst-factory-handoff` | Inbox JSON + commit on branch |
| **Inbox on main** | [vst-factory-inbox-slack.yml](../.github/workflows/vst-factory-inbox-slack.yml) | Slack → start **cursor-vst-plugin-factory** |
| **Schedule complete** | [vst-factory-schedule-slack.yml](../.github/workflows/vst-factory-schedule-slack.yml) | Slack health / Night Circuit result |
| **On plugin PR push** | GitHub CI `cmake` | Required check |

Full calendar and Cursor Automation prompt templates: [VST_PLUGIN_FACTORY_SCHEDULE.md](VST_PLUGIN_FACTORY_SCHEDULE.md).

## VST Plugin Factory — standing membership

**cursor-vst-plugin-factory** is a **regular** factory implementer (not ad-hoc). Default persona and prompt shell: [VST_PLUGIN_FACTORY_AGENT.md](VST_PLUGIN_FACTORY_AGENT.md). Cursor rule (plugin paths): [`.cursor/rules/vst-plugin-factory.mdc`](../.cursor/rules/vst-plugin-factory.mdc).

## Slack

- **#agent-team** (`C0C3M2NDK7Z`) — meetings, roster, automation decisions — [SLACK_AGENT_TEAM.md](SLACK_AGENT_TEAM.md)
- **#disklordz-dev** — factory inbox + schedule webhooks — [VST_PLUGIN_FACTORY_SLACK.md](VST_PLUGIN_FACTORY_SLACK.md)

## Senior VST GUI Designer

Enterprise UI authority for all in-repo JUCE editors. Persona: [VST_GUI_DESIGNER_AGENT.md](VST_GUI_DESIGNER_AGENT.md). Baseline audit: [reports/VST_GUI_AUDIT_2026-09-24.md](reports/VST_GUI_AUDIT_2026-09-24.md).

## Related docs

- [VST_GUI_SYSTEM.md](VST_GUI_SYSTEM.md)
- [CURSOR_AGENT_PLAYBOOK.md](CURSOR_AGENT_PLAYBOOK.md)
- [AGENTIC_PROJECT_STANDARDS.md](AGENTIC_PROJECT_STANDARDS.md)
- [DISKLORDZ_SAAS_AGENT_LANES.md](DISKLORDZ_SAAS_AGENT_LANES.md)
- [DISKLORDZ_PLUGIN_TRACKS.md](DISKLORDZ_PLUGIN_TRACKS.md)
