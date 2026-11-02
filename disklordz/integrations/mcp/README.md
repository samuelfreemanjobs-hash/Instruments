# MCP wiring

| Reference | This repo |
|-----------|-----------|
| supabase/mcp | `.cursor/mcp.json` |
| stripe/agent-toolkit | `.cursor/mcp.json` (HTTP MCP) |
| github/github-mcp-server | `.cursor/mcp.json.example` |
| modelcontextprotocol/servers | Example configs in `.cursor/mcp.json.example` |
| modelcontextprotocol/typescript-sdk | [`disklordz-mcp-server/`](disklordz-mcp-server/) |
| punkpeye/awesome-mcp-servers | Use catalog to add Slack/Airtable/Vercel MCPs |

Build custom server:

```bash
cd disklordz/integrations/mcp/disklordz-mcp-server && npm ci && npm run build
```
