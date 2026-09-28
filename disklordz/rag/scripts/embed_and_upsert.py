#!/usr/bin/env python3
"""Embed chunks.jsonl and upsert into Supabase prompt_knowledge_chunks (optional OpenAI)."""

from __future__ import annotations

import json
import os
import sys
import urllib.error
import urllib.request
from pathlib import Path

RAG_ROOT = Path(__file__).resolve().parents[1]
CHUNKS = RAG_ROOT / "data" / "chunks.jsonl"


def load_chunks() -> list[dict]:
    if not CHUNKS.is_file():
        print(f"Missing {CHUNKS}; run chunk_corpus.py first", file=sys.stderr)
        sys.exit(1)
    rows: list[dict] = []
    with CHUNKS.open(encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if line:
                rows.append(json.loads(line))
    return rows


def embed_openai(texts: list[str], model: str, api_key: str) -> list[list[float]]:
    body = json.dumps({"input": texts, "model": model}).encode("utf-8")
    req = urllib.request.Request(
        "https://api.openai.com/v1/embeddings",
        data=body,
        headers={
            "Authorization": f"Bearer {api_key}",
            "Content-Type": "application/json",
        },
        method="POST",
    )
    with urllib.request.urlopen(req, timeout=120) as resp:
        payload = json.loads(resp.read().decode("utf-8"))
    data = sorted(payload["data"], key=lambda row: row["index"])
    return [row["embedding"] for row in data]


def supabase_upsert(rows: list[dict], url: str, service_key: str) -> None:
    endpoint = f"{url.rstrip('/')}/rest/v1/prompt_knowledge_chunks"
    body = json.dumps(rows).encode("utf-8")
    req = urllib.request.Request(
        endpoint,
        data=body,
        headers={
            "apikey": service_key,
            "Authorization": f"Bearer {service_key}",
            "Content-Type": "application/json",
            "Prefer": "resolution=merge-duplicates",
        },
        method="POST",
    )
    try:
        with urllib.request.urlopen(req, timeout=120) as resp:
            print(f"Upserted {len(rows)} rows (HTTP {resp.status})")
    except urllib.error.HTTPError as exc:
        detail = exc.read().decode("utf-8", errors="replace")
        print(f"Supabase upsert failed: {exc.code} {detail}", file=sys.stderr)
        sys.exit(1)


def main() -> None:
    if "--dry-run" in sys.argv:
        chunks = load_chunks() if CHUNKS.is_file() else []
        print(
            json.dumps(
                {
                    "dry_run": True,
                    "chunk_count": len(chunks),
                    "needs": ["NEXT_PUBLIC_SUPABASE_URL", "SUPABASE_SERVICE_ROLE_KEY", "OPENAI_API_KEY"],
                }
            )
        )
        return

    supabase_url = os.environ.get("NEXT_PUBLIC_SUPABASE_URL") or os.environ.get("SUPABASE_URL")
    service_key = os.environ.get("SUPABASE_SERVICE_ROLE_KEY")
    api_key = os.environ.get("OPENAI_API_KEY")
    model = os.environ.get("OPENAI_EMBEDDING_MODEL", "text-embedding-3-small")

    if not supabase_url or not service_key:
        print("Set NEXT_PUBLIC_SUPABASE_URL and SUPABASE_SERVICE_ROLE_KEY", file=sys.stderr)
        sys.exit(1)
    if not api_key:
        print("Set OPENAI_API_KEY to embed; chunk-only mode not supported for upsert", file=sys.stderr)
        sys.exit(1)

    chunks = load_chunks()
    batch_size = 32
    for i in range(0, len(chunks), batch_size):
        batch = chunks[i : i + batch_size]
        texts = [c["text"] for c in batch]
        vectors = embed_openai(texts, model, api_key)
        payload = []
        for ch, emb in zip(batch, vectors, strict=True):
            lane_id = None
            source = ch.get("source", "")
            if "/lanes/" in source or source.startswith("docs/lanes/"):
                lane_id = Path(source).stem
            payload.append(
                {
                    "id": ch["id"],
                    "source_path": source,
                    "lane_id": lane_id,
                    "content": ch["text"],
                    "embedding": emb,
                    "metadata": {"index": ch.get("index", 0)},
                }
            )
        supabase_upsert(payload, supabase_url, service_key)
    print("Done.")


if __name__ == "__main__":
    main()
