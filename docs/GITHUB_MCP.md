# GitHub MCP (Cursor)

Lets Cursor agents work with **GitHub** (repos, issues, pull requests, Actions, etc.) via the official hosted server — useful for factory handoffs, PR CI, and pushing Origin mirrors without pasting tokens into chat.

Official server: [github/github-mcp-server](https://github.com/github/github-mcp-server) · [Cursor install guide](https://github.com/github/github-mcp-server/blob/main/docs/installation-guides/install-cursor.md)

## 1. Repo config (in git)

[`.cursor/mcp.json`](../.cursor/mcp.json) registers:

```json
"github": {
  "type": "http",
  "url": "https://api.githubcopilot.com/mcp/",
  "headers": {
    "Authorization": "Bearer ${env:GITHUB_TOKEN}"
  }
}
```

**No secrets in git** — set `GITHUB_TOKEN` in your environment (or use Cursor **Connect** if OAuth is offered for this server in your Cursor version).

## 2. Personal access token

1. GitHub → **Settings → Developer settings → Personal access tokens** (fine-grained recommended).
2. Grant access to **`samuelfreemanjobs-hash/Instruments`** (and any handoff repos).
3. Scopes / permissions as needed: **Contents**, **Pull requests**, **Issues**, **Actions** (read for CI; write only if agents should open PRs).

**macOS / Linux:**

```bash
export GITHUB_TOKEN="github_pat_..."
```

Restart Cursor so `${env:GITHUB_TOKEN}` resolves in `mcp.json`.

## 3. Connect in Cursor

1. **Settings → Tools & MCP** (or **Customize → MCP**).
2. Find **github** from this repo’s `.cursor/mcp.json`.
3. If status is **Needs login**, click **Connect** or ensure `GITHUB_TOKEN` is set.

### Optional: read-only toolsets

Add headers in `.cursor/mcp.json` (local only — prefer env-specific global config if you do not want read-only in git):

```json
"headers": {
  "Authorization": "Bearer ${env:GITHUB_TOKEN}",
  "X-MCP-Readonly": "true",
  "X-MCP-Toolsets": "repos,issues,pull_requests,actions"
}
```

See [remote server configuration](https://github.com/github/github-mcp-server/blob/main/docs/remote-server.md).

## 4. Cloud Agents

1. Add **`GITHUB_TOKEN`** as an [environment secret](https://cursor.com/docs/cloud-agent/setup) (fine-grained PAT, repo-scoped).
2. Allow egress to **`api.githubcopilot.com`** (listed in `.cursor/environment.json` `egressAllowlist`).
3. Enable the **github** MCP server for the Cloud environment if your dashboard exposes an MCP allowlist.

Cloud agents already have **`gh`** CLI for many read operations; GitHub MCP adds structured tools for issues, PRs, and repo content from agent chat.

## 5. Without MCP

Use **`gh`** in the shell (already available on Cloud) or the GitHub web UI. Do not commit PATs or paste them into issues.

Related: [docs/REPO_AUTOMATION.md](REPO_AUTOMATION.md) · [docs/MPC_AGENT_ORIGIN_IMPORT.md](MPC_AGENT_ORIGIN_IMPORT.md)
