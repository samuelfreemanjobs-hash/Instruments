# Parallel agent lanes — Drum SaaS MVP

While **Cursor Cloud** owns `disklordz/website/` through MVP ship, other agents stay in non-conflicting lanes.

| Agent | Status | Do now |
|-------|--------|--------|
| **Cursor (Cloud)** | **Lead** | Finish WO-SAAS-004–006: parametric factory, deploy Vercel, Supabase migration docs, PR #28 to ready |
| **Antigravity (Windows)** | Parallel | `[Plugin][HISE]` WO-2026-HISE-001; run `install-antigravity-workspace.ps1` + `sync-handoff.ps1`; read `disklordz/antigravity/inbox/` |
| **Cline (local IDE)** | Parallel | NTS-1 multi-bass (`cursor/nts1-coder-scaffold-91dc`) **or** OpenClaw artist compile on factory branch — **not** `disklordz/website/` |
| **Head of A&R (CROW)** | **Lead** | Lane integrity, kit QC, references — [DISKLORDZ_AR_DEPARTMENT.md](DISKLORDZ_AR_DEPARTMENT.md) |
| **Artist lane agents** | Under CROW | `artist-midnight-circuit`, `artist-boulevard-86`, `artist-disklordz-screw`, `artist-terminal-mirage` · `disklordz/ar/subagents/` |
| **OpenClaw / Isaac sessions** | Parallel | Local compile + session notes → `disklordz/ar/sessions/` → reference curator |
| **PM Agent** | **Lead** | Airtable Disklordz OS — `product_id`, WOs, ship dates, acceptance (pairs with VARA) |
| **Business Planner** | Gate | Approve Vercel project + Supabase prod keys; merge PR #28; open WO for v1 billing later |
| **Marketing Director (VARA)** | **Lead** | GTM system, funnels, campaigns — [DISKLORDZ_MARKETING_DIRECTOR.md](DISKLORDZ_MARKETING_DIRECTOR.md) · Slack `#disklordz-marketing-dept` |
| **Product Launch Manager (HELM)** | **Lead** | Launch calendar, asset gates — [DISKLORDZ_PRODUCT_LAUNCH_MANAGER.md](DISKLORDZ_PRODUCT_LAUNCH_MANAGER.md) |
| **Workflow Automation (FORGE)** | **Lead** | Cron, queues, agent registry, event bus — [DISKLORDZ_WORKFLOW_AUTOMATION_AGENT.md](DISKLORDZ_WORKFLOW_AUTOMATION_AGENT.md) |
| **Chief of Staff (SAGE)** | **Lead** | Routes exec requests; Slack EA — [DISKLORDZ_CHIEF_OF_STAFF.md](DISKLORDZ_CHIEF_OF_STAFF.md) · `#disklordz-exec` |
| **Marketing subagents** | Under VARA | `mkt-funnel`, `mkt-lifecycle`, `mkt-content`, `mkt-brand`, `mkt-perf`, `mkt-launch`, `mkt-commerce` — see `disklordz/marketing/subagents/` |
| **THOR / Executive** | Sync | Unblock env secrets (Supabase, Vercel) — not implementation |
| **Bytebot (local Docker)** | Optional | GUI/desktop/browser tasks on **your machine** — see [BYTEBOT_SETUP.md](BYTEBOT_SETUP.md); not in Cloud Agent VM |

**WIP rule:** Max **2** Cursor JUCE WOs on plugin factory; **SaaS does not count** against HISE sketch WIP.
