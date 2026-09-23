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

## 4. Cloud Agents — approve setup

1. When the agent requests setup, **Approve**:
   - Secret **`GITHUB_TOKEN`** (fine-grained PAT for `samuelfreemanjobs-hash/Instruments` — use this exact name, all caps).
   - Egress **`api.githubcopilot.com`** (MCP host). **`api.github.com`** / **`github.com`** are usually already allowed for REST/`gh`.
2. Open the [Instruments environment](https://cursor.com/dashboard/cloud-agents/environments/e/85d21659-b124-11f1-a3d8-362438fd9788) and click **Save**.
3. Start a **new** Cloud Agent run (secrets and egress do not apply to an already-running pod).
4. Verify: `bash scripts/verify_github_mcp.sh` — expect MCP section to show HTTP response, not `SKIP` or egress `FAIL`.

Cloud agents already have **`gh`** CLI for git operations; GitHub MCP adds structured issue/PR/repo tools in chat when MCP is enabled for the run.

## 5. Without MCP

Use **`gh`** in the shell (already available on Cloud) or the GitHub web UI. Do not commit PATs or paste them into issues.

Related: [docs/REPO_AUTOMATION.md](REPO_AUTOMATION.md) · [docs/MPC_AGENT_ORIGIN_IMPORT.md](MPC_AGENT_ORIGIN_IMPORT.md)
