# Disklordz SaaS — v0 definition

**Star product:** prompt-driven **drum sample kit** web app → path to subscription SaaS.  
**PM:** Airtable **Disklordz OS** (not Linear).  
**Implementation:** Cursor Cloud Agent on `Instruments`; work orders drive GitHub issues/PRs.

## v0 promise (one sentence)

> Type a vibe → hear a short preview → download a small WAV pack (or save to account).

No DAW required. No “infinite catalog” in v0.

## v0 scope (ship this, nothing else)

| In v0 | Out of v0 |
|-------|-----------|
| Email or magic-link **account** (Supabase Auth / Clerk — pick one in WO) | Full subscription billing |
| **Prompt** + 3–5 **style presets** (genre/machine tags) | Every drum machine in archive |
| **Preview** player (4–8 one-shots or one loop) | DAW plugin export |
| **ZIP download** with manifest JSON | MPC/Maschine project export |
| **Provenance** flag per asset (hash / source id) | AI training on user uploads |
| Deployed **HTTPS** URL (Vercel or similar) | Native mobile app |
| Basic **usage limits** (rate limit / daily gens) | Team seats |

## SaaS ladder (after v0)

| Stage | User gets | Revenue |
|-------|-----------|---------|
| **v0** | Free tier, N gens/day, watermark or small pack | Waitlist / analytics only |
| **v1** | Paid **Pro**: more gens, larger packs, no watermark | Stripe / Lemon Squeezy |
| **v2** | **Library** + saved prompts + API key | Monthly subscription |
| **v3** | Team / studio (seats) | Higher tier |

JD Upgraded remains **credibility SKU**; SaaS is **growth SKU**.

## Repo home

`disklordz/website/` (monorepo) — Next.js or similar; API routes call `disklordz/sound-factory/` when ready.

## Airtable linkage

- **Products** row: `Disklordz Drum SaaS`  
- **Agent Work Orders** use `product` field + `WO-YYYY-NNN` in PR titles  
- Automations: [disklordz/automation/README.md](../disklordz/automation/README.md) (when present on branch)

## v0 work order sequence (suggested)

1. `WO-SAAS-001` — Scaffold web app + deploy preview  
2. `WO-SAAS-002` — Auth + user record  
3. `WO-SAAS-003` — Prompt API stub → static preview WAV  
4. `WO-SAAS-004` — Real generation path (factory script)  
5. `WO-SAAS-005` — ZIP export + provenance manifest  
6. `WO-SAAS-006` — Rate limits + error UX  

**007+** (ILLUGEN-shaped roadmap): [DISKLORDZ_ILLUGEN_RESEARCH.md](DISKLORDZ_ILLUGEN_RESEARCH.md) — generation spec, variations, async jobs, credits, RAG, product factory.

**Agent / deploy / RAG docs:** [CURSOR_AGENT_PLAYBOOK.md](CURSOR_AGENT_PLAYBOOK.md) · [RAG_AND_INTELLIGENT_AUTOMATION.md](RAG_AND_INTELLIGENT_AUTOMATION.md) · [COLAB_ZERO_INSTALL_TESTING.md](COLAB_ZERO_INSTALL_TESTING.md) · [AGENTIC_PROJECT_STANDARDS.md](AGENTIC_PROJECT_STANDARDS.md)

Each WO: Airtable → GitHub issue (A1) → PR with `WO-…` in title → merge → Airtable Done (A2).

## MVP completion checklist

| Item | Status |
|------|--------|
| Prompt + presets | Done |
| Parametric factory (prompt affects audio) | Done (`factory_parametric_v1`) |
| Preview + ZIP + manifest | Done |
| Auth + saved kits | Done (requires Supabase env + SQL) |
| Rate limit UX | Done (IP / in-memory) |
| Deploy HTTPS | **You:** Vercel root `disklordz/website` + env vars |

Parallel work: [DISKLORDZ_SAAS_AGENT_LANES.md](DISKLORDZ_SAAS_AGENT_LANES.md).
