# DiskLordz Audio Command (PWA + CLI)

## Purpose

Operator cockpit for the **DiskLordz Audio** commercial roadmap — sprint checklist, year-one milestones, weekly reviews, and fleet advisor briefings. This is **not** the VST engineering lane (that stays in Cursor + `golden-wav-qa`).

**Business copilot agent:** [audio-business-copilot](../agents/profit/audio-business-copilot/agent.md)

## Build & run

```bash
# Weekly review markdown (stdout or --write to reviews/)
python3 disklordz/audio-command/scripts/weekly_review.py
python3 disklordz/audio-command/scripts/weekly_review.py --write

# Which profit agents to consult this week
python3 disklordz/audio-command/scripts/fleet_briefing.py

# Serve PWA locally (any static server)
python3 -m http.server 8765 --bind 127.0.0.1 -d disklordz/audio-command/pwa
# open http://127.0.0.1:8765/
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
