# GitHub MCP (Cursor)

Lets Cursor agents use GitHub’s official hosted MCP server (issues, PRs, repos, Actions, and related API tools) against **your** GitHub account or org.

## 1. Repo config (in git)

[`.cursor/mcp.json`](../.cursor/mcp.json) points at the hosted server:

`https://api.githubcopilot.com/mcp/`

**No secrets are stored in git.** The `Authorization` header uses `${env:GITHUB_TOKEN}`.

Requires **Cursor v0.48+** (Streamable HTTP / remote MCP).

## 2. Authentication

### Desktop: OAuth (preferred when available)

1. **Cursor Settings → MCP** (or **Customize → MCP**).
2. Find the **github** server from this repo’s `.cursor/mcp.json`.
3. If it shows **Needs login**, click **Connect** and complete GitHub sign-in in the browser.

OAuth is the usual path for local Cursor when you do not want a long-lived PAT in your shell.

### Headless / Cloud Agents: `GITHUB_TOKEN`

Cloud runs and automation need a [GitHub personal access token](https://github.com/settings/personal-access-tokens/new) (classic or fine-grained) with scopes you are comfortable granting (often `repo`, `read:org`, `read:user`, and workflow/read as needed).

Add **`GITHUB_TOKEN`** as a **Cloud Agent environment secret** (not in the repo). The MCP config resolves it at runtime.

Fine-grained tokens: restrict to the `samuelfreemanjobs-hash/Instruments` repo (and any other repos this environment checks out).

## 3. Network (Cloud Agent egress)

If your environment uses a **restricted egress allowlist**, allow:

| Host | Purpose |
|------|---------|
| `api.githubcopilot.com` | Hosted GitHub MCP |
| `github.com` | OAuth and web |
| `api.github.com` | GitHub REST API (often already allowed) |

Approve these in the Cloud Agent environment **Network** settings when prompted.

## 4. Verify

1. Restart Cursor or start a **new** Cloud Agent after secrets/egress are set.
2. **Settings → MCP** → **github** should show connected (green) with a non-zero tool count.
3. In chat, ask: “List open pull requests on this repository.”

## 5. Without MCP

Use the preconfigured **`gh`** CLI in Cloud Agents (`gh pr list`, `gh issue view`, etc.) and [GitHub Actions](../../.github/workflows/) in this repo.

Related: [SUPABASE_MCP.md](../disklordz/website/docs/SUPABASE_MCP.md) · [STRIPE_MCP.md](../disklordz/website/docs/STRIPE_MCP.md) · [GitHub MCP install guide (official)](https://github.com/github/github-mcp-server/blob/main/docs/installation-guides/install-cursor.md)
