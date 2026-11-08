# Cloudflare Agents RAG worker stub (cloudflare/agents)

Edge RAG is optional; primary path is Supabase pgvector + `disklordz/website/src/lib/rag/`.

Deploy sketch:

1. Worker + D1/KV for corpus cache
2. Proxy to `/api/rag/suggest` or embed worker-side

Local verify:

```bash
test -f disklordz/website/src/lib/rag/hybrid-retrieve.ts && echo OK
```

No Cloudflare credentials required for monorepo wiring check.
