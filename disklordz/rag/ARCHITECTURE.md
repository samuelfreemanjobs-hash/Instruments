# Disklordz RAG (prompt knowledge)

## Purpose

Index **lane docs, prompt tips, and product specs** for retrieval-augmented prompt assistance and automation (WO-SAAS-012). Not used in v0 generate path.

## Build & run

```bash
cd disklordz/rag
python3 scripts/chunk_corpus.py
python3 scripts/query_local.py "cyber funk 96 bpm"
```

Production embed + pgvector: WO-SAAS-012 (see [docs/RAG_AND_INTELLIGENT_AUTOMATION.md](../../docs/RAG_AND_INTELLIGENT_AUTOMATION.md)).

## Data flow

```text
corpus/manifest.json → chunk_corpus.py → data/chunks.jsonl
    → (future) embed_and_upsert.py → Supabase pgvector
    → (future) /api/rag/suggest → GenerationSpec hints
```

## Key modules

| Path | Role |
|------|------|
| `corpus/manifest.json` | Source paths relative to repo root |
| `scripts/chunk_corpus.py` | Markdown → overlapping chunks |
| `scripts/query_local.py` | Keyword rank demo (no API key) |
| `data/.gitkeep` | Generated chunks (gitignored jsonl in .gitignore) |

## Threading / realtime

Indexing is **offline/CI**; query is **request-scoped** (<500ms target with pgvector).

## Extension points

- Add `embed_and_upsert.py` + Supabase migration  
- Wire `KitGenerator` “Random” / “Improve prompt” to `/api/rag/suggest`  
- GitHub Action: re-index on `docs/**` change  

## Related docs

- [DISKLORDZ_ILLUGEN_RESEARCH.md](../../docs/DISKLORDZ_ILLUGEN_RESEARCH.md)
- [RAG_AND_INTELLIGENT_AUTOMATION.md](../../docs/RAG_AND_INTELLIGENT_AUTOMATION.md)
