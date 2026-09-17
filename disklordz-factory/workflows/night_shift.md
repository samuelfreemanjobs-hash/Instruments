# Autonomous night shift (with human approval)

All times are illustrative local schedule; orchestrator cron/n8n triggers equivalent jobs.

| Time | Job |
|------|-----|
| 00:00 | Market Intelligence scan |
| 00:30 | A&R concept selection |
| 01:00 | Music Producer briefs |
| 01:30 | Generation / production queue |
| 03:00 | Sound Design supporting assets |
| 04:00 | Sample Miner product candidates |
| 05:00 | Visual Director artwork |
| 05:30 | Content Producer metadata |
| 06:00 | QA (audio, product, visual, rights) |
| 06:30 | Publishing queue prepared |
| 07:00 | **Human approval** — one-click approve/reject |

Nothing publishes externally until approval records `approved_by` and `approved_at` in the catalog.
