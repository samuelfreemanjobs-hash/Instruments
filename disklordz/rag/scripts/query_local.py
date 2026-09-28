#!/usr/bin/env python3
"""Keyword retrieval demo over data/chunks.jsonl (no embeddings)."""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path

RAG_ROOT = Path(__file__).resolve().parents[1]
CHUNKS = RAG_ROOT / "data" / "chunks.jsonl"


def score(query: str, text: str) -> int:
    q = re.findall(r"[a-z0-9]+", query.lower())
    if not q:
        return 0
    t = text.lower()
    return sum(t.count(term) for term in q)


def main() -> None:
    if len(sys.argv) < 2:
        raise SystemExit("usage: query_local.py <query>")
    if not CHUNKS.is_file():
        raise SystemExit(f"missing {CHUNKS}; run chunk_corpus.py first")
    query = " ".join(sys.argv[1:])
    rows: list[tuple[int, dict]] = []
    with CHUNKS.open(encoding="utf-8") as f:
        for line in f:
            doc = json.loads(line)
            s = score(query, doc.get("text", ""))
            if s:
                rows.append((s, doc))
    rows.sort(key=lambda x: -x[0])
    for s, doc in rows[:5]:
        preview = doc["text"][:280].replace("\n", " ")
        print(f"[{s}] {doc['source']} #{doc['index']}: {preview}…")


if __name__ == "__main__":
    main()
