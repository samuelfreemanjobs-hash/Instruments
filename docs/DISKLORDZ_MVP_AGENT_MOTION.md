# Disklordz profit agents — MVP motion

Products are at **MVP** (SaaS generate/factory, plugins in CI, SynthForge lane). Profit agents should **move on a schedule**, not only exist in `PM_ADD.md`.

## Run locally

```bash
# Dev server on :3000 recommended
cd disklordz/website && npm ci && npm run dev

# Quick motion (integration promote, RAG chunk, stub checklist, optional site smoke)
./scripts/run-agent-fleet-mvp.sh

# Include JD/WAVE-9090 ci-verify + DAW inbox syntax check
FLEET_MVP_FULL=1 ./scripts/run-agent-fleet-mvp.sh
```

Outputs:

| Artifact | Purpose |
|----------|---------|
| [`docs/fleet-activity-latest.md`](fleet-activity-latest.md) | Human-readable last run (all agents) |
| `disklordz/agents/workflows/fleet-activity.jsonl` | Append-only event log |

## CI autopilot

| Workflow | When | What moves |
|----------|------|------------|
| [`agent-fleet-mvp-motion.yml`](../.github/workflows/agent-fleet-mvp-motion.yml) | **08:00 & 16:00 UTC** weekdays | Full MVP motion matrix + artifact upload |
| [`agent-fleet-execute.yml`](../.github/workflows/agent-fleet-execute.yml) | **15:00 UTC** weekdays | `fleet_execute.py` (includes MVP motion tail) |
| [`agent-fleet-governance.yml`](../.github/workflows/agent-fleet-governance.yml) | Daily + fleet path pushes | Registry drift guard |
| [`agent-fleet-health.yml`](../.github/workflows/agent-fleet-health.yml) | **14:00 UTC** daily | Fleet count + optional prod verify |

Set **`DISKLORDZ_VERIFY_BASE_URL`** in GitHub secrets so **integration-health** and **conversion-qa** run against production, not only localhost.

## Activation tiers (MVP → prod)

| Tier | Agents | Unblock |
|------|--------|---------|
| **A — repo (no secrets)** | pm-agent, workflow-automation, prompt-coach (chunk), release-notes, hardware-preset-designer, integration promote | Run `./scripts/run-agent-fleet-mvp.sh` |
| **B — dev URL** | integration-health, conversion-qa, lane-workflow (007 smoke), support-macro, fraud-abuse (via generate) | `DISKLORDZ_URL=http://127.0.0.1:3000` |
| **C — prod URL** | Same as B against Vercel | `DISKLORDZ_VERIFY_BASE_URL` in GitHub |
| **D — revenue / async** | billing-ops, async-generation, onboarding, analytics, churn, marketing | Stripe, Inngest, n8n — [`NEXT.md`](NEXT.md) |

## Extend motion

Edit [`disklordz/agents/workflows/fleet_mvp_motion.py`](../disklordz/agents/workflows/fleet_mvp_motion.py) — keep hooks **safe** (no writes to prod DB/Stripe without human gates per META R10).

Related: [DISKLORDZ_AGENT_FLEET.md](DISKLORDZ_AGENT_FLEET.md) · [PM_ADD.md](../disklordz/agents/workflows/PM_ADD.md) · [STUB_AGENT_BLOCKERS.md](../disklordz/agents/workflows/STUB_AGENT_BLOCKERS.md)
