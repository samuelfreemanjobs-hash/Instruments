# Marketing Director — hierarchical team orchestrator

## Purpose

**Marketing Director** supervises marketing specialists (research, creative, copy, media, analytics, compliance) using a **supervisory pattern**: the Director plans and delegates; **Cursor Cloud / Claude Code teammates** execute each specialist role and commit JSON artifacts — **no Anthropic API in repo CI**.

Optional **local live mode** (`marketing_director_live.py`) and **eval harness** (`test_harness.py`) for developers with `ANTHROPIC_API_KEY`; CI runs **mocked** harness only.

## Build & run (team mode — default)

```bash
python3 tools/marketing-director/marketing_director_team.py plan \
  --request "Mother's Day …" \
  --slug mothers-day-skincare

# Or via business CLI:
python3 scripts/business-agents/business_agent.py director plan --request "…" --slug my-campaign
```

Creates `business-agents/marketing-director/runs/<slug>/` with director task, specialist handoffs, and output slots.

After teammates finish:

```bash
python3 scripts/business-agents/business_agent.py director validate --run business-agents/marketing-director/runs/<slug>
```

## Data flow

```text
User request
  → director plan (scaffold run folder)
  → Cloud Agent as marketing-director (plan in <plan> tags, delegate)
  → Specialist teammates → *.json in run/specialists/
  → compliance_agent (hard gate before publish)
  → director synthesis → deliverable.md
  → Owner publishes (never CI)
```

## Key modules

| Path | Role |
|------|------|
| `marketing_director_team.py` | Scaffold runs, validate completeness |
| `marketing_director.py` | Importable orchestrator (live + tests) |
| `marketing_director_live.py` | Optional local API CLI (dev only) |
| `test_harness.py` | Mocked / smoke / full eval (`--mode mocked` in CI) |
| `eval_cases.py` | Eval case definitions |
| `brand_memory/instruments_disklordz.json` | Default brand_memory_read |
| `../../docs/business-agents/skills/marketing-plan-current-quarter/SKILL.md` | Current quarter strategy |
| `../../docs/business-agents/skills/brand-voice/SKILL.md` | Brand voice skill (copy, creative, director) |
| `../../docs/business-agents/skills/prohibited-claims-and-disclaimers/SKILL.md` | Compliance rulebook |
| `../docs/business-agents/prompts/marketing-director-system.md` | Director system prompt |
| `../docs/business-agents/prompts/specialists/*.md` | Specialist prompts |

## Extension points

- Wire `request_human_review` to Slack (see `disklordz/automation/` patterns).
- Replace stub tool executors in live mode with RAG / web search.

## Related

- [docs/business-agents/TEAM_ROLES.md](../../docs/business-agents/TEAM_ROLES.md)
- [.claude/agents/marketing-director/AGENT.md](../../.claude/agents/marketing-director/AGENT.md)
