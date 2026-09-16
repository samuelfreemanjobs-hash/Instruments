# Sync remaining fields (Zapier)

Core tables were created with **primary fields only** via Zapier `create_table`.

## Option A — PAT (recommended)

After `AIRTABLE_API_KEY` is in Cursor secrets and `api.airtable.com` egress is allowed:

```bash
export AIRTABLE_API_KEY=pat...
export AIRTABLE_BASE_ID=appwaEPUOCXBoq8jB
python create_base_from_schema.py --schema ../base-schema.json
# Skips existing base; use Meta API to add missing fields per table (extend script) OR
# run full bootstrap on a fresh base created in UI.
```

## Option B — Zapier code actions (used for Cloud Agent bootstrap)

1. Generate ops: `python apply_schema_fields.py` → `.payloads/field_ops.json`
2. Batch apply via Zapier `code_action_airtablecliapi__meta_create_fields_batch` (`base_id` + `operations` JSON array).
3. Seed rows via `code_action_airtablecliapi__create_record_rest` and patches via `code_action_airtablecliapi__update_record_by_formula`.

Note: native `airtable_make_api_mutating_request` often fails on Meta API bodies; prefer code actions.

Table IDs: see `ACTIVE_BASE.json`.

## Option C — Airtable UI

Use `FIELD_GUIDE.md` to add columns manually (fast for MVP).
