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

**No secrets in git** — set `GITHUB_TOKEN` in your environment.

> **OAuth vs PAT:** GitHub’s **hosted** server (`https://api.githubcopilot.com/mcp/`) currently expects a **Personal Access Token** in the `Authorization` header ([official Cursor guide](https://github.com/github/github-mcp-server/blob/main/docs/installation-guides/install-cursor.md)). **OAuth browser login** applies to the **local Docker** server (`ghcr.io/github/github-mcp-server`) — see [oauth-login.md](https://github.com/github/github-mcp-server/blob/main/docs/oauth-login.md). Cursor’s **Connect** button may still store a token for you after sign-in; treat it as PAT-backed either way.

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

**One-click (desktop):** [Install GitHub MCP deeplink](https://cursor.com/en/install-mcp?name=github&config=eyJ1cmwiOiJodHRwczovL2FwaS5naXRodWJjb3BpbG90LmNvbS9tY3AvIiwiaGVhZGVycyI6eyJBdXRob3JpemF0aW9uIjoiQmVhcmVyIFlPVVJfR0lUSFVCX1BBVCJ9fQ%3D%3D) — then paste your PAT when prompted.

**Manual:**

1. **Settings → Tools & MCP** (or **Customize → MCP**).
2. Find **github** from this repo’s `.cursor/mcp.json`.
3. Click **Connect** / edit headers — use a PAT (hosted server) or switch to Docker config for OAuth (local only).

**Verify (green dot):** Settings → MCP shows **github** connected; in chat, ask: “List open PRs on samuelfreemanjobs-hash/Instruments”.

**Verify (script):**

```bash
bash scripts/verify_github_mcp.sh
```

On Cloud, `gh auth status` confirms GitHub git/API integration; MCP additionally needs `GITHUB_TOKEN` + `api.githubcopilot.com` egress.

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
