# Airtable bases (DiskLordz)

Use **one** base as the company OS system of record:

| Base name (Airtable UI) | Base ID | Role |
|-------------------------|---------|------|
| **DiskLordz OS** | `AIRTABLE_BASE_ID` (your dedicated base) | Products, projects, work orders, sound assets, automations — [`base-schema.json`](base-schema.json) |
| DISKLORDZ Factory | `appykxDsfaMFC2hGp` | **Separate** storefront CRM (Products/Sales/Orders/Customers) — do not merge with OS schema |
| Business Idea Tracker (legacy) | `appwaEPUOCXBoq8jB` | Early bootstrap tables; migrate off when dedicated OS base is live |

## Provision dedicated **DiskLordz OS**

1. Set secrets (Cursor environment + optional local `.env`):
   - `AIRTABLE_API_KEY` — PAT with `data.records:*` and `schema.bases:*`
   - `AIRTABLE_BASE_ID` — leave empty until step 3 writes it, or paste an existing empty base ID

2. Allow `api.airtable.com` egress on Cloud Agents (or run locally).

3. Create base + tables + write base ID:

```bash
cd disklordz/automation
cp .env.example .env   # add AIRTABLE_API_KEY only; leave BASE_ID empty
python ../airtable/scripts/create_base_from_schema.py \
  --schema ../airtable/base-schema.json \
  --write-env .env
source .env
python ../airtable/scripts/seed_bootstrap.py
python ../airtable/scripts/verify_connection.py
```

4. Copy `AIRTABLE_BASE_ID` from `.env` into **Cursor secrets** and **GitHub Actions** secrets (Sprint A2).

5. Update [`ACTIVE_BASE.json`](ACTIVE_BASE.json) `base_id` and table IDs (run `python ../airtable/scripts/export_active_base.py` after create — see script).

`create_base_from_schema.py` is idempotent: if a base named **DiskLordz OS** already exists, it reuses that ID and does not duplicate.
