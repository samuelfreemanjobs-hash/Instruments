#!/usr/bin/env bash
# One-shot: four DL-OPP records + A&R greenlights for YT-DL-001…004
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
API="${FACTORY_API_URL:-http://127.0.0.1:8787}"
ARGS=(--base-url "$API")
if [[ -f "$ROOT/database/youtube_research_sources.local.json" ]]; then
  ARGS+=(--sources-overlay "$ROOT/database/youtube_research_sources.local.json")
fi
exec python3 "$ROOT/scripts/ingest_youtube_network_research.py" "${ARGS[@]}" "$@"
