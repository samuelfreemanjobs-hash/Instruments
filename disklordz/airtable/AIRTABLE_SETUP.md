# Airtable setup — DiskLordz OS

Airtable is the **system of record** for products, work orders, assets, and automations.

## 1. Create a Personal Access Token (PAT)

1. Open [airtable.com/create/tokens](https://airtable.com/create/tokens)  
2. Name: `DiskLordz Cursor`  
3. Scopes (minimum):
   - `data.records:read`
   - `data.records:write`
   - `schema.bases:read`
   - `schema.bases:write`
4. Access: **All current and future bases** (or restrict to one workspace and add bases later)  
5. Copy the token — shown once.

## 2. Option A — Automated base (recommended)

```bash
cd disklordz/automation
cp .env.example .env
# Edit .env: set AIRTABLE_API_KEY=pat...

python3 -m venv .venv && source .venv/bin/activate
pip install -r requirements.txt

python ../airtable/scripts/create_base_from_schema.py \
  --schema ../airtable/base-schema.json \
  --write-env ../automation/.env
```

The script:

- Creates base **DiskLordz OS** (or `--base-name`)  
- Creates all tables + select options  
- Link fields between tables are **text IDs** first (`*_link`); optional `--add-record-links` pass adds true linked-record fields (see script help)  
- Writes `AIRTABLE_BASE_ID=app...` into `.env`  

Then seed bootstrap data:

```bash
python ../airtable/scripts/seed_bootstrap.py
```

## 3. Option B — Manual base

1. Create base **DiskLordz OS** in Airtable UI.  
2. For each table in `base-schema.json`, create fields per [`FIELD_GUIDE.md`](FIELD_GUIDE.md).  
3. Copy **Base ID** from URL: `https://airtable.com/appXXXXXXXX/...` → `appXXXXXXXX`  
4. Set in `disklordz/automation/.env`:

```bash
AIRTABLE_API_KEY=pat...
AIRTABLE_BASE_ID=app...
```

5. Run `python ../airtable/scripts/verify_connection.py`  
6. Run `seed_bootstrap.py`

## 4. Verify

```bash
python ../airtable/scripts/verify_connection.py
python scripts/airtable_client.py list "Product Families" --max 3
```

## 5. Cursor Cloud Agent secrets

In Cursor **environment secrets** (not git), set:

- `AIRTABLE_API_KEY`  
- `AIRTABLE_BASE_ID`  

Redeploy or restart agents so Audio PM and Automation Engineer can read/write.

## 6. Zapier MCP (optional)

Your Zapier account already has **Airtable** connected. Agents can use:

- `airtable_get_all_records` — reads  
- `airtable_create_record` / `airtable_update_record` — writes (confirm destructive ops)  

Use the same base selected in Zapier when configuring actions.

## 7. Recommended views (create in UI)

| Table | View |
|-------|------|
| Agent Work Orders | Kanban by `status` |
| Products | Grid filtered `status != Released` |
| Projects | Active only |
| Automations | `status != Live` for backlog |
| Market Signals | `status = New` |

## 8. Done when

- [ ] `verify_connection.py` prints all table names  
- [ ] Seed row: `DL-FAMILY-DIGITAL-SAMPLER` exists  
- [ ] Project `PRJ-2026-001` + Sprint A automation rows exist  
- [ ] `.env` or Cursor secrets set (never committed)  

Next: **Workflow Automation Engineer** — Sprint A (WO → GitHub issue).
