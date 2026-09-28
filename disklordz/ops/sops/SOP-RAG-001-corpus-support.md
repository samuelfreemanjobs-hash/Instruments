# SOP-RAG-001 — RAG corpus refresh and support queries

| Field | Value |
|-------|--------|
| **Owner seat** | hermes-support |
| **Consumer seats** | hermes-support, hermes-research |
| **Cadence** | weekly_or_on_doc_change |

## Purpose

Support agents and humans query up-to-date Disklordz knowledge locally.

## Procedure

1. After doc or product changes affecting support:
   ```bash
   python3 disklordz/rag/scripts/chunk_corpus.py
   python3 disklordz/rag/scripts/query_local.py "smoke test query"
   ```
2. Status:
   ```bash
   python3 disklordz/hermes/scripts/hermes_tool.py support rag-status
   ```
3. Include new SOPs under `disklordz/ops/sops/` in corpus when customer-facing.

## Verification

- `chunks.jsonl` line count increases or stable when no doc changes.
- Query returns relevant chunks for product keywords.

## Related

- [disklordz/rag/ARCHITECTURE.md](../../rag/ARCHITECTURE.md)
