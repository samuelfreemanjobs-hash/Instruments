# Sub-agents — Autonomous Coding Agent (24/7)

Coordinator/worker split per Claude Code coordinator mode.

| Sub-agent | Model | Tools | Role |
|-----------|-------|-------|------|
| `autonomous-coding-agent-explore` | haiku | Read, Grep, Glob | Gather repo + API evidence only |
| `autonomous-coding-agent-verify` | inherit | Read, Bash | Run verification scripts; no writes |
| `autonomous-coding-agent-implement` | inherit | Read, Edit, Write | Apply minimal diff after coordinator plan |

## Delegation rules

1. Coordinator **never** delegates understanding — pass exact paths, env names, and API routes.
2. Messages delivered **between tool rounds** only (command queue invariant).
3. `autonomous-coding-agent-verify` must run `disklordz/integrations/scripts/verify-integrations.sh` when touching SaaS routes.
4. Max fan-out: 3 workers per turn unless user approves.

## Fork agents (cache-safe)

When summarizing long transcripts, use fork with **byte-identical** tool array and placeholder tool results (see `loop.md`).
