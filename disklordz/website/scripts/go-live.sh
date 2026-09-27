#!/usr/bin/env bash
# Local go-live orchestrator (mirrors GitHub Actions workflow).
set -euo pipefail
# shellcheck disable=SC1091
source "$(dirname "$0")/lib/common.sh"
cd_website
load_go_live_env

RUN_MIGRATE=true
SYNC_VERCEL=false
DEPLOY=true
SMOKE=true
STRIPE_WEBHOOK=false
CONFIGURE_AUTH=false
STRIPE_PRICE=false
PREFLIGHT_ONLY=false
DEPLOY_WAIT="${GO_LIVE_DEPLOY_WAIT:-45}"

while [ $# -gt 0 ]; do
  case "$1" in
    --no-migrate) RUN_MIGRATE=false ;;
    --sync-vercel) SYNC_VERCEL=true ;;
    --no-deploy) DEPLOY=false ;;
    --no-smoke) SMOKE=false ;;
    --stripe-webhook) STRIPE_WEBHOOK=true ;;
    --configure-auth) CONFIGURE_AUTH=true ;;
    --stripe-price) STRIPE_PRICE=true ;;
    --preflight-only) PREFLIGHT_ONLY=true ;;
    -h|--help)
      cat <<'EOF'
Usage: saas.sh go-live [options]

  --sync-vercel       Push env to Vercel (VERCEL_TOKEN + VERCEL_PROJECT_ID)
  --stripe-webhook    Create Stripe webhook; print whsec once
  --stripe-price      Create Pro product/price; print STRIPE_PRO_PRICE_ID
  --configure-auth    Set Supabase site URL + redirect via Management API
  --no-migrate        Skip supabase db push
  --no-deploy         Skip Vercel deploy hook
  --no-smoke          Skip verify-go-live

Env: copy .env.go-live.example → .env.go-live (gitignored)
EOF
      exit 0
      ;;
    *) die "Unknown flag: $1" ;;
  esac
  shift
done

bash scripts/preflight-go-live.sh production
if $PREFLIGHT_ONLY; then
  exit 0
fi

if $STRIPE_PRICE; then
  bash scripts/setup-stripe-pro-price.sh
fi

if $RUN_MIGRATE; then
  bash scripts/apply-supabase-migrations.sh
fi

if $CONFIGURE_AUTH; then
  bash scripts/configure-supabase-auth.sh
fi

if $STRIPE_WEBHOOK; then
  bash scripts/setup-stripe-webhook.sh
fi

if $SYNC_VERCEL; then
  bash scripts/sync-vercel-env.sh
fi

if $DEPLOY; then
  require_env VERCEL_DEPLOY_HOOK_URL
  curl -fsS -X POST "$VERCEL_DEPLOY_HOOK_URL" >/dev/null
  log "Triggered Vercel deploy hook."
fi

if $SMOKE; then
  require_env DISKLORDZ_URL
  log "Waiting ${DEPLOY_WAIT}s for deploy..."
  sleep "$DEPLOY_WAIT"
  verify_args=()
  if [ "${GO_LIVE_VERIFY_STRICT:-1}" = "1" ]; then
    verify_args=(--require-accounts)
    if [ -n "${STRIPE_SECRET_KEY:-}" ] && [ -n "${STRIPE_WEBHOOK_SECRET:-}" ]; then
      verify_args+=(--require-billing)
    fi
  fi
  bash scripts/verify-go-live.sh "${verify_args[@]}"
fi

log "Go-live script finished."
