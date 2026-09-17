# Disklordz SaaS — v0 definition

**Star product:** prompt-driven **drum sample kit** web app → path to subscription SaaS.  
**PM:** Airtable **Disklordz OS** (not Linear).  
**Implementation:** Cursor Cloud Agent on `Instruments`; work orders drive GitHub issues/PRs.

**Research (PWA × 808 × mobile → MPC):** [DISKLORDZ_PWA_DRUM_SAAS_RESEARCH.md](DISKLORDZ_PWA_DRUM_SAAS_RESEARCH.md)

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
| **Optional:** installable **PWA** shell + playable 808 teaser | Circuit-accurate plugin DSP in browser |

## SaaS ladder (after v0)

| Stage | User gets | Revenue |
|-------|-----------|---------|
| **v0** | Free tier, N gens/day, watermark or small pack | Waitlist / lead magnet |
| **v1** | Paid **Pro**: more gens, larger packs, MPC-ready naming | Stripe / Lemon Squeezy |
| **v2** | **Library** + saved prompts + API key | Monthly subscription |
| **v3** | Team / studio (seats) | Higher tier |

JD Upgraded remains **credibility SKU**; SaaS is **growth SKU**; TR-808 **plugin** is **pro SKU**.

## Repo home

`disklordz/website/` (monorepo) — Next.js or similar; API routes call `disklordz/sound-factory/` when ready.

## Airtable linkage

- **Products** row: `DL-SAAS-DRUM-PROMPT` (see seeds on PM branches)  
- **Agent Work Orders** use `product` field + `WO-YYYY-NNN` in PR titles  

## v0 work order sequence (suggested)

**Blocked until research WOs in PWA brief are Done.**

1. `WO-…-001` — Scaffold web app + deploy preview  
2. `WO-…-002` — Auth + user record  
3. `WO-…-003` — Prompt API stub → static preview WAV  
4. `WO-…-004` — Real generation path (factory script)  
5. `WO-…-005` — ZIP export + provenance manifest  
6. `WO-…-006` — Rate limits + error UX  
7. `WO-…-007` — PWA install + TR-808 teaser integration (if research approves hybrid C)
