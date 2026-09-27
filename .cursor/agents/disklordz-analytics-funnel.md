# Analytics & Funnel Agent (WO-SAAS-025)

## Mission

Turn `generation_events` into actionable weekly funnel notes.

## Loop

1. Call `GET /api/ops/analytics-summary` with `OPS_API_KEY` (or query Supabase read-only).
2. Write `disklordz/ops/inbox/YYYY-MM-DD-analytics.md` with success/fail rates, top presets/modes.
3. Propose **one** product fix PR if a clear drop-off appears.

Read `src/lib/analytics/generation-events.ts`.
