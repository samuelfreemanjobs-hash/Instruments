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

## 3. Dedicated base (required)

DiskLordz must use its **own** base — **DiskLordz OS** — not mixed with other workspaces. See [`BASES.md`](BASES.md).

| Secret | What to set |
|--------|-------------|
| `AIRTABLE_API_KEY` | Your PAT |
| `AIRTABLE_BASE_ID` | ID of **DiskLordz OS** after `create_base_from_schema.py` (or an empty base you created in the UI) |

Legacy bootstrap data may still exist under **`appwaEPUOCXBoq8jB`** (*Business Idea Tracker*); migrate or re-seed into the dedicated base.

## 4. Option B — Manual base (greenfield)

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

## 5. Verify

```bash
python ../airtable/scripts/verify_connection.py
python scripts/airtable_client.py list "Product Families" --max 3
```

## 6. Cursor Cloud Agent secrets

In Cursor **environment secrets** (not git), set:

- `AIRTABLE_API_KEY`  
- `AIRTABLE_BASE_ID`  

Redeploy or restart agents so Audio PM and Automation Engineer can read/write.

## 7. Zapier MCP (Cloud Agent fallback)

When `api.airtable.com` is blocked on the VM, agents can bootstrap via Zapier **code actions** (`create_record_rest`, `meta_create_fields_batch`) using the connected Airtable account. See [`scripts/sync_schema_fields_zapier.md`](scripts/sync_schema_fields_zapier.md).

## 8. GitHub Actions (Sprint A2)

Add repository secrets for [`.github/workflows/airtable-pr-sync.yml`](../../.github/workflows/airtable-pr-sync.yml):

- `AIRTABLE_API_KEY`
- `AIRTABLE_BASE_ID` (your **DiskLordz OS** base ID)

Merged PR titles containing `WO-YYYY-NNN` patch the matching work order to **Done** and set `github_pr`.

## 9. Zapier MCP (reads/writes in chat)

Your Zapier account already has **Airtable** connected. Agents can use:

- `airtable_get_all_records` — reads  
- `airtable_create_record` / `airtable_update_record` — writes (confirm destructive ops)  

Use the same base selected in Zapier when configuring actions.

## 11. Recommended views (create in UI)

| Table | View |
|-------|------|
| Agent Work Orders | Kanban by `status` |
| Products | Grid filtered `status != Released` |
| Projects | Active only |
| Automations | `status != Live` for backlog |
| Market Signals | `status = New` |

## 10. Done when

- [x] All 11 DiskLordz tables in `ACTIVE_BASE.json` (bootstrap via Zapier Sep 2026)  
- [x] Seed: `DL-FAMILY-DIGITAL-SAMPLER`, `DL-FOUNDATION-001`, `PRJ-2026-001`, `WO-2026-001` / `WO-2026-002`, automations `A1`–`A3`  
- [ ] `verify_connection.py` passes from agent VM (needs PAT + `api.airtable.com` egress)  
- [ ] GitHub repo secrets for Sprint A2 workflow  
- [ ] Cursor secrets: `AIRTABLE_API_KEY`, `AIRTABLE_BASE_ID` (requested via environment setup)  

Next: run `wo_to_github_issue.py` locally when secrets exist; wire scheduled Zap for A1 if desired.
