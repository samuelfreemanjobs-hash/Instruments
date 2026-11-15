# Supabase + Cursor for Black Friday Madness

## Cannot automate Cursor App secrets

Published app secrets are set only in **Cursor → app → Secrets** (secure form). No Zapier action, repo script, or MCP tool can write those for you. A script can fill **local** `.env` files; you copy the same three names into the publish dialog.

**Zapier:** there is no supported Zap for “create Supabase project + push Cursor app secrets.” Use the CLI script below instead.

## One-shot local provision (recommended)

```bash
cd tools/deal-hunter
export SUPABASE_ACCESS_TOKEN=sbp_...   # dashboard → Account → Access tokens
export SUPABASE_ORG_ID=...             # only if creating a new project
# or: export SUPABASE_PROJECT_REF=existing-ref

bash scripts/provision-supabase.sh
npm run dev
```

This links the project, runs `supabase db push`, and updates `tools/deal-hunter/.env` + `web/.env.local`.

## Supabase MCP (agents run SQL / migrations)

The monorepo already registers Supabase MCP in [`.cursor/mcp.json`](../../../.cursor/mcp.json).

1. Set `SUPABASE_PROJECT_REF` and optionally `SUPABASE_ACCESS_TOKEN`.
2. **Cursor Settings → MCP → supabase → Connect** (OAuth).
3. Ask the agent to apply migrations or inspect tables for `deal-hunter` (migrations under `tools/deal-hunter/supabase/migrations`).

MCP does **not** inject keys into the published app; use `provision-supabase.sh` or the dashboard for API keys.

## Manual fallback

```bash
npm run setup:env
npm run supabase:migrate   # prints combined SQL if CLI not linked
```

See [supabase/README.md](../supabase/README.md) and [CURSOR_PUBLISH.md](./CURSOR_PUBLISH.md).
