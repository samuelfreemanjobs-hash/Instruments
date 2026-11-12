#!/usr/bin/env bash
# Resolve SUPABASE_URL + SUPABASE_SERVICE_ROLE_KEY for CI (service_role optional as repo secret).
set -euo pipefail

PROJECT_REF="${SUPABASE_PROJECT_REF:-}"
TOKEN="${SUPABASE_ACCESS_TOKEN:-}"

if [[ -n "${BFM_SUPABASE_URL:-}" ]]; then
  SUPABASE_URL="$BFM_SUPABASE_URL"
elif [[ -n "$PROJECT_REF" ]]; then
  SUPABASE_URL="https://${PROJECT_REF}.supabase.co"
else
  SUPABASE_URL="${SUPABASE_URL:-}"
fi

if [[ -z "${SUPABASE_SERVICE_ROLE_KEY:-}" && -n "$TOKEN" && -n "$PROJECT_REF" ]]; then
  if command -v supabase >/dev/null 2>&1; then
    JSON=$(supabase projects api-keys --project-ref "$PROJECT_REF" -o json)
    SUPABASE_SERVICE_ROLE_KEY=$(echo "$JSON" | python3 - <<'PY'
import json, sys
data = json.load(sys.stdin)
for row in data:
    if row.get("name") == "service_role":
        print(row.get("api_key", ""))
        break
PY
)
  fi
fi

if [[ -z "${SUPABASE_URL:-}" || -z "${SUPABASE_SERVICE_ROLE_KEY:-}" ]]; then
  echo "::error::Need SUPABASE_URL + SUPABASE_SERVICE_ROLE_KEY (or ACCESS_TOKEN + PROJECT_REF for auto-fetch)" >&2
  exit 1
fi

export SUPABASE_URL SUPABASE_SERVICE_ROLE_KEY

if [[ -n "${GITHUB_ENV:-}" ]]; then
  {
    echo "SUPABASE_URL=${SUPABASE_URL}"
    echo "SUPABASE_SERVICE_ROLE_KEY=${SUPABASE_SERVICE_ROLE_KEY}"
  } >> "$GITHUB_ENV"
fi

echo "Resolved Supabase credentials for ${PROJECT_REF:-project}"
