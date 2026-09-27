#!/usr/bin/env bash
# Set Supabase Auth site URL + redirect allow list via Management API.
set -euo pipefail
# shellcheck disable=SC1091
source "$(dirname "$0")/lib/common.sh"
load_go_live_env

require_env SUPABASE_ACCESS_TOKEN "https://supabase.com/dashboard/account/tokens"
require_env SUPABASE_PROJECT_REF "project subdomain"
require_env DISKLORDZ_URL "production URL, no trailing slash"

BASE="${DISKLORDZ_URL%/}"
SITE_URL="$BASE"
REDIRECT="${BASE}/auth/callback"

log "Updating Supabase auth URLs for project $SUPABASE_PROJECT_REF"
log "  site_url=$SITE_URL"
log "  redirect=$REDIRECT"

payload=$(python3 -c "import json; print(json.dumps({'site_url': '$SITE_URL', 'uri_allow_list': '$REDIRECT'}))")

resp=$(curl -s -w "\n%{http_code}" -X PATCH \
  "https://api.supabase.com/v1/projects/${SUPABASE_PROJECT_REF}/config/auth" \
  -H "Authorization: Bearer ${SUPABASE_ACCESS_TOKEN}" \
  -H "Content-Type: application/json" \
  -d "$payload")

code=$(echo "$resp" | tail -n1)
body=$(echo "$resp" | sed '$d')

if [ "$code" != "200" ] && [ "$code" != "201" ]; then
  warn "Management API returned HTTP $code"
  echo "$body" | head -c 800
  echo ""
  warn "Set manually: Supabase Dashboard → Authentication → URL configuration"
  warn "  Site URL: $SITE_URL"
  warn "  Redirect URLs: $REDIRECT"
  exit 1
fi

log "Supabase auth URLs updated."
