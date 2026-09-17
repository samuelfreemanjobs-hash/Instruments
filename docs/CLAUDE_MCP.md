# Claude ↔ Cursor MCP bridge

Access **Claude Code project workspaces** (including repos that only exist under Claude’s on-disk project area) from **Cursor** via MCP.

## What was added

| File | Purpose |
|------|---------|
| [`.cursor/mcp.json`](../.cursor/mcp.json) | Cursor MCP: `claude-code` + `claude-projects` |
| [`.mcp.json`](../.mcp.json) | Claude Code project MCP (same `claude-projects` server + optional GitHub HTTP) |
| [`disklordz-factory/integrations/claude_projects_mcp/`](../disklordz-factory/integrations/claude_projects_mcp/) | Stdio MCP server that lists/reads `~/.claude/projects/*` |

## One-time setup (your Mac)

### 1. Python deps (claude-projects server)

```bash
pip3 install -r disklordz-factory/integrations/claude_projects_mcp/requirements.txt
```

Optional: point at a custom Claude projects root:

```bash
export CLAUDE_PROJECTS_ROOT="$HOME/.claude/projects"
```

### 2. Cursor

1. Open this repo in Cursor.
2. **Settings → Tools & MCP** — enable **`claude-projects`** (should show Connected after reload).
3. Optional **`claude-code`**: install [Claude Code](https://code.claude.com/docs/en/setup) and ensure `claude` is on your `PATH`, or edit `.cursor/mcp.json`:

```json
"claude-code": {
  "command": "/Users/YOU/.local/bin/claude",
  "args": ["mcp", "serve"]
}
```

Reload the window (**Developer: Reload Window**) after editing MCP config.

### 3. Claude Code (same repo)

In this repository, Claude Code loads [`.mcp.json`](../.mcp.json). Approve **`claude-projects`** when prompted (`claude` → trust workspace → `/mcp`).

For GitHub repos (e.g. already pushed `EMU-SP1200`), add a fine-grained PAT and export:

```bash
export GITHUB_PAT=github_pat_...
```

Then use the **`github`** entry in `.mcp.json` (or `claude mcp add-json` per [GitHub MCP docs](https://docs.github.com/en/copilot/how-tos/provide-context/use-mcp-in-your-ide/set-up-the-github-mcp-server)).

## Using `claude-projects` in Cursor

Ask the agent to:

1. `search_claude_projects` with query `sp-1200` or `sp1200`
2. `list_claude_project_files` for the matching `project_id`
3. `read_claude_project_file` for sources you want in this monorepo

Example user prompt:

> Search Claude projects for sp-1200, show files, and copy the repo into `./sp-1200`.

## `claude-code` MCP (optional)

`claude mcp serve` exposes Claude Code’s own tools (Read, Edit, LS, …) to Cursor. Requires a recent Claude Code build; if Cursor shows **connected** but **0 tools**, update Claude Code ([known schema issue in older builds](https://github.com/anthropics/claude-code/issues/10031)).

## Security

- `claude-projects` is **read-only** and limited to `CLAUDE_PROJECTS_ROOT`.
- Do **not** commit `GITHUB_PAT` or other secrets; use environment variables only.
