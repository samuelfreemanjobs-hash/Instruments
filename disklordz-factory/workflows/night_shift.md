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

## Automation

- **In-process runner:** `POST /night-shift/run` on Factory API (simulates all schedule steps, creates batch assets, fills approval queue).
- **n8n:** import [night_shift_n8n.json](night_shift_n8n.json) and point the HTTP node at your API base URL.
- **Operator UI:** dashboard **Run night shift** button → morning **Approval queue**.
