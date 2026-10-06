# Sub-agents — Prompt Coach

Coordinator/worker split per Claude Code coordinator mode.

| Sub-agent | Model | Tools | Role |
|-----------|-------|-------|------|
| `prompt-coach-explore` | haiku | Read, Grep, Glob | Gather repo + API evidence only |
| `prompt-coach-verify` | inherit | Read, Bash | Run verification scripts; no writes |
| `prompt-coach-implement` | inherit | Read, Edit, Write | Apply minimal diff after coordinator plan |

## Delegation rules

1. Coordinator **never** delegates understanding — pass exact paths, env names, and API routes.
2. Messages delivered **between tool rounds** only (command queue invariant).
3. `prompt-coach-verify` must run `disklordz/integrations/scripts/verify-integrations.sh` when touching SaaS routes.
4. Max fan-out: 3 workers per turn unless user approves.

## Fork agents (cache-safe)

When summarizing long transcripts, use fork with **byte-identical** tool array and placeholder tool results (see `loop.md`).
