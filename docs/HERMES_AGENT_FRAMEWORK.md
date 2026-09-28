# Hermes — multi-seat agent framework (Cursor)

**Status:** **Default development and coding model** for this monorepo (plugins, SaaS, RAG, tools).  
**Quickstart:** [HERMES_QUICKSTART.md](HERMES_QUICKSTART.md)

**Hermes** orchestrates **senior specialist seats**. The **Hermes lead** (your Cursor Cloud or IDE Agent session) routes work, enforces WOs, and delivers PRs with evidence. Seats implement; they do not bypass security or merge without human review.

**Grok Bot** runs the **closed-loop** spec/WO layer ([GROK_CLOSED_LOOP_ENGINE.md](GROK_CLOSED_LOOP_ENGINE.md)). **Hermes + Cursor** runs **code + proof**.

## Seats (full team — 15 Cursor seats + Grok)

**Catalog:** [HERMES_SEATS.md](HERMES_SEATS.md) · **Registry:** [.cursor/hermes/SKILLS_REGISTRY.md](../.cursor/hermes/SKILLS_REGISTRY.md)

| Tier | Seats |
|------|--------|
| **1 Implement** | lead, architect, dsp, gui, web, qa, research (Hyperresearch) |
| **2 Platform** | devops, handoff, ops |
| **3 Revenue / quality** | gtm, presets, support, security, data |

**Toolkit CLI** (Tier 2–3 helpers): `python3 disklordz/hermes/scripts/hermes_tool.py --help`

**Grok** (`hermes-grokgate`): closed-loop specs/WOs — not a Cursor seat ([GROK_CLOSED_LOOP_ENGINE.md](GROK_CLOSED_LOOP_ENGINE.md)).

Hyperresearch: [HERMES_HYPERRESEARCH.md](HERMES_HYPERRESEARCH.md) · `disklordz/research/scripts/hyperresearch.py`

Invoke via Cursor **Task** with `description` prefixed by seat ID, e.g. `hermes-gui: Celestial polish`.

## Operating loop

Observe → Decide → Dispatch → Verify → Record (same as Grok closed-loop; Hermes **Verify** = tests + artifacts).

**Lead rules:**

1. Max **2** concurrent Cursor **JUCE** implementation WOs (factory policy). SaaS WOs do not count against HISE sketch WIP — see [DISKLORDZ_SAAS_AGENT_LANES.md](DISKLORDZ_SAAS_AGENT_LANES.md).
2. PR titles include `WO-…` when work is tracked in Airtable (plugins: `WO-2026-NNN`, SaaS: `WO-SAAS-NNN`).
3. **Plugin UI:** GUI Agent evidence for non-trivial JUCE editor changes.
4. **SaaS UI:** browser walkthrough + `npm run build` green.
5. Read product `ARCHITECTURE.md` before structural edits.

## Product → crew (default)

| Product / lane | Primary seats |
|----------------|---------------|
| **Junova-X** | architect, dsp, gui, qa |
| **JD Upgraded** (`Source/`) | dsp, qa; architect if CMake/targets change |
| **Wave909** | dsp, gui, qa |
| **Drum SaaS** | web, qa |
| **RAG / automation** | web or lead |
| **HISE sketch** | Antigravity (Windows); Cursor on **JUCE port WO** only |

### Junova-X (P0 example)

```text
hermes-lead → branch + PR
hermes-architect → Junova-X/CMakeLists.txt, ARCHITECTURE.md
hermes-dsp → Source/DSP/ parity
hermes-gui → Source/UI/Celestial/ + design PNGs
hermes-qa → pluginval + host smoke
```

## File anchors

| Path | Purpose |
|------|---------|
| `.cursor/rules/hermes-default.mdc` | Always-on: Hermes is default dev team |
| `.cursor/hermes/SKILLS_REGISTRY.md` | Skills installed per seat |
| `.cursor/skills/hermes-elite-*/SKILL.md` | Seat SOPs — **read at task start** |
| `.cursor/hermes/seats/*.md` | Charters for Task prompts |
| `.cursor/rules/hermes-junova-x.mdc` | Extra context for `Junova-X/**` |

## Copy-paste: Hermes lead (Cursor)

```text
You are Hermes lead — our default dev team for samuelfreemanjobs-hash/Instruments.
Read docs/HERMES_AGENT_FRAMEWORK.md and .cursor/hermes/SKILLS_REGISTRY.md.
Route work to elite seats; each seat reads its hermes-elite-* skill before coding.
Deliver WO-titled PRs with test evidence. Grok owns spec loop; you own implementation loop.
```

## Related

- [HERMES_QUICKSTART.md](HERMES_QUICKSTART.md)
- [AGENTS.md](../AGENTS.md)
- [CURSOR_AGENT_PLAYBOOK.md](CURSOR_AGENT_PLAYBOOK.md)
- [GROK_CLOSED_LOOP_ENGINE.md](GROK_CLOSED_LOOP_ENGINE.md)
