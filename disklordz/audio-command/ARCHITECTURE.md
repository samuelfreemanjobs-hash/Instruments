# DiskLordz Audio Command (PWA + CLI)

## Purpose

**Coach** surfaces **one task at a time** (playbooks + outreach). You should not hunt the checklist — the app and agent present the next step and nudge you via notifications, calendar, or Slack webhook.

This is **not** the VST engineering lane (that stays in Cursor + `golden-wav-qa`).

**Business copilot agent:** [audio-business-copilot](../agents/profit/audio-business-copilot/agent.md)

## Build & run

```bash
# What you should do RIGHT NOW (agent + webhooks use this)
python3 disklordz/audio-command/scripts/coach_engine.py

# Push to Slack/Discord (set DISKLORDZ_COACH_WEBHOOK_URL)
python3 disklordz/audio-command/scripts/coach_outreach.py --send

# Weekly review markdown (stdout or --write to reviews/)
python3 disklordz/audio-command/scripts/weekly_review.py --write

# Phone calendar subscribe
python3 disklordz/audio-command/scripts/coach_calendar.py --write

# Serve PWA locally (any static server)
python3 -m http.server 8765 --bind 127.0.0.1 -d disklordz/audio-command
# open http://127.0.0.1:8765/pwa/
```

## Data flow

```text
Master docs (docs/DISKLORDZ_*.md)
  ↔ data/sprint-checklist.json (canonical checklist)
  ↔ data/milestones-year1.json
  ↔ browser localStorage (operator toggles)
  ↔ optional data/progress.json (committed snapshot when user exports)

weekly_review.py
  → reads checklist + milestones + progress
  → writes reviews/YYYY-Www.md

fleet_briefing.py
  → maps current phase → advisor agents from fleet-advisors.json
```

## Key modules

| Path | Role |
|------|------|
| `pwa/` | Installable checklist UI (offline-capable) |
| `data/` | Canonical JSON for checklist and milestones |
| `scripts/weekly_review.py` | Generate weekly development review |
| `scripts/fleet_briefing.py` | Fleet agent invocation hints |

## Extension points

- Sync `progress.json` to Supabase for multi-device (future)
- Embed PWA in `disklordz/website` under `/audio-command` when store ships
- Wire `weekly_review.py` to GitHub Actions artifact on schedule

## Related docs

- [docs/DISKLORDZ_MASTER_PLAN.md](../../docs/DISKLORDZ_MASTER_PLAN.md)
- [docs/CHORD_ENGINE_PRD.md](../../docs/CHORD_ENGINE_PRD.md)
- [DISKLORDZ_AGENTS.md](../../DISKLORDZ_AGENTS.md)
