# Hyperresearch (Hermes `hermes-research`)

## Purpose

**Hyperresearch** is the Disklordz deep-research lane: corpus + repo scan → structured briefs → **WO drafts** for Grok/Airtable. It does **not** merge code or write to external systems without human confirm.

## Build & run

```bash
python3 disklordz/research/scripts/hyperresearch.py --topic "ILLUGEN credits model" --product saas
python3 disklordz/research/scripts/hyperresearch.py --topic "Junova-X chorus BBD" --product junova --write
```

Optional RAG (after `python3 disklordz/rag/scripts/chunk_corpus.py`):

```bash
python3 disklordz/research/scripts/hyperresearch.py --topic "async generation jobs" --rag
```

## Data flow

```text
Topic + product lens
    → manifest sources (research-sources.json + rag manifest)
    → keyword scan + optional query_local.py
    → brief markdown (outbox/HR-*.md)
    → Hermes lead / Grok → Airtable WO
```

## Key modules

| Path | Role |
|------|------|
| `scripts/hyperresearch.py` | CLI agent |
| `research-sources.json` | Default doc corpus |
| `templates/brief-template.md` | Output shape |
| `outbox/` | Generated briefs (commit when WO-worthy) |

## Extension points

- Add paths to `research-sources.json` per product.
- Wire pgvector when WO-SAAS-012b lands.
- External web fetch: agent skill only (untrusted content → no auto-WO without CD).

## Related

- [docs/HERMES_HYPERRESEARCH.md](../../docs/HERMES_HYPERRESEARCH.md)
- [.cursor/skills/hermes-elite-research/SKILL.md](../../.cursor/skills/hermes-elite-research/SKILL.md)
