#!/usr/bin/env bash
# Validate tooling + env before go-live. Exit 1 if required production vars missing.
set -euo pipefail
# shellcheck disable=SC1091
source "$(dirname "$0")/lib/common.sh"
load_go_live_env

MODE="${1:-production}"
STRICT="${PREFLIGHT_STRICT:-1}"

log "Disklordz go-live preflight (mode=$MODE)"

require_cmd curl
require_cmd python3
require_cmd node
require_cmd npm

if command -v supabase >/dev/null; then echo "  OK   supabase CLI"; else echo "  —    supabase CLI (needed for migrate)"; fi
if command -v vercel >/dev/null; then echo "  OK   vercel CLI"; else echo "  —    vercel CLI (optional; use sync-vercel-env API instead)"; fi
if command -v gh >/dev/null; then echo "  OK   gh CLI"; else echo "  —    gh CLI (optional; check-github-secrets)"; fi

missing=0

check_req() {
  local name="$1"
  if [ -z "${!name:-}" ]; then
    echo "  FAIL $name"
    missing=1
  else
    echo "  OK   $name"
  fi
}

if [ "$MODE" = "production" ]; then
  log "Required for full production go-live:"
  check_req DISKLORDZ_URL
  check_req SUPABASE_ACCESS_TOKEN
  check_req SUPABASE_PROJECT_REF
  check_req VERCEL_DEPLOY_HOOK_URL

  log "Required on Vercel (sync via GitHub secrets or .env.go-live):"
  check_req NEXT_PUBLIC_SUPABASE_URL
  check_req NEXT_PUBLIC_SUPABASE_ANON_KEY
  check_req SUPABASE_SERVICE_ROLE_KEY

  log "Billing (Pro):"
  optional_env STRIPE_SECRET_KEY
  optional_env STRIPE_PRO_PRICE_ID
  optional_env STRIPE_WEBHOOK_SECRET
  if [ -z "${STRIPE_SECRET_KEY:-}" ] || [ -z "${STRIPE_PRO_PRICE_ID:-}" ] || [ -z "${STRIPE_WEBHOOK_SECRET:-}" ]; then
    warn "Stripe incomplete — guest + free accounts still work; Pro checkout will not."
  fi
elif [ "$MODE" = "local" ]; then
  log "Local smoke — no production secrets required."
else
  die "Unknown mode: $MODE (use production|local)"
fi

if [ "$missing" -eq 1 ] && [ "$STRICT" = "1" ] && [ "$MODE" = "production" ]; then
  die "Preflight failed. Copy .env.go-live.example → .env.go-live or use GitHub Actions secrets."
fi

log "Preflight complete."
exit 0
