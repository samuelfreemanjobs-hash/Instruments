#!/usr/bin/env python3
"""Build chunks.jsonl from corpus/manifest.json (stdlib only)."""

from __future__ import annotations

import hashlib
import json
import re
from pathlib import Path

RAG_ROOT = Path(__file__).resolve().parents[1]
REPO_ROOT = RAG_ROOT.parents[1]
MANIFEST = RAG_ROOT / "corpus" / "manifest.json"
OUT = RAG_ROOT / "data" / "chunks.jsonl"
CHUNK_CHARS = 1200
OVERLAP = 200


def chunk_text(text: str, source: str) -> list[dict]:
    text = re.sub(r"\n{3,}", "\n\n", text.strip())
    if not text:
        return []
    chunks: list[dict] = []
    start = 0
    idx = 0
    while start < len(text):
        end = min(start + CHUNK_CHARS, len(text))
        piece = text[start:end]
        cid = hashlib.sha256(f"{source}:{idx}:{piece[:80]}".encode()).hexdigest()[:16]
        chunks.append(
            {
                "id": cid,
                "source": source,
                "index": idx,
                "text": piece,
            }
        )
        if end >= len(text):
            break
        start = end - OVERLAP
        idx += 1
    return chunks


def main() -> None:
    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))
    sources = manifest.get("sources") or []
    OUT.parent.mkdir(parents=True, exist_ok=True)
    total = 0
    with OUT.open("w", encoding="utf-8") as out:
        for rel in sources:
            path = REPO_ROOT / rel
            if not path.is_file():
                print(f"skip missing: {rel}")
                continue
            body = path.read_text(encoding="utf-8")
            for ch in chunk_text(body, rel):
                out.write(json.dumps(ch, ensure_ascii=False) + "\n")
                total += 1
    print(f"Wrote {total} chunks → {OUT.relative_to(REPO_ROOT)}")


if __name__ == "__main__":
    main()
