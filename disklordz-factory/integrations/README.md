# Integrations (planned)

| Integration | Purpose |
|-------------|---------|
| n8n | Night shift cron, webhooks to Factory API |
| YouTube Data API | Upload programming, metadata |
| Airtable / Postgres | Catalog sync |
| Slack | `#disklordz-ci` alerts (see repo `scripts/setup-disklordz-integrations.sh`) |
| DistroKid / distributor | Streaming |
| Store (Stripe/Gumroad) | Supply commerce |
| **Claude projects MCP** | [`claude_projects_mcp/`](claude_projects_mcp/) — read Claude Code workspace repos from Cursor |

See [docs/CLAUDE_MCP.md](../../docs/CLAUDE_MCP.md) for setup.

Implement adapters here; keep secrets out of git.
