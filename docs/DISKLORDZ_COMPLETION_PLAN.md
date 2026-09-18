# Disklordz business — completion plan

Master checklist from **code-complete MVP** to **live SaaS**, **durable ops**, and **ILLUGEN-shaped growth**. Use with [DISKLORDZ_SAAS_V0.md](DISKLORDZ_SAAS_V0.md), [DISKLORDZ_GO_LIVE.md](DISKLORDZ_GO_LIVE.md), and [DISKLORDZ_ILLUGEN_RESEARCH.md](DISKLORDZ_ILLUGEN_RESEARCH.md).

**Legend:** **Human** = secrets, dashboards, legal · **Agent** = repo/CI/code · **Done when** = objective exit criteria (no calendar dates).

---

## Phase 0 — Lock engineering baseline

| # | Task | Owner | Done when |
|---|------|-------|-----------|
| 0.1 | Merge PR **Disklordz unified QA** (#45) | Human | On `main`; **Disklordz website** CI green |
| 0.2 | Merge ops command center / plugin CI if not already on `main` | Human | **Build (`cmake`)** green on `main` |
| 0.3 | Require **`cmake`** on `main` (branch protection) | Human | PRs blocked until Build passes — [REPO_AUTOMATION.md](REPO_AUTOMATION.md) |
| 0.4 | Optional: `SLACK_WEBHOOK_URL` + CI Slack workflow | Human | Failed Build posts to Slack |

**Agent habit:** after any `disklordz/**` change → `./disklordz/scripts/run_qa.sh`.

---

## Phase 1 — Production platform (go-live)

Goal: public HTTPS app talking to **prod Supabase** and **Stripe test or live**.

### 1A — Vercel

| # | Task | Owner | Done when |
|---|------|-------|-----------|
| 1.1 | Create Vercel project; **Root Directory** = `disklordz/website` | Human | Production URL exists |
| 1.2 | Connect GitHub; deploy from `main` | Human | Push to `main` triggers deploy |
| 1.3 | Create **Production Deploy Hook** → save as `VERCEL_DEPLOY_HOOK_URL` | Human | Hook URL in GitHub secrets |

Ref: [DISKLORDZ_GO_LIVE.md](DISKLORDZ_GO_LIVE.md) §1.

### 1B — Supabase

| # | Task | Owner | Done when |
|---|------|-------|-----------|
| 1.4 | Create/select Supabase project; note **project ref** | Human | `SUPABASE_PROJECT_REF` known |
| 1.5 | Add GitHub secrets: `SUPABASE_ACCESS_TOKEN`, `SUPABASE_PROJECT_REF` | Human | Secrets set |
| 1.6 | Run migrations | Human | **Actions → Disklordz Supabase migrations** OK, or `apply-supabase-migrations.sh` |
| 1.7 | Confirm RLS: `saved_kits`, credits, storage bucket policies | Human | Auth user can only read own rows |

Migrations (order): listed in [DISKLORDZ_GO_LIVE.md](DISKLORDZ_GO_LIVE.md) §4.

### 1C — Vercel environment

| # | Task | Owner | Done when |
|---|------|-------|-----------|
| 1.8 | Set on Vercel (or sync via workflow): `NEXT_PUBLIC_SUPABASE_URL`, `NEXT_PUBLIC_SUPABASE_ANON_KEY`, `SUPABASE_SERVICE_ROLE_KEY` | Human | Preview + Production env complete |
| 1.9 | Optional: `SAAS_DAILY_GEN_LIMIT` (default 20) | Human | Documented in `.env.example` parity |

Ref: [disklordz/website/DEPLOY.md](../disklordz/website/DEPLOY.md), [DISKLORDZ_GO_LIVE_SECRETS.md](DISKLORDZ_GO_LIVE_SECRETS.md).

### 1D — Stripe

| # | Task | Owner | Done when |
|---|------|-------|-----------|
| 1.10 | Stripe account; create **Pro** price → `STRIPE_PRO_PRICE_ID` | Human | Price id in secrets |
| 1.11 | Register webhook → `https://<DISKLORDZ_URL>/api/stripe/webhook` | Human | `STRIPE_WEBHOOK_SECRET` set |
| 1.12 | Add `STRIPE_SECRET_KEY`, webhook secret, price id to GitHub + Vercel | Human | Checkout + portal routes work in test mode |

Scripts: `disklordz/website/scripts/setup-stripe-webhook.sh`, [STRIPE.md](../disklordz/website/docs/STRIPE.md).

### 1E — Automated go-live run

| # | Task | Owner | Done when |
|---|------|-------|-----------|
| 1.13 | Fill all secrets in [DISKLORDZ_GO_LIVE_SECRETS.md](DISKLORDZ_GO_LIVE_SECRETS.md) | Human | Preflight step passes |
| 1.14 | **Actions → Disklordz go-live** (migrations, sync Vercel, deploy hook, smoke) | Human | Workflow success |
| 1.15 | Set `DISKLORDZ_URL` to production URL (no trailing slash) | Human | Smoke uses correct host |

Local equivalent: `cd disklordz/website && bash scripts/go-live.sh --sync-vercel --stripe-webhook`.

---

## Phase 2 — Production validation (business smoke)

| # | Task | Owner | Done when |
|---|------|-------|-----------|
| 2.1 | `npm run verify:go-live` against prod | Human/Agent | Script exits 0 |
| 2.2 | Magic-link login → `/account` | Human | Session persists |
| 2.3 | Generate one_shot + loop + factory pack; download ZIP | Human | WAVs + manifest valid |
| 2.4 | Pro checkout (test card) → credits/plan update via webhook | Human | Ledger reflects purchase |
| 2.5 | Saved kit appears in history with filters | Human | WO-011 path verified |
| 2.6 | Optional: DAW inbox watcher extracts `disklordz-*.zip` | Human | [daw-inbox README](../disklordz/daw-inbox/README.md) |

Capture failures as GitHub issues with `WO-SAAS-xxx` labels.

---

## Phase 3 — Hardening (before marketing push)

Goal: survive real traffic without double-charges, rate-limit bypass, or silent failures.

| # | Work order (suggested) | Scope | Owner | Done when |
|---|------------------------|-------|-------|-----------|
| 3.1 | **WO-SAAS-006b** | Redis (or Upstash) rate limits replacing in-memory IP cap | Agent | Same limits across serverless instances; tests |
| 3.2 | **WO-SAAS-010b** | Stripe webhook **idempotency** on `event.id` | Agent | Replay webhook does not double-credit |
| 3.3 | **WO-SAAS-017** (new) | Structured logging + error boundaries on `/api/generate` | Agent | Generic client errors; server logs detail |
| 3.4 | **WO-SAAS-018** (new) | Health/readiness route or Vercel monitoring checklist | Agent | Uptime check on `/` + `/api/rate-limit` |
| 3.5 | — | npm audit: plan Next/postcss upgrade (breaking) | Agent | Tracked issue; no silent `--force` on prod branch |

Dependencies: Phase 2 complete (prod to test against).

---

## Phase 4 — RAG v2 (prompt quality)

| # | Work order | Scope | Owner | Done when |
|---|------------|-------|-------|-----------|
| 4.1 | **WO-SAAS-012b** | Supabase **pgvector** migration + chunk table | Agent | Migration in `supabase/migrations/` |
| 4.2 | **WO-SAAS-012c** | `embed_and_upsert.py` + CI re-index on `docs/**` + `disklordz/rag/corpus` | Agent | Action on corpus change |
| 4.3 | **WO-SAAS-012d** | `/api/rag/suggest` uses vector search (fallback keyword) | Agent | Latency &lt; 500ms p95 on prod |
| 4.4 | — | Re-run `./disklordz/scripts/run_qa.sh` + manual prompt UX check | Human | Enhance/Random measurably better |

Ref: [RAG_AND_INTELLIGENT_AUTOMATION.md](RAG_AND_INTELLIGENT_AUTOMATION.md).

---

## Phase 5 — ILLUGEN parity (async + engine swap)

Prioritize from [DISKLORDZ_ILLUGEN_RESEARCH.md](DISKLORDZ_ILLUGEN_RESEARCH.md) once Phase 2–3 stable.

| # | Theme | Scope | Done when |
|---|--------|-------|-----------|
| 5.1 | **Async jobs** | Job table + poll UI; reserve/commit credits | Long gens don’t block HTTP worker |
| 5.2 | **Remote engine** | Gateway API behind `engine-render`; stub → hosted model | Same `GenerationSpec` contract |
| 5.3 | **Variation engine** | “Generate more” adds candidates without full regen | Matches 008 batch semantics |
| 5.4 | **Product factory scale** | Batch/archive pipeline from `sound-factory/` | Large packs off-request |

Each slice = separate WO in Airtable + PR title `WO-SAAS-0xx`.

---

## Phase 6 — Business & compliance (human-led)

| # | Task | Owner | Done when |
|---|------|-------|-----------|
| 6.1 | Privacy policy + Terms (+ cookie notice if analytics) | Human | Linked from site footer |
| 6.2 | Stripe Customer Portal copy + refund policy | Human | Matches actual Pro offering |
| 6.3 | Analytics (Plausible/Posthog/Vercel Analytics) | Human | Conversion funnel visible |
| 6.4 | Support channel (email / Discord) | Human | Linked from `/account` or footer |
| 6.5 | Switch Stripe **test → live** when ready | Human | Live keys only on Production env |

---

## Automation map (already in repo)

| Trigger | What runs |
|---------|-----------|
| PR/push `disklordz/**` | [disklordz-website.yml](../.github/workflows/disklordz-website.yml) — lint, build, RAG smoke |
| Nightly 11:00 UTC | [nightly-qa.yml](../.github/workflows/nightly-qa.yml) — `run_business.py --profile full` |
| Manual | [disklordz-go-live.yml](../.github/workflows/disklordz-go-live.yml) |
| Local | `./disklordz/scripts/run_qa.sh` |

---

## Suggested execution order (single thread)

```text
Phase 0 → Phase 1 (1A→1E) → Phase 2 → Phase 3 → Phase 4 ∥ Phase 6 (legal)
                              ↘ Phase 5 when revenue signal justifies engine cost
```

**Cursor / Cloud Agent:** implement Phases **3–5** as explicit prompts, e.g. *“Implement WO-SAAS-006b per DISKLORDZ_COMPLETION_PLAN Phase 3.1; run run_qa.sh; do not deploy prod.”*

**Human:** Phases **1, 2, 6** and all secrets.

---

## Definition of “Disklordz business complete”

1. Production URL passes **verify:go-live** and manual Phase 2 checklist.  
2. Paying customer path works (test or live Stripe).  
3. Phase 3.1 + 3.2 shipped.  
4. Legal pages live (Phase 6.1–6.2).  

Everything in Phase 4–5 is **growth**, not required for first dollar.
