# Supabase MCP (Cursor / VS Code)

Connect the agent to your Supabase projects via the [hosted MCP server](https://supabase.com/docs/guides/getting-started/mcp).

## 1. Config (this repo)

Project config lives at [`.cursor/mcp.json`](../.cursor/mcp.json):

```json
{
  "mcpServers": {
    "supabase": {
      "url": "https://mcp.supabase.com/mcp"
    }
  }
}
```

Reload the window after editing.

## 2. Authenticate

1. **Cursor:** **Settings → Cursor Settings → Tools & MCP** → find **supabase** → **Connect** (OAuth in the browser).
2. Or run an agent turn that calls the Supabase MCP; complete the browser login when prompted.

No personal access token is required for the hosted URL (OAuth 2.1).

## 3. Scope to one project (recommended)

Append query parameters to the URL in `.cursor/mcp.json`:

| Goal | Example URL |
|------|-------------|
| Single project | `https://mcp.supabase.com/mcp?project_ref=YOUR_PROJECT_REF` |
| Read-only SQL | `https://mcp.supabase.com/mcp?project_ref=YOUR_PROJECT_REF&read_only=true` |
| Limit tools | `https://mcp.supabase.com/mcp?features=database,docs` |

Get `project_ref` from the Supabase dashboard URL: `https://supabase.com/dashboard/project/<project_ref>`.

Or use the dashboard **Connect → MCP** tab to copy a pre-built URL.

## 4. Alternative: PAT + npx (CI / no OAuth)

Use a [personal access token](https://supabase.com/dashboard/account/tokens) **only** in env vars, never in git:

```json
{
  "mcpServers": {
    "supabase": {
      "command": "npx",
      "args": [
        "-y",
        "@supabase/mcp-server-supabase@latest",
        "--access-token",
        "${env:SUPABASE_ACCESS_TOKEN}"
      ]
    }
  }
}
```

On Windows, some clients need `"command": "cmd"` and `"args": ["/c", "npx", ...]`.

## 5. Local Supabase CLI

When `supabase start` is running:

```json
"url": "http://localhost:54321/mcp"
```

## 6. Agent usage

After connect, the agent can use tools such as `search_docs`, `execute_sql`, and `get_advisors`. Follow the Supabase skill: `.cursor/plugins` **supabase** skill (or install the Supabase plugin from the Cursor marketplace).

**Schema workflow:** iterate with `execute_sql`; commit with `supabase migration new` + `supabase db pull --local` — do not spam `apply_migration` while experimenting.

## 7. Troubleshoot

```bash
curl -so /dev/null -w "%{http_code}\n" https://mcp.supabase.com/mcp
```

`401` without a token means the server is up. Timeouts → network/firewall.

If tools do not appear: reconnect MCP, restart Cursor, confirm `.cursor/mcp.json` is at the **workspace root** you opened.
