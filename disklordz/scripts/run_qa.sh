#!/usr/bin/env bash
# Disklordz monorepo QA — website, daw-inbox, RAG corpus (no Supabase/Vercel required).
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
REPO="$(cd "$ROOT/.." && pwd)"

echo "== Disklordz QA (repo: $REPO) =="

echo "== website: npm ci =="
cd "$ROOT/website"
npm ci

echo "== website: lint =="
npm run lint

echo "== website: build =="
npm run build

echo "== daw-inbox: npm ci + syntax =="
cd "$ROOT/daw-inbox"
npm ci
node --check bin/watch.mjs

echo "== rag: chunk corpus =="
python3 "$ROOT/rag/scripts/chunk_corpus.py"

echo "== rag: query smoke =="
python3 "$ROOT/rag/scripts/query_local.py" "memphis phonk 92 bpm" | head -n 3

echo "== sound-factory: stub kits (optional offline assets) =="
if [[ -x "$(command -v python3)" ]]; then
  python3 "$ROOT/sound-factory/scripts/generate_stub_kits.py" 2>/dev/null || echo "(stub script skipped or non-fatal)"
fi

echo ""
echo "Disklordz QA passed."
