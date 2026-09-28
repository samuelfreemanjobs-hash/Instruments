#!/usr/bin/env bash
# Local go-live orchestrator (mirrors GitHub Actions workflow).
set -euo pipefail
cd "$(dirname "$0")/.."

RUN_MIGRATE=true
SYNC_VERCEL=false
DEPLOY=true
SMOKE=true
STRIPE_WEBHOOK=false

while [ $# -gt 0 ]; do
  case "$1" in
    --no-migrate) RUN_MIGRATE=false ;;
    --sync-vercel) SYNC_VERCEL=true ;;
    --no-deploy) DEPLOY=false ;;
    --no-smoke) SMOKE=false ;;
    --stripe-webhook) STRIPE_WEBHOOK=true ;;
    -h|--help)
      echo "Usage: $0 [--sync-vercel] [--stripe-webhook] [--no-migrate] [--no-deploy] [--no-smoke]"
      echo "Requires env vars; see docs/DISKLORDZ_GO_LIVE_SECRETS.md"
      exit 0
      ;;
    *) echo "Unknown: $1"; exit 1 ;;
  esac
  shift
done

if $RUN_MIGRATE; then
  bash scripts/apply-supabase-migrations.sh
fi

if $STRIPE_WEBHOOK; then
  bash scripts/setup-stripe-webhook.sh
fi

if $SYNC_VERCEL; then
  bash scripts/sync-vercel-env.sh
fi

if $DEPLOY; then
  HOOK="${VERCEL_DEPLOY_HOOK_URL:?Set VERCEL_DEPLOY_HOOK_URL or use --no-deploy}"
  curl -s -X POST "$HOOK" >/dev/null
  echo "Triggered Vercel deploy hook."
fi

if $SMOKE; then
  export DISKLORDZ_URL="${DISKLORDZ_URL:?Set DISKLORDZ_URL}"
  echo "Waiting 45s for deploy..."
  sleep 45
  npm run verify:go-live
fi

echo "Go-live script finished."
