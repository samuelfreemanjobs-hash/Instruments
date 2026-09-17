# Research before anyone speaks

DiskLordz Factory **does not** let Market Intel or A&R invent lineups, demand, or artist greenlights in Slack or the dashboard.

## Gate

Production-facing claims require:

1. **`POST /research/opportunities`** — at least one `DL-OPP-*` with **`sources[]`** (URLs or citations) and a **score** (0–100).
2. **`POST /research/aar-decisions`** — greenlight ties an **`artist_id`** to an **`opportunity_id`** with a **rationale** (min 10 chars). Score must be ≥ 50 to greenlight.

Check: **`GET /research/status`** → `ready_for_production: true` only when both exist.

## Slack behaviour

| Agent | Without research | With research |
|--------|------------------|---------------|
| Market Intel | “No market claims — submit DL-OPP” | Lists sourced records + links |
| A&R | “No artist lineup announced” | Greenlights/rejects with rationale only |
| Producer | Held | Briefs after greenlight |

Night shift still builds **technical QA assets**; titles use **unassigned lane** until artists are greenlit.

## Example (curl)

```bash
curl -s -X POST http://127.0.0.1:8787/research/opportunities -H 'Content-Type: application/json' -d '{
  "title": "Dark 90s digital phonk — beatmaker kits",
  "genre": "phonk",
  "subgenre": "memphis",
  "bpm_min": 140,
  "bpm_max": 155,
  "search_demand": "high",
  "competition": "medium",
  "score": 72,
  "sources": [
    "https://example.com/youtube-trend-note",
    "internal: Airtable scan 2026-09-17"
  ]
}'

curl -s -X POST http://127.0.0.1:8787/research/aar-decisions -H 'Content-Type: application/json' -d '{
  "opportunity_id": "DL-OPP-000001",
  "artist_id": "DL002",
  "decision": "greenlight",
  "rationale": "Phonk lane matches DL002 palette; kit + Shorts flywheel; sources reviewed."
}'
```

Then run night shift — Slack will show **sourced** intel and **named** greenlights only.

## Your backup (Slack / MCP)

Use **Slack** or your research stack to drop source links in `#disklordz-factory`; ingest them via the API above (or a future Airtable sync). Agents stay quiet until that ingest happens.

### One-shot: four YouTube brands (DL001, DL002, DL006, DL004)

With Factory API running on `:8787`:

```bash
chmod +x disklordz-factory/scripts/ingest-youtube-network-research.sh
./disklordz-factory/scripts/ingest-youtube-network-research.sh
```

Optional: copy [`youtube_research_sources.example.json`](../disklordz-factory/database/youtube_research_sources.example.json) to `disklordz-factory/database/youtube_research_sources.local.json` (gitignored), paste your Slack/Airtable URLs per `YT-DL-001`…`004`, then re-run the script (auto-picked up by the shell wrapper).

```bash
# Preview payloads only
python3 disklordz-factory/scripts/ingest_youtube_network_research.py --dry-run

# Opportunities only, no A&R greenlight
python3 disklordz-factory/scripts/ingest_youtube_network_research.py --no-greenlight
```

Seed payloads: [`disklordz-factory/database/youtube_research_seed.json`](../disklordz-factory/database/youtube_research_seed.json). Replace `internal: REPLACE` lines in sources before treating greenlights as production-ready.

See also [FACTORY_SLACK.md](FACTORY_SLACK.md).
