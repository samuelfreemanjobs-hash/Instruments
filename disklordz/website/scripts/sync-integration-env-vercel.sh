#!/usr/bin/env bash
# Push integration-related env vars to Vercel (OpenAI, Inngest, remote engines).
set -euo pipefail

TOKEN="${VERCEL_TOKEN:?VERCEL_TOKEN required}"
PROJECT="${VERCEL_PROJECT_ID:?VERCEL_PROJECT_ID required}"
TEAM="${VERCEL_TEAM_ID:-}"

api() {
  local method="$1"
  local path="$2"
  local body="${3:-}"
  local url="https://api.vercel.com${path}"
  if [ -n "$TEAM" ]; then
    case "$url" in *\?*) url="${url}&teamId=${TEAM}" ;; *) url="${url}?teamId=${TEAM}" ;; esac
  fi
  if [ -n "$body" ]; then
    curl -s -X "$method" "$url" \
      -H "Authorization: Bearer $TOKEN" \
      -H "Content-Type: application/json" \
      -d "$body"
  else
    curl -s -X "$method" "$url" -H "Authorization: Bearer $TOKEN"
  fi
}

upsert_env() {
  local key="$1"
  local value="$2"
  local target="$3"
  [ -n "$value" ] || return 0
  local resp
  resp=$(api POST "/v10/projects/${PROJECT}/env?upsert=true" "$(python3 -c "
import json,sys
print(json.dumps({
  'key': sys.argv[1],
  'value': sys.argv[2],
  'type': 'encrypted',
  'target': [sys.argv[3]],
}))
" "$key" "$value" "$target")")
  if echo "$resp" | python3 -c "import sys,json; sys.exit(0 if 'error' in json.load(sys.stdin) else 1)" 2>/dev/null; then
    echo "::error::Vercel env $key ($target) failed: $(echo "$resp" | python3 -c "import sys,json; print(json.load(sys.stdin)['error'].get('code','unknown'))")"
    return 1
  fi
  echo "Vercel env: $key ($target)"
}

for target in production preview; do
  upsert_env OPENAI_API_KEY "${OPENAI_API_KEY:-}" "$target"
  upsert_env OPENAI_EMBEDDING_MODEL "${OPENAI_EMBEDDING_MODEL:-text-embedding-3-small}" "$target"
  upsert_env OPENAI_CHAT_MODEL "${OPENAI_CHAT_MODEL:-gpt-4o-mini}" "$target"
  upsert_env INNGEST_EVENT_KEY "${INNGEST_EVENT_KEY:-}" "$target"
  upsert_env INNGEST_SIGNING_KEY "${INNGEST_SIGNING_KEY:-}" "$target"
  upsert_env DISKLORDZ_ENGINE "${DISKLORDZ_ENGINE:-parametric}" "$target"
  upsert_env DISKLORDZ_AUDIOCRAFT_ENGINE_URL "${DISKLORDZ_AUDIOCRAFT_ENGINE_URL:-}" "$target"
  upsert_env DISKLORDZ_STABLE_AUDIO_ENGINE_URL "${DISKLORDZ_STABLE_AUDIO_ENGINE_URL:-}" "$target"
done

echo "Integration Vercel env sync done."
