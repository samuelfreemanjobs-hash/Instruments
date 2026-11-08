# Operator autopilot — “I don’t know how” → agent-run

Run **Cursor Cloud Agent** on **Instruments** from plain-English tasks. One intake path (GitHub Issues), a simple router, existing fleet skills, and **META R10** gates for money and production.

**Setup / go-live (Slack, secrets, Airtable, stubs, Windows MPC):** [OPERATOR_OPS_MASTER_CHECKLIST.md](OPERATOR_OPS_MASTER_CHECKLIST.md)

**Related:** [disklordz/sops/SOP-018-operator-autopilot.md](disklordz/sops/SOP-018-operator-autopilot.md) · [disklordz/templates/TEMPLATE-cloud-agent-prompt.md](disklordz/templates/TEMPLATE-cloud-agent-prompt.md) · [disklordz/sops/SOP-001-cloud-agent-task.md](disklordz/sops/SOP-001-cloud-agent-task.md) · [STUB_AGENT_BLOCKERS.md](../disklordz/agents/workflows/STUB_AGENT_BLOCKERS.md)

---

## Purpose

| You | System |
|-----|--------|
| Describe what you need in normal language | Issue label **`agent-run`** |
| Don’t write a full spec | Template + Cloud Agent prompt block |
| Avoid surprise prod / Stripe | Agent **stops** at R10 gates and documents your clicks |

This is **not** a second control plane (no Paperclip, ceo, or extra repos required).

---

## Intake (30 seconds)

1. GitHub → **Instruments** → **Issues** → **New issue** → **Agent run (operator autopilot)**.
2. Fill the form; submit. Issue gets labels **`agent-run`** (and optional tags).
3. Copy the **Cloud Agent prompt** section from the issue into [Cursor Cloud](https://cursor.com/agents) (or IDE Agent) linked to this repo.

**Alternative:** paste the same fields into Cursor without an issue—issues are for **traceability** and weekly review.

---

## Router — where work goes

| Task smells like… | Executor | Proof |
|-------------------|----------|-------|
| Next.js, API, factory, RAG, fleet docs in repo | **Cursor Cloud** on `Instruments` | `npm run build`, tests, PR + artifacts |
| Plugin / DSP in monorepo | **Cursor Cloud** + C++ lane | `run_business.py --profile ci` |
| Prod deploy, migrations, Vercel env | **You** add secrets once → Actions **`disklordz-go-live`** | [OPERATOR_OPS_MASTER_CHECKLIST.md](OPERATOR_OPS_MASTER_CHECKLIST.md) Phase B · [DISKLORDZ_GO_LIVE.md](DISKLORDZ_GO_LIVE.md) |
| Slack CI, Airtable handoff, Windows inbox setup | **You** (HUMAN steps) | [OPERATOR_OPS_MASTER_CHECKLIST.md](OPERATOR_OPS_MASTER_CHECKLIST.md) Phases A, C, E |
| Stub agents (billing, Inngest, n8n…) | Fix secrets in [DISKLORDZ_GO_LIVE_SECRETS.md](DISKLORDZ_GO_LIVE_SECRETS.md) / blockers doc | `stub-agent-env-check.sh` |
| HISE / Windows sketch | **Antigravity** handoff | `docs/HISE_ANTIGRAVITY_LANE.md` |
| Greenfield JUCE SKU | **`audio-plugin-coder`** (separate clone) | APC ship phase |
| Browser vendor portal (no API) | **Bytebot** (local Docker) | `docs/BYTEBOT_SETUP.md` |
| Kits → MPC folders | **`disklordz/daw-inbox`** on Windows | Not an LLM |

**Profit-agent hint:** see [DISKLORDZ_AGENTS.md](../DISKLORDZ_AGENTS.md). Mention agent id in the issue (e.g. `product-factory`, `daw-inbox-copilot`, `conversion-qa`).

---

## Cloud Agent prompt (paste into every run)

```markdown
Operator autopilot — issue #<N> (or ad hoc).

I don't know how to do: <from issue>
Goal: <from issue>
Success criteria: <checkboxes from issue>

Read: ARCHITECTURE.md, AGENTS.md, docs/OPERATOR_AUTOPILOT.md
Product ARCHITECTURE if applicable (e.g. disklordz/website/ARCHITECTURE.md)
Branch: cursor/<short-desc>-<suffix>

Stop and list blockers (no self-confirm) if:
- Production Stripe/billing or live customer data
- Supabase production schema change
- Merge to main or production deploy (unless issue explicitly allows)
- Dependency removal or force-push

Deliverable: push branch, draft PR, evidence (logs/screenshot/video).
```

Full template: [TEMPLATE-cloud-agent-prompt.md](disklordz/templates/TEMPLATE-cloud-agent-prompt.md).

---

## Gates (agent must stop; human approves)

Aligned with [CLAUDE.md](../CLAUDE.md) **R10**:

- Schema change on production Supabase
- Production Stripe secrets / live mode charges
- Public API or contract break
- Deploy to production / merge (unless issue says otherwise)
- Writes that need Zapier/MCP **write** without explicit approval in the issue

Acceptable automated output when blocked: **step-by-step dashboard instructions**, which GitHub secrets to add, and a **draft PR** that does not touch prod.

---

## Weekly operator ritual (~20 min)

1. Triage open issues labeled **`agent-run`** (close duplicates; add `needs-secrets` where blocked).
2. Run **one** Cloud Agent per issue (serial is fine).
3. Review draft PRs; merge when CI and artifacts match success criteria.
4. **Actions:** `agent-fleet-health`, `agent-fleet-execute` on `main` if you changed fleet.
5. Regenerate stub report:

   ```bash
   bash disklordz/agents/workflows/scripts/stub-agent-env-check.sh
   ```

   Fix **one** blocker category from [STUB_AGENT_BLOCKERS.md](../disklordz/agents/workflows/STUB_AGENT_BLOCKERS.md).

---

## What we deliberately do not use for this

| Tool | Why skip for autopilot |
|------|-------------------------|
| Paperclip / ceo | Second control plane; fleet + Issues enough |
| Etsy / “digital product” agent forks | Wrong product shape |
| Ralph | Optional local grind; Cloud + template is default |
| 100+ meta-agent forks | Noise |

---

## Extension points

- **Airtable WO:** use [SOP-006](disklordz/sops/SOP-006-work-order-to-pr.md); add label `agent-run` on linked GitHub issue.
- **Slack:** notify on issue open via existing CI/integrations when configured.
- **Scheduled Cloud runs:** Cursor Cloud API — only after intake + router are stable.
