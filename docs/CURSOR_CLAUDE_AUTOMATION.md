# Claude → Cursor automation

Hand off a **GitHub issue** spec to a **Cursor Cloud Agent** using a repo label and optional API dispatch.

## Quick start

1. Create an issue using [TEMPLATE-claude-to-cursor-handoff.md](templates/TEMPLATE-claude-to-cursor-handoff.md).
2. Add the label **`cursor-agent`** (create it once in GitHub → Issues → Labels if missing).
3. Workflow [`.github/workflows/cursor-agent-from-issue.yml`](../.github/workflows/cursor-agent-from-issue.yml) runs:
   - Builds a prompt from the issue title + body + template footer.
   - If secret **`CURSOR_API_KEY`** is set, POSTs to [Cloud Agents API](https://cursor.com/docs/cloud-agent/api/endpoints) (`POST https://api.cursor.com/v1/agents`).
   - Comments on the issue with the agent run URL (or instructions when the key is absent).

## Secrets

| Secret | Required | Purpose |
|--------|----------|---------|
| `CURSOR_API_KEY` | Optional | Launch Cloud Agent via API |
| `GITHUB_TOKEN` | Default | Issue comments (provided by Actions) |

Generate the API key in **Cursor Settings → Integrations / API keys**. Never commit keys.

## Native Cursor Automations (alternative)

You can also configure **Issue label changed** triggers at [cursor.com/automations](https://cursor.com/automations) without GitHub Actions. Keep this workflow for teams that want the prompt template and audit trail in-repo.

## Related

- [CURSOR_AGENT_PLAYBOOK.md](CURSOR_AGENT_PLAYBOOK.md)
- [disklordz/antigravity/ARCHITECTURE.md](../disklordz/antigravity/ARCHITECTURE.md)
