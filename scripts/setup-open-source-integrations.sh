#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"

echo "== Disklordz open-source integrations setup =="

python3 "$ROOT/disklordz/rag/scripts/chunk_corpus.py"

if [[ -d "$ROOT/disklordz/integrations/.venv" ]]; then
  # shellcheck disable=SC1091
  source "$ROOT/disklordz/integrations/.venv/bin/activate"
else
  python3 -m venv "$ROOT/disklordz/integrations/.venv"
  # shellcheck disable=SC1091
  source "$ROOT/disklordz/integrations/.venv/bin/activate"
  pip install -q -r "$ROOT/disklordz/integrations/requirements-optional.txt" || true
fi

if [[ -d "$ROOT/disklordz/integrations/mcp/disklordz-mcp-server" ]]; then
  (cd "$ROOT/disklordz/integrations/mcp/disklordz-mcp-server" && npm ci && npm run build) || true
fi

(cd "$ROOT/disklordz/website" && npm ci && npm run build)

echo "Done. Check: cd disklordz/website && npm run dev"
echo "Status: curl -s http://localhost:3000/api/integrations/status | head"
