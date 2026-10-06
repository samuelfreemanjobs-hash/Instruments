---
name: disklordz-integrations
description: Wire external GitHub reference projects into Disklordz via disklordz/integrations without vendoring full upstream repos.
---

# Disklordz integrations skill

## Hub

- Manifest: `disklordz/integrations/manifest.json` (35 repos)
- Setup: `./scripts/setup-open-source-integrations.sh`
- Health: `GET /api/integrations/status`

## MCP

- Production: `.cursor/mcp.json` (Supabase + Stripe HTTP MCP)
- Template: `.cursor/mcp.json.example` (GitHub MCP, optional servers)
- Custom tools: `disklordz/integrations/mcp/disklordz-mcp-server/`

## Optional services

- n8n: `docker compose -f disklordz/integrations/docker-compose.optional.yml --profile n8n up -d`
- GPU engines: `disklordz/integrations/engines/README.md`
