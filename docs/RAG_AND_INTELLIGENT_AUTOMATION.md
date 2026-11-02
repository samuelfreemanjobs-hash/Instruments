# RAG and intelligent automation (Disklordz)

**Goal:** Use **retrieval-augmented generation** over your own corpus (artist lanes, prompt tips, WO specs, factory metadata) to power:

- Prompt **suggestions** and **Random** discovery (ILLUGEN-style)  
- **Structured generation spec** drafting (genre → BPM → key)  
- **Agent automation** (Airtable WOs, support macros) without hallucinating lane rules  

Implementation home: [`disklordz/rag/`](../disklordz/rag/). **WO-SAAS-012** ships production RAG; v0 repo includes corpus manifest + build scripts.

---

## Architecture

```text
Sources (git)
  docs/lanes/*.md, prompts/artists/*.md, DISKLORDZ_*.md, saved_kits metadata (later)
        │
        ▼
  chunk_markdown.py  →  chunks.jsonl
        │
        ▼
  embed (OpenAI / local / Supabase Edge)  →  vectors
        │
        ▼
  Vector store
  ┌─────────────────┬──────────────────────┐
  │ Supabase        │ pgvector (preferred  │
  │ pgvector        │ — same project as    │
  │                 │ auth)                │
  └─────────────────┴──────────────────────┘
        │
        ▼
  Query API (/api/rag/suggest or agent tool)
        │
        ├── User: "dark cyber funk 96 bpm" → top-k chunks → LLM → GenerationSpec JSON
        └── Automation: WO title → retrieve acceptance criteria templates
```

---

## Vector database choice

| Option | When |
|--------|------|
| **Supabase pgvector** | Default for Disklordz (auth + DB already) |
| **Pinecone / Weaviate** | Separate search team or huge corpus |
| **Local FAISS** | Colab / offline experiments only |

Schema sketch (apply in WO-SAAS-012 migration):

```sql
-- prompt_knowledge_chunks: id, source_path, lane_id, content, embedding vector(1536)
-- match_prompt_knowledge(query_embedding, match_count) → RPC
```

---

## Corpus (versioned in git)

| Source | Role |
|--------|------|
| `docs/DISKLORDZ_ILLUGEN_RESEARCH.md` | Product + WO backlog |
| `docs/lanes/DL*.md` | Artist lane rules |
| `disklordz/factory/prompts/artists/` | When on factory branch |
| `disklordz/rag/corpus/manifest.json` | Canonical list for indexing |

Re-index on merge to `main` (GitHub Action optional, WO-SAAS-012).

---

## Intelligent automation (beyond chat)

| Flow | RAG role |
|------|----------|
| Airtable WO → Antigravity handoff | Retrieve HISE lane doc chunks for `acceptance_criteria` |
| SaaS “Random prompt” | Sample from high-quality prompt exemplars in index |
| Product factory batch | Retrieve folder template + metadata rules for SKU |
| Cursor Cloud Agent | Rules + RAG tool = consistent lane vocabulary |

Existing automations (no RAG required): [disklordz/automation/README.md](../disklordz/automation/README.md).

---

## Security

- Do not embed **user PII** or **private kits** until consent + retention policy  
- RAG API: authenticated or rate-limited; no raw chunk leakage of unreleased product briefs  
- Separate **dev** and **prod** indexes  

---

## Local / CI commands

```bash
cd disklordz/rag
python3 scripts/chunk_corpus.py          # writes data/chunks.jsonl
# WO-SAAS-012: embed_and_upsert.py with OPENAI_API_KEY or Supabase service role
python3 scripts/query_local.py "memphis cowbell 92 bpm"  # keyword demo without API key
```

---

## Cursor as development assistant

1. Add rule: “For prompt UX, read `DISKLORDZ_ILLUGEN_RESEARCH.md` and `disklordz/rag/ARCHITECTURE.md`.”  
2. Task agent: “Implement WO-SAAS-012 step 1: chunk + pgvector migration only.”  
3. Use [CURSOR_AGENT_PLAYBOOK.md](CURSOR_AGENT_PLAYBOOK.md) success criteria.  

---

## Deploy online

RAG **query** runs in **Vercel serverless** or **Supabase Edge**; **index build** runs in CI or scheduled Action with service role key in GitHub Secrets.

Website deploy: [disklordz/website/DEPLOY.md](../disklordz/website/DEPLOY.md).
