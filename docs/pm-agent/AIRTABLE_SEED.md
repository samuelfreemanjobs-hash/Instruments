# Airtable seed — Disklordz first run

**Preferred:** [CLAUDE_SETUP.md](CLAUDE_SETUP.md) — MCP config + Project paste bundle.

**Or terminal:** `python3 scripts/sync_disklordz_pm_to_airtable.py` (needs `AIRTABLE_API_KEY`).

**Or Claude chat** (with Airtable MCP connected):

---

## 1. `PM ADD: Disklordz — phonk factory & GRAVEYARD_SHIFT v1`

```
Urgency: HIGH
Status: ACTIVE
Current Phase: PHASE-1
% Complete: 25
Scope: Active math factory (tools/render_kit.py), SQLite sample_dna, kit v1 GRAVEYARD_SHIFT (OG Memphis phonk + gritty trap). Master plan: Instruments repo docs/DISKLOrdZ_FACTORY_PLAN.md. Hardware capture optional. No autonomous swarms.
Proposed Outcome: Ship GRAVEYARD_SHIFT drum kit ZIP with LICENSE + demo track.
For Who: Phonk/trap producers wanting SP-1200 style 808 knock
Opportunity Value: First Disklordz commercial SKU + VSTi path
Next Action: Listen to GRAVEYARD_SHIFT batch in DAW; sort 26 WAVs into 01_Kicks/02_808s; tweak synthesis.py if needed
Stack:
Python factory
tools/disklordz
SQLite
JUCE 7 (VSTi phase 2)
Gumroad/BeatStars TBD
Notes: Factory kickoff 2026-09-16 — 6 starter + 20 grid 808s in tools/output/samples.
```

---

## 2. `PM UPDATE: PM Agent — Proactive Project Intelligence`

Rename / re-scope for Disklordz:

```
Project Name → Disklordz — Audio PM Agent
For Who → Sam / Disklordz — audio product operations
Scope → Airtable PM for Disklordz: factory, packs, JUCE plugins, releases. Prompt: docs/pm-agent/DISKLOrdZ_AUDIO_PM_AGENT.md
% Complete → 85
Next Action → Run PM BRIEFING weekly; register Disklordz factory project; exclude non-audio FI projects from Disklordz briefings when Sam requests filter
Notes → Rebranded from Freeman Intelligence PM Agent 2026-09-16.
```

---

## 3. `PM ADD: Disklordz — Activepieces automation (self-host)`

Or run `python3 scripts/sync_disklordz_pm_to_airtable.py` after `AIRTABLE_API_KEY` is in GitHub secrets (sync upserts this row).

```
Urgency: HIGH
Status: ACTIVE
Current Phase: PHASE-1
% Complete: 10
Scope: Self-hosted Activepieces for factory webhooks (Slack/Airtable). infra/activepieces/automate.sh
Proposed Outcome: Docker + provisioned factory webhook; render_kit auto-notify
For Who: Sam / Disklordz studio
Next Action: Set DISKLORDZ_AP_ADMIN_* in infra/activepieces/.env and run ./automate.sh
Stack:
Docker Compose
Activepieces
GitHub Actions (optional)
API Keys Needed: DISKLORDZ_AP_ADMIN (local .env); GitHub AIRTABLE_API_KEY, DISKLORDZ_ACTIVEPIECES_WEBHOOK (optional)
Notes: Checklist in scripts/sync_disklordz_pm_to_airtable.py ACTIVEPIECES_NOTES — merge PR #12 first
```

---

## 4. `PM BRIEFING`

After ADD + UPDATE, send **`PM BRIEFING`** to verify.

Optional: archive or tag legacy Freeman-only projects so Disklordz briefings stay focused.
