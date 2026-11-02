# SOP-007 — RAG corpus update (Prompt Coach)

## Purpose

Keep lane vocabulary and docs retrievable for `/api/rag/suggest` and prompt coaching.

## Procedure

1. **Edit corpus** — Markdown under paths referenced in `disklordz/rag/` (see [disklordz/rag/ARCHITECTURE.md](../../../disklordz/rag/ARCHITECTURE.md)).
2. **Chunk** — `python3 disklordz/rag/scripts/chunk_corpus.py`
3. **Query locally** — `python3 disklordz/rag/scripts/query_local.py "your query"`
4. **Embed (prod)** — `embed_and_upsert.py` when `OPENAI_API_KEY` + Supabase pgvector migration applied (WO-SAAS-012).
5. **CI** — Push to `docs/**` triggers `.github/workflows/rag-reindex.yml` when configured.

## Verify

```bash
curl -s -X POST "$DISKLORDZ_URL/api/rag/suggest" \
  -H "Content-Type: application/json" \
  -d '{"mode":"random","presetId":"boulevard-86"}' | python3 -m json.tool
```

Expect `prompt` in response.

## Agent

**prompt-coach** — also runs chunk step inside `fleet_execute.py`.
