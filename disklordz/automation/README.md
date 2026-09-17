# Disklordz automation

## Seed PM bundle (Products + Projects + Work Orders)

```bash
export AIRTABLE_API_KEY='pat...'
export AIRTABLE_BASE_ID='app...'   # Disklordz OS base

python3 disklordz/automation/scripts/seed_airtable_bundle.py --dry-run
python3 disklordz/automation/scripts/seed_airtable_bundle.py
```

Seed: [`airtable/seed/plugin-tracks-2026.json`](airtable/seed/plugin-tracks-2026.json) — **Junova-X**, **NovaDrum**, **Drum SaaS** products; projects; WO-2026-001 … 003.

Requires **Product Families** row `DL-FAMILY-DIGITAL-SAMPLER` (from full `bootstrap.json` on PR #9) or create that family manually first.

Work orders only (legacy): `seed_work_orders.py` + `work-orders-junova-2026.json`.

Then queue GitHub issues (when workflow on `main`):

```bash
gh workflow run airtable-work-order-to-github.yml -f work_order_id=WO-2026-001
```
