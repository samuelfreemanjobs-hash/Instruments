# Parallel agent lanes — Drum SaaS MVP

While **Cursor Cloud** owns `disklordz/website/` through MVP ship, other agents stay in non-conflicting lanes.

| Agent | Status | Do now |
|-------|--------|--------|
| **Cursor (Cloud)** | **Lead** | Finish WO-SAAS-004–006: parametric factory, deploy Vercel, Supabase migration docs, PR #28 to ready |
| **Factory DSP Engineer (Cloud, daily)** | **Automated** | WO-SAAS-018: [DISKLORDZ_FACTORY_DAILY_AGENT.md](DISKLORDZ_FACTORY_DAILY_AGENT.md) |
| **SaaS Ops Guardian (Cloud, daily)** | **Automated** | WO-SAAS-019: health + go-live — [DISKLORDZ_BUSINESS_AGENTS.md](DISKLORDZ_BUSINESS_AGENTS.md) |
| **Billing Integrity (Cloud, weekly)** | **Automated** | WO-SAAS-020: Stripe/credits audit |
| **Growth & Lane Marketing (Cloud, weekly)** | **Automated** | WO-SAAS-021: drafts → `disklordz/marketing/inbox/` |
| **PM / WO Router (Cloud, dispatch)** | **Automated** | WO-SAAS-022: `disklordz-pm-router` dispatch |
| **Preset Lane Curator (Cloud, weekly)** | **Automated** | WO-SAAS-023: presets ↔ PRESET_BASE |
| **Antigravity (Windows)** | Parallel | `[Plugin][HISE]` WO-2026-HISE-001; run `install-antigravity-workspace.ps1` + `sync-handoff.ps1`; read `disklordz/antigravity/inbox/` |
| **Cline (local IDE)** | Parallel | NTS-1 multi-bass (`cursor/nts1-coder-scaffold-91dc`) **or** OpenClaw artist compile on factory branch — **not** `disklordz/website/` |
| **A&R / OpenClaw artists** | Parallel | Lane guardians + Isaac source sessions; feed **preset copy/tags** for SaaS (already mapped DL001/002/004/006) |
| **Business Planner** | Gate | Approve Vercel project + Supabase prod keys; merge PR #28; open WO for v1 billing later |
| **Marketing (human)** | Review | Publish drafts from WO-SAAS-021 inbox; MPC tutorial, `#disklordz-product` |
| **THOR / Executive** | Sync | Unblock env secrets (Supabase, Vercel) — not implementation |
| **Bytebot (local Docker)** | Optional | GUI/desktop/browser tasks on **your machine** — see [BYTEBOT_SETUP.md](BYTEBOT_SETUP.md); not in Cloud Agent VM |

**WIP rule:** Max **2** Cursor JUCE WOs on plugin factory; **SaaS does not count** against HISE sketch WIP.

**Master index:** [DISKLORDZ_BUSINESS_AGENTS.md](DISKLORDZ_BUSINESS_AGENTS.md) (schedules, scripts, secrets).
