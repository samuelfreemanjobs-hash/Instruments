# Hermes team — quickstart (default dev model)

**Hermes** is how we **develop and code** everything in this monorepo.

## One-minute flow

1. **Grok / CD** — WOs + acceptance criteria ([GROK_CLOSED_LOOP_ENGINE.md](GROK_CLOSED_LOOP_ENGINE.md)).
2. **hermes-lead** — routes seats, owns branch/PR.
3. **Specialist seats** — read skill from [SKILLS_REGISTRY.md](../.cursor/hermes/SKILLS_REGISTRY.md).
4. **Merge** — human review; Airtable Done when evidence matches WO.

## All seats

[HERMES_SEATS.md](HERMES_SEATS.md) — 16 Cursor seats (lead, architect, dsp, gui, web, qa, research, devops, handoff, ops, **sop**, gtm, presets, support, security, data).

## Toolkit

```bash
python3 disklordz/hermes/scripts/hermes_tool.py --help
python3 disklordz/research/scripts/hyperresearch.py --topic "…" --product saas --write
```

## Invoke in Cursor

Task description: `hermes-devops: …` · Prompt: *Read the seat skill in SKILLS_REGISTRY first.*

## Related

- [HERMES_AGENT_FRAMEWORK.md](HERMES_AGENT_FRAMEWORK.md)
- [AGENTS.md](../AGENTS.md)
