# GitHub Actions secret: `AIRTABLE_API_KEY`

The Cloud Agent **cannot** create this secret (`gh secret set` returns 403). You add it once in GitHub.

## Steps

1. Open **[New repository secret](https://github.com/samuelfreemanjobs-hash/Instruments/settings/secrets/actions/new)**  
2. **Name:** `AIRTABLE_API_KEY`  
3. **Secret:** your Airtable personal access token (`pat…`) — same token as Cloud Agent / Cursor MCP  
4. **Add secret**

## Verify

[Run workflow: Sync Disklordz PM to Airtable](https://github.com/samuelfreemanjobs-hash/Instruments/actions/workflows/sync-disklordz-pm.yml) → **Run workflow** → branch `cursor/disklordz-blueprint-tools-2702` (or `main` after merge).

Success log: `Created factory project` / `Updated factory project` and `Updated PM Agent`.

## Token scopes

[Create token](https://airtable.com/create/tokens) with read/write on base `appUuhVQHAOv31wJ1` (Projects table).
