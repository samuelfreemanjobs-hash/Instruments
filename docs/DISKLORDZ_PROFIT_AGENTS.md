# 15 AI agents for Disklordz profit (automatable)

These are **not** bundled as 15 folders inside Instruments today. They are **roles** you can automate with GitHub-hosted tools + this repo’s scripts, MCP servers, and workflows. Machine-readable list: [`disklordz/integrations/agents/profit-agents.json`](../disklordz/integrations/agents/profit-agents.json).

| # | Agent role | Primary GitHub / tool | Profit lever | Automate with |
|---|------------|----------------------|--------------|---------------|
| 1 | **Conversion QA agent** | [microsoft/playwright-mcp](https://github.com/microsoft/playwright-mcp) | Fewer broken checkouts → more Pro | Cursor MCP + scheduled workflow hitting `/api/generate` |
| 2 | **Billing ops agent** | [stripe/agent-toolkit](https://github.com/stripe/agent-toolkit) | Recover failed payments, fix prices | `.cursor/mcp.json` Stripe MCP + human-approved writes |
| 3 | **Ship velocity agent** | [github/github-mcp-server](https://github.com/github/github-mcp-server) | Faster WO → issue → PR → deploy | Cloud Agent + Airtable handoff workflows |
| 4 | **Async generation agent** | [inngest/inngest](https://github.com/inngest/inngest) | Long jobs finish → higher credit burn / satisfaction | `/api/generate/async` + `activate-integrations.sh` |
| 5 | **Prompt coach agent** | [vercel/ai](https://github.com/vercel/ai) + pgvector | Better free-tier kits → sign-up | `/api/rag/suggest` + `embed_and_upsert.py` |
| 6 | **Product factory agent** | [crewAIInc/crewAI](https://github.com/crewAIInc/crewAI) | Storefront packs SKUs → higher AOV | `integrations/agents/crew_product_factory.py` + `/api/factory/batch` |
| 7 | **Spec validator agent** | [pydantic/pydantic-ai](https://github.com/pydantic/pydantic-ai) | Fewer bad gens → fewer refunds | `integrations/agents/pydantic_spec_agent.py` |
| 8 | **Lane workflow agent** | [langchain-ai/langgraph](https://github.com/langchain-ai/langgraph) | prompt → spec → variations pipeline | `integrations/agents/langgraph_spec_flow.py` |
| 9 | **Ops / schema agent** | [supabase/mcp](https://github.com/supabase/mcp) | Safe migrations, no prod outages | `apply-supabase-migrations.sh` + read-only MCP |
| 10 | **Marketing glue agent** | [n8n-io/n8n](https://github.com/n8n-io/n8n) | Leads → email → return visits | `docker-compose.optional.yml --profile n8n` |
| 11 | **Support macro agent** | RAG corpus + Cloud Agent | Less manual support | `.github/skills/disklordz-rag` + `/api/rag/suggest` |
| 12 | **Desktop ops agent** | [bytebot-ai/bytebot](https://github.com/bytebot-ai/bytebot) | Vendor portals, bulk downloads | [docs/BYTEBOT_SETUP.md](BYTEBOT_SETUP.md) (local Docker) |
| 13 | **Factory batch GPU agent** | [triggerdotdev/trigger.dev](https://github.com/triggerdotdev/trigger.dev) | Large pack runs without timeout | Documented in `integrations/engines/README.md` |
| 14 | **Engine swap agent** | [facebookresearch/audiocraft](https://github.com/facebookresearch/audiocraft) / [Stability-AI/stable-audio-tools](https://github.com/Stability-AI/stable-audio-tools) | Premium sound → Pro retention | `DISKLORDZ_ENGINE=remote` + engine stubs |
| 15 | **Integration health agent** | This repo | Catch misconfig before revenue leak | `GET /api/integrations/status` + `verify-integrations.sh` |

## One command (local or CI)

```bash
# All automated steps that your env secrets allow:
bash disklordz/integrations/scripts/activate-integrations.sh --all

# GitHub Actions:
# Actions → Activate Disklordz integrations → Run workflow
```

## Secrets to add (GitHub Actions)

| Secret | Enables |
|--------|---------|
| `OPENAI_API_KEY` | RAG embed + AI prompt polish |
| `INNGEST_EVENT_KEY` / `INNGEST_SIGNING_KEY` | Async generate |
| Existing go-live secrets | Migrations, Vercel, smoke |

See [DISKLORDZ_GO_LIVE_SECRETS.md](DISKLORDZ_GO_LIVE_SECRETS.md).

## Related

- [DISKLORDZ_OPEN_SOURCE_REFERENCES.md](DISKLORDZ_OPEN_SOURCE_REFERENCES.md)
- [disklordz/integrations/ARCHITECTURE.md](../disklordz/integrations/ARCHITECTURE.md)
