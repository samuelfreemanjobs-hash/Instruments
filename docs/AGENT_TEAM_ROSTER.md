# Agent team roster — Instruments monorepo

Canonical names, owners, and handoff paths for humans and automations. **PM Agent** assigns work; **Workflow Automation Agent** fires scheduled and event-driven runs; implementers stay in non-conflicting lanes.

| Agent ID | Role | Primary stack | Handoff / trigger |
|----------|------|---------------|-------------------|
| **pm-agent** | Product & work orders | Airtable Disklordz OS | Opens WOs, GitHub issues, sets `owner_agent` |
| **workflow-automation-agent** | Schedules & integrations | GitHub Actions, Zapier, Cursor Automations | Cron, `repository_dispatch`, subscription timers |
| **cursor-vst-plugin-factory** | JUCE / VST3 / CLAP factory | `Source/`, `Wave909/`, `ProphetRev2Trap/`, root CMake | `[Plugin][JUCE]` WOs → [VST factory inbox](../disklordz/vst-factory/inbox/) |
| **night-circuit-qa** | Night Circuit verification | `ProphetRev2Trap/`, `vst-testing-ops/` | `[Plugin][JUCE][QA]` · schedule: [VST_PLUGIN_FACTORY_SCHEDULE.md](VST_PLUGIN_FACTORY_SCHEDULE.md) |
| **night-circuit-sound-design** | Presets + GUI curation | `ProphetRev2Trap/design/`, `FeaturedPresets.cpp` | `[Plugin][JUCE][Design]` · biweekly Thu automation |
| **cursor-cloud** (generic) | SaaS & cross-cutting | `disklordz/website/` | `[SaaS]` WOs — see [DISKLORDZ_SAAS_AGENT_LANES.md](DISKLORDZ_SAAS_AGENT_LANES.md) |
| **antigravity-hise** | HISE sketch lane (Windows) | `hise-sketch/` | `[Plugin][HISE]` → [Antigravity inbox](../disklordz/antigravity/inbox/) |
| **factory-manager** | WIP & capacity (human + Airtable) | Policy | Max **2** active Cursor JUCE WOs |
| **business-planner** | SKU / GTM gate | Human | New customer-facing plugin SKUs |
| **marketing** | Briefs & launch copy | Human | Required before store-facing SKUs |

## PM Agent — routing rules

1. Every implementation task gets an **Agent Work Order** with `work_order_id` (`WO-…`).
2. Title prefix sets the lane:
   - **`[Plugin][JUCE]`** → `owner_agent: cursor-vst-plugin-factory`
   - **`[Plugin][HISE]`** → `owner_agent: antigravity-hise`
   - **`[SaaS]`** → Cursor Cloud on `disklordz/website/` (does not count against JUCE WIP)
3. PR titles and GitHub issues must include the WO id.
4. Before opening a **new customer-facing** JUCE SKU, confirm **Business Planner + Marketing** (same gate as [DISKLORDZ_PLUGIN_TRACKS.md](DISKLORDZ_PLUGIN_TRACKS.md)).
5. Respect **Factory Manager** WIP: no more than **two** `[Plugin][JUCE]` WOs **In Progress** for Cursor unless policy is explicitly extended in Airtable.

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

## Related docs

- [CURSOR_AGENT_PLAYBOOK.md](CURSOR_AGENT_PLAYBOOK.md)
- [AGENTIC_PROJECT_STANDARDS.md](AGENTIC_PROJECT_STANDARDS.md)
- [DISKLORDZ_SAAS_AGENT_LANES.md](DISKLORDZ_SAAS_AGENT_LANES.md)
- [DISKLORDZ_PLUGIN_TRACKS.md](DISKLORDZ_PLUGIN_TRACKS.md)
