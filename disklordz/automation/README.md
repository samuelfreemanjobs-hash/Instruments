# Disklordz automation

## Seed Junova-X work orders (Airtable)

```bash
export AIRTABLE_API_KEY='pat...'
export AIRTABLE_BASE_ID='app...'   # Disklordz OS base

python3 disklordz/automation/scripts/seed_work_orders.py --dry-run
python3 disklordz/automation/scripts/seed_work_orders.py
```

Seed: [`airtable/seed/work-orders-junova-2026.json`](airtable/seed/work-orders-junova-2026.json) — WO-2026-001 … 003.

Then queue GitHub issues (when workflow on `main`):

```bash
gh workflow run airtable-work-order-to-github.yml -f work_order_id=WO-2026-001
```
