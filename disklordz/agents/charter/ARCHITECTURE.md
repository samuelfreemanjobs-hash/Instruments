# META LLM Charter (Disklordz integration)

## Purpose

Embed [entropyvortex/meta-llm-charter](https://github.com/entropyvortex/meta-llm-charter) so all **profit agents** and Cloud/Cursor work share one discipline core (META v3.1: Bias, META-0, R1–R10).

## Build & run

```bash
./scripts/sync-meta-llm-charter.sh
```

Installs:

- `META_CHARTER.md` — upstream `CLAUDE.md` (≈2.4 KB, CI can gate size)
- `.claude/skills/{zero-pause,weave,premortem}` — explicit-invocation only
- Root `CLAUDE.md` — charter pointer + Disklordz entry

## Data flow

```text
Session start → CLAUDE.md / AGENTS.md
             → META_CHARTER.md (always-on rules)
             → profit agent agent.md + soul.md
             → optional /weave /premortem /zero-pause
```

## Key modules

| Path | Role |
|------|------|
| `META_CHARTER.md` | Normative R1–R10 + META-0 |
| `vendor-skills/` | Vendored skill sources |
| `UPSTREAM.json` | Pin commit + byte count |
| [`../profit/`](../profit/) | 29 profit agents (soul references charter) |

## Extension points

Re-sync after upstream releases: `./scripts/sync-meta-llm-charter.sh`. Optional: wire `weave/hooks/scope-guard.sh` in `.claude/settings.json` PreToolUse.

## Related docs

- [docs/DISKLORDZ_PROFIT_AGENTS.md](../../docs/DISKLORDZ_PROFIT_AGENTS.md)
- [AGENTS.md](../../AGENTS.md)
