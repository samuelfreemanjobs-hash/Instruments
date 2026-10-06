---
name: disklordz-rag
description: Maintain Disklordz prompt knowledge (corpus, pgvector, /api/rag/suggest). Use for WO-SAAS-012 and lane docs.
---

# Disklordz RAG skill

## Pipeline

```bash
python3 disklordz/rag/scripts/chunk_corpus.py
# optional: OPENAI_API_KEY + Supabase service role
python3 disklordz/rag/scripts/embed_and_upsert.py
```

## Hybrid retrieval

Website uses keyword v1 plus optional pgvector (`match_prompt_knowledge` RPC). Optional AI polish via Vercel AI SDK when `OPENAI_API_KEY` is set.

## Corpus

Update `disklordz/rag/corpus/manifest.json` when adding lane docs under `docs/` or `disklordz/factory/prompts/`.
