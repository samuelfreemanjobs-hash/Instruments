#!/usr/bin/env bash
# Local or CI: migrations → auth URLs → catalog sync → optional verify.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
STEP="${1:-all}"
LIVE_URL="${BFM_LIVE_URL:-https://black-friday-madness.apps.grok.bot}"

run_supabase() {
  if command -v supabase >/dev/null 2>&1; then
    supabase "$@"
    return
  fi
  npx --yes supabase@latest "$@"
}

do_migrate() {
  if [[ -z "${SUPABASE_PROJECT_REF:-}" ]]; then
    echo "Skip migrate: set SUPABASE_PROJECT_REF" >&2
    return 0
  fi
  (cd "$ROOT" && run_supabase link --project-ref "$SUPABASE_PROJECT_REF")
  (cd "$ROOT" && run_supabase db push)
}

do_auth() {
  BFM_LIVE_URL="$LIVE_URL" SUPABASE_PROJECT_REF="${SUPABASE_PROJECT_REF:-}" SUPABASE_ACCESS_TOKEN="${SUPABASE_ACCESS_TOKEN:-}" \
    bash "$ROOT/scripts/bfm-configure-auth-urls.sh"
}

do_sync() {
  local url="${SUPABASE_URL:-}"
  if [[ -z "$url" && -n "${SUPABASE_PROJECT_REF:-}" ]]; then
    url="https://${SUPABASE_PROJECT_REF}.supabase.co"
  fi
  if [[ -z "$url" || -z "${SUPABASE_SERVICE_ROLE_KEY:-}" ]]; then
    echo "::error::Set SUPABASE_URL (or SUPABASE_PROJECT_REF) and SUPABASE_SERVICE_ROLE_KEY" >&2
    exit 1
  fi
  export SUPABASE_URL="$url"
  export USE_SUPABASE_CATALOG=1
  (cd "$ROOT" && npm run sync:supabase)
}

do_verify() {
  export BFM_LIVE_URL="$LIVE_URL"
  (cd "$ROOT" && npx tsx scripts/verify-bfm-live.ts)
}

case "$STEP" in
  migrate) do_migrate ;;
  auth-urls) do_auth ;;
  sync) do_sync ;;
  verify) do_verify ;;
  all)
    do_migrate
    do_auth
    do_sync
    do_verify
    ;;
  *)
    echo "Usage: $0 {migrate|auth-urls|sync|verify|all}" >&2
    exit 1
    ;;
esac
