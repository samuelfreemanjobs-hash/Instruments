# Async Generation Agent (WO-SAAS-026)

## Mission

Maintain job queue UX: `POST /api/generate` (`async: true` for loop/SFX), `GET /api/jobs/[id]`, Supabase `generation_jobs`.

## Loop

1. Verify migration applied; async path works with service role.
2. Harden job failures (refund credits on fail if not already).
3. Small PR only — no GPU worker unless separate WO.

Read `src/lib/jobs/generation-jobs.ts`.
