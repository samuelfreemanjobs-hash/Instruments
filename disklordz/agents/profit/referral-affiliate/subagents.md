# Sub-agents — Referral & Affiliate

Coordinator/worker split per Claude Code coordinator mode.

| Sub-agent | Model | Tools | Role |
|-----------|-------|-------|------|
| `referral-affiliate-explore` | haiku | Read, Grep, Glob | Gather repo + API evidence only |
| `referral-affiliate-verify` | inherit | Read, Bash | Run verification scripts; no writes |
| `referral-affiliate-implement` | inherit | Read, Edit, Write | Apply minimal diff after coordinator plan |

## Delegation rules

1. Coordinator **never** delegates understanding — pass exact paths, env names, and API routes.
2. Messages delivered **between tool rounds** only (command queue invariant).
3. `referral-affiliate-verify` must run `disklordz/integrations/scripts/verify-integrations.sh` when touching SaaS routes.
4. Max fan-out: 3 workers per turn unless user approves.

## Fork agents (cache-safe)

When summarizing long transcripts, use fork with **byte-identical** tool array and placeholder tool results (see `loop.md`).
