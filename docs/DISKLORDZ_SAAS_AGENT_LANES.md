# Parallel agent lanes — Drum SaaS MVP

While **Cursor Cloud** owns `disklordz/website/` through MVP ship, other agents stay in non-conflicting lanes.

| Agent | Status | Do now |
|-------|--------|--------|
| **VST Plugin Factory** (`cursor-vst-plugin-factory`) | **Standing** | `[Plugin][JUCE]` WOs from Airtable → `disklordz/vst-factory/inbox/`; Mon health / Fri doc sync per [VST_PLUGIN_FACTORY_SCHEDULE.md](VST_PLUGIN_FACTORY_SCHEDULE.md) |
| **Workflow Automation Agent** | **Operate** | Owns [vst-plugin-factory-schedule.yml](../.github/workflows/vst-plugin-factory-schedule.yml) + Airtable dispatch; wire Cursor Automations from schedule doc |
| **PM Agent** | **Route** | Assign `owner_agent`; cap 2 JUCE WIPs — [AGENT_TEAM_ROSTER.md](AGENT_TEAM_ROSTER.md) |
| **Cursor (Cloud)** | **Lead** | Finish WO-SAAS-004–006: parametric factory, deploy Vercel, Supabase migration docs, PR #28 to ready |
| **Antigravity (Windows)** | Parallel | `[Plugin][HISE]` WO-2026-HISE-001; run `install-antigravity-workspace.ps1` + `sync-handoff.ps1`; read `disklordz/antigravity/inbox/` |
| **Cline (local IDE)** | Parallel | NTS-1 multi-bass (`cursor/nts1-coder-scaffold-91dc`) **or** OpenClaw artist compile on factory branch — **not** `disklordz/website/` |
| **A&R / OpenClaw artists** | Parallel | Lane guardians + Isaac source sessions; feed **preset copy/tags** for SaaS (already mapped DL001/002/004/006) |
| **Business Planner** | Gate | Approve Vercel project + Supabase prod keys; merge PR #28; open WO for v1 billing later |
| **Marketing** | Parallel | Landing copy, MPC handoff tutorial, `#disklordz-product` launch thread |
| **THOR / Executive** | Sync | Unblock env secrets (Supabase, Vercel) — not implementation |
| **Bytebot (local Docker)** | Optional | GUI/desktop/browser tasks on **your machine** — see [BYTEBOT_SETUP.md](BYTEBOT_SETUP.md); not in Cloud Agent VM |

**WIP rule:** Max **2** Cursor JUCE WOs on plugin factory; **SaaS does not count** against HISE sketch WIP.
