#!/usr/bin/env bash
# Configure Supabase Auth site URL + redirect allow-list for the published PWA.
set -euo pipefail

LIVE_URL="${BFM_LIVE_URL:-https://black-friday-madness.apps.grok.bot}"
LIVE_URL="${LIVE_URL%/}"
REDIRECT="${LIVE_URL}/auth/callback"
PROJECT_REF="${SUPABASE_PROJECT_REF:-}"
TOKEN="${SUPABASE_ACCESS_TOKEN:-}"

if [[ -z "$PROJECT_REF" || -z "$TOKEN" ]]; then
  echo "Skip auth URL API: set SUPABASE_PROJECT_REF and SUPABASE_ACCESS_TOKEN" >&2
  echo "Or in Dashboard → Authentication → URL configuration:" >&2
  echo "  Site URL: ${LIVE_URL}" >&2
  echo "  Redirect: ${REDIRECT}" >&2
  exit 0
fi

LOCAL_REDIRECT="http://localhost:3000/auth/callback"
ALLOW_LIST="${REDIRECT},${LOCAL_REDIRECT}"

export LIVE_URL ALLOW_LIST
BODY=$(python3 - <<'PY'
import json, os
print(json.dumps({
  "site_url": os.environ["LIVE_URL"],
  "uri_allow_list": os.environ["ALLOW_LIST"],
}))
PY
)

HTTP=$(curl -sS -o /tmp/bfm-auth-config.json -w "%{http_code}" \
  -X PATCH "https://api.supabase.com/v1/projects/${PROJECT_REF}/config/auth" \
  -H "Authorization: Bearer ${TOKEN}" \
  -H "Content-Type: application/json" \
  -d "$BODY")

if [[ "$HTTP" != "200" && "$HTTP" != "204" ]]; then
  echo "Auth config API returned HTTP ${HTTP} (set URLs manually in Dashboard):" >&2
  cat /tmp/bfm-auth-config.json >&2 || true
  echo "  Site URL: ${LIVE_URL}" >&2
  echo "  Redirect: ${REDIRECT}" >&2
  exit 0
fi

echo "Supabase Auth URLs configured for ${LIVE_URL}"
