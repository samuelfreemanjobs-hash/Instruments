#!/usr/bin/env bash
# List which expected GitHub Actions secrets exist (names only — never values).
set -euo pipefail
# shellcheck disable=SC1091
source "$(dirname "$0")/lib/common.sh"

require_cmd gh

REPO="${GITHUB_REPOSITORY:-}"
if [ -z "$REPO" ]; then
  REPO=$(gh repo view --json nameWithOwner -q .nameWithOwner 2>/dev/null || true)
fi
[ -n "$REPO" ] || die "Run from a git repo with gh auth, or set GITHUB_REPOSITORY"

EXPECTED=(
  SUPABASE_ACCESS_TOKEN
  SUPABASE_PROJECT_REF
  DISKLORDZ_URL
  VERCEL_DEPLOY_HOOK_URL
  VERCEL_TOKEN
  VERCEL_PROJECT_ID
  NEXT_PUBLIC_SUPABASE_URL
  NEXT_PUBLIC_SUPABASE_ANON_KEY
  SUPABASE_SERVICE_ROLE_KEY
  STRIPE_SECRET_KEY
  STRIPE_WEBHOOK_SECRET
  STRIPE_PRO_PRICE_ID
  SAAS_DAILY_GEN_LIMIT
)

log "GitHub secrets for $REPO (presence only)"

existing=$(gh secret list --repo "$REPO" 2>/dev/null | awk '{print $1}' || true)

for name in "${EXPECTED[@]}"; do
  if echo "$existing" | grep -qx "$name"; then
    echo "  OK   $name"
  else
    echo "  —    $name"
  fi
done

log "Set missing secrets: docs/DISKLORDZ_GO_LIVE_SECRETS.md"
log "Or run: bash scripts/print-secret-set-commands.sh"
