#!/usr/bin/env bash
# Push env vars from GitHub Actions secrets (env) to Vercel project API.
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
    url="${url}?teamId=${TEAM}"
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
  api POST "/v10/projects/${PROJECT}/env" "$(python3 -c "
import json,sys
print(json.dumps({
  'key': sys.argv[1],
  'value': sys.argv[2],
  'type': 'encrypted',
  'target': [sys.argv[3]],
}))
" "$key" "$value" "$target")" >/dev/null
  echo "Vercel env: $key ($target)"
}

for target in production preview; do
  upsert_env NEXT_PUBLIC_SUPABASE_URL "${NEXT_PUBLIC_SUPABASE_URL:-}" "$target"
  upsert_env NEXT_PUBLIC_SUPABASE_ANON_KEY "${NEXT_PUBLIC_SUPABASE_ANON_KEY:-}" "$target"
  upsert_env SUPABASE_SERVICE_ROLE_KEY "${SUPABASE_SERVICE_ROLE_KEY:-}" "$target"
  upsert_env STRIPE_SECRET_KEY "${STRIPE_SECRET_KEY:-}" "$target"
  upsert_env STRIPE_WEBHOOK_SECRET "${STRIPE_WEBHOOK_SECRET:-}" "$target"
  upsert_env STRIPE_PRO_PRICE_ID "${STRIPE_PRO_PRICE_ID:-}" "$target"
  upsert_env SAAS_DAILY_GEN_LIMIT "${SAAS_DAILY_GEN_LIMIT:-20}" "$target"
done

echo "Vercel env sync done."
