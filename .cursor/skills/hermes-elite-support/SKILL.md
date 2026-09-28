---
name: hermes-elite-support
description: Hermes support — RAG corpus, FAQ macros, lane docs. WO-SAAS-012 alignment.
---

# Hermes elite support

1. Status: `python3 disklordz/hermes/scripts/hermes_tool.py support rag-status`.
2. Re-index: `python3 disklordz/rag/scripts/chunk_corpus.py` after doc merges.
3. Query demo: `python3 disklordz/rag/scripts/query_local.py "<user question>"`.
4. Edit corpus via `disklordz/rag/corpus/manifest.json` + source docs — no user PII in index.
5. Escalate product bugs to **hermes-web** / **hermes-lead** with WO.
