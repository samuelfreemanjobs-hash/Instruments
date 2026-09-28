# getzep/graphiti (optional)

Use when kit history + lane docs need **temporal** edges (e.g. “this WO superseded that prompt rule”).

1. Run Graphiti as a separate service.
2. Ingest `saved_kits` metadata and `prompt_knowledge_chunks` as episodes.
3. Expose a read tool to `/api/rag/suggest` before keyword/pgvector merge.

Not bundled in the website build — see https://github.com/getzep/graphiti
