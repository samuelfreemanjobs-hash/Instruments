# Supabase MCP (Cursor)

Lets Cursor agents run SQL, inspect schema, and use Supabase docs tools against **your** project (same DB as the Drum SaaS app).

## 1. Repo config (already in git)

[`.cursor/mcp.json`](../../../.cursor/mcp.json) points at the hosted server:

`https://mcp.supabase.com/mcp`

It scopes to one project and enables database + docs + development tools. **No secrets are stored in git** — values come from environment variables.

## 2. Set environment variables (your machine)

| Variable | Where to get it |
|----------|------------------|
| `SUPABASE_PROJECT_REF` | Supabase dashboard URL: `https://<THIS_PART>.supabase.co` |
| `SUPABASE_ACCESS_TOKEN` | Optional but recommended for headless/CI-style auth: [Account → Access tokens](https://supabase.com/dashboard/account/tokens) |

**macOS / Linux (shell profile):**

```bash
export SUPABASE_PROJECT_REF="your-project-ref"
export SUPABASE_ACCESS_TOKEN="sbp_..."
```

Restart Cursor after setting env vars so `${env:...}` resolves in `mcp.json`.

## 3. Connect in Cursor (OAuth)

1. **Cursor Settings → MCP** (or **Customize → MCP**).
2. Find the **supabase** server from this repo’s `.cursor/mcp.json`.
3. If it shows **Needs login**, click **Connect** and sign in to Supabase in the browser; pick the org that owns the project.

OAuth is the usual path; the access token in `headers` is an alternative when OAuth is awkward (e.g. some automation).

## 4. Safer read-only mode (optional)

For exploration only, change the URL in `.cursor/mcp.json` to append `&read_only=true`. Remove it when applying migrations or DDL.

## 5. Cloud Agents

Cloud runs need the same MCP entry and network access to `mcp.supabase.com`. In your [Cloud Agent environment](https://cursor.com/docs/cloud-agent/setup), allow that host if you use a custom egress policy, and set `SUPABASE_PROJECT_REF` / `SUPABASE_ACCESS_TOKEN` as **environment secrets** (not in the repo).

## 6. Without MCP

- **GitHub Actions:** [Disklordz Supabase migrations](../../.github/workflows/disklordz-supabase-migrate.yml) + repo secrets.
- **CLI:** `bash scripts/apply-supabase-migrations.sh` in this directory.

Related: [DEPLOY.md](../DEPLOY.md) · [docs/DISKLORDZ_GO_LIVE.md](../../../docs/DISKLORDZ_GO_LIVE.md)
