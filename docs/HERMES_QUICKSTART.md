# Hermes team — quickstart (default dev model)

**Hermes** is the standard way we **develop and code** everything in `samuelfreemanjobs-hash/Instruments`: plugins, SaaS, RAG, and tooling.

## One-minute flow

1. **Creative Director / Grok** — closed-loop WOs + acceptance criteria ([GROK_CLOSED_LOOP_ENGINE.md](GROK_CLOSED_LOOP_ENGINE.md)).
2. **Hermes lead** (Cursor Cloud or IDE Agent) — reads [HERMES_AGENT_FRAMEWORK.md](HERMES_AGENT_FRAMEWORK.md), picks seats, owns the branch/PR.
3. **Specialist seats** — each reads its skill from [.cursor/hermes/SKILLS_REGISTRY.md](../.cursor/hermes/SKILLS_REGISTRY.md), implements, hands evidence to lead.
4. **Merge** — human review; Airtable Done when WO criteria met.

## Seats

| Seat | When to use |
|------|-------------|
| `hermes-lead` | Every task — routing, WIP, PR |
| `hermes-architect` | CMake, APVTS, module boundaries, new products |
| `hermes-dsp` | C++ realtime audio |
| `hermes-gui` | JUCE editors + **GUI Agent** |
| `hermes-web` | `disklordz/website/` Next.js |
| `hermes-qa` | pluginval, CI, smoke checklists |
| `hermes-research` | **Hyperresearch** — briefs + draft WOs ([HERMES_HYPERRESEARCH.md](HERMES_HYPERRESEARCH.md)) |

Invoke in Cursor: **Task** with description `hermes-dsp: …` and prompt “Read the seat skill in SKILLS_REGISTRY first.”

## Product map

| Product | Primary seats |
|---------|----------------|
| Junova-X | architect + dsp + gui + qa |
| JD Upgraded / Wave909 | dsp + qa (architect if CMake) |
| Drum SaaS | web + qa |
| RAG / automation | web or lead |

## Rules in git

- Always-on: [.cursor/rules/hermes-default.mdc](../.cursor/rules/hermes-default.mdc)
- Junova-X: [.cursor/rules/hermes-junova-x.mdc](../.cursor/rules/hermes-junova-x.mdc)

## Expanding the team

See [HERMES_SEATS_ROADMAP.md](HERMES_SEATS_ROADMAP.md) for business ops, IDE handoff, GitHub/CI, and other recommended seats.

## Related

- [AGENTS.md](../AGENTS.md) — build commands
- [AGENTIC_PROJECT_STANDARDS.md](AGENTIC_PROJECT_STANDARDS.md) — WO discipline
