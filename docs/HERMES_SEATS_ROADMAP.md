# Hermes seats — roadmap & recommendations

**Goal:** Extend the default dev team without duplicating Grok, human gates, or Cursor Cloud itself.

## Your three proposals

| Proposed seat | Recommendation | Hermes name | Notes |
|---------------|----------------|-------------|--------|
| **Business ops agent** | **Yes — narrow scope** | `hermes-ops` | Airtable WO ↔ GitHub issue hygiene, seed scripts (`disklordz/automation/`), WIP cap (2 JUCE WOs), Done checklist. **Not** pricing/GTM approval — that stays **Business Planner + Marketing** (human). **No** autonomous Airtable/Stripe writes without explicit user confirm. |
| **VS Code handoff agent** | **Yes — rename for clarity** | `hermes-handoff` | Covers **Antigravity** (`disklordz/antigravity/inbox/`), **local IDE** (Cline, VS Code, Cursor desktop) ↔ Cloud. Uses git handoff JSON + `scripts/antigravity-bridge/`. Not a remote IDE login. |
| **GitHub cloud agent** | **Partially — don’t duplicate Cloud** | `hermes-devops` | Cursor **Cloud Agent already is** the GitHub implementation runtime. Add a seat for **platform**: Actions/workflows, CI triage, branch protection, `gh` **read** + workflow PRs, PR template/CI comments. Lead still owns feature PRs; devops owns **pipeline** PRs. |

## Tier model (avoid seat sprawl)

### Tier 1 — Core implement (installed)

`lead` · `architect` · `dsp` · `gui` · `web` · `qa`

### Tier 2 — Platform & coordination (recommended next)

| Seat | Helps with |
|------|------------|
| **hermes-devops** | `build.yml`, nightly QA, pluginval matrix, Vercel/Supabase **docs** (not prod deploy), ci-investigator-style summaries |
| **hermes-handoff** | HISE/Antigravity HO files, Windows lane, local↔cloud branch sync |
| **hermes-ops** | Airtable bundle seed, WO numbering, Factory Manager WIP, Grok→Cursor dispatch checklist |

### Tier 3 — Revenue & creative (mostly Grok + human)

| Seat | Helps with |
|------|------------|
| **hermes-gtm** | Store copy, `$29→$49` ladders, landing alignment (`junova-x-landing`) — **draft only** |
| **hermes-presets** | Factory preset banks, A&R lane tags for SaaS kits, XML/JSON preset pipelines |
| **hermes-support** | RAG corpus chunks, FAQ macros, `docs/lanes/*` — pairs with WO-SAAS-012 |

Use Tier 3 when Grok + Marketing bandwidth is the bottleneck; otherwise Grok closed-loop is enough.

### Keep **outside** Hermes (already defined)

| Role | Why |
|------|-----|
| **Grok Bot** | Spec + WO + verify loop — not a Cursor seat |
| **Business Planner / Marketing** | Human gates for SKU and launch |
| **Antigravity (Windows)** | Executes HISE lane locally |
| **A&R / OpenClaw artists** | Creative source, not repo agents |

## Other seats worth adding later

| Seat | When it pays off |
|------|------------------|
| **hermes-security** | On-demand review before ship (secrets, API authZ, RLS) — use explicit `/security review` or subagent |
| **hermes-research** | ILLUGEN 007+, Colab spikes, competitor matrices → WO drafts only |
| **hermes-data** | Supabase migrations, pgvector RAG schema — overlaps `web` until DB work is heavy |

## Decision rule

Add a new Hermes seat when **all** are true:

1. Recurring work type (weekly+)  
2. Distinct skill file + evidence type  
3. Not already covered by lead + existing seat  
4. Clear **read vs write** boundary (writes need human confirm for ops/GTM)

## Related

- [HERMES_AGENT_FRAMEWORK.md](HERMES_AGENT_FRAMEWORK.md)
- [HERMES_QUICKSTART.md](HERMES_QUICKSTART.md)
- [DISKLORDZ_SAAS_AGENT_LANES.md](DISKLORDZ_SAAS_AGENT_LANES.md)
- [GROK_CLOSED_LOOP_ENGINE.md](GROK_CLOSED_LOOP_ENGINE.md)
