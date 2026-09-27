#!/usr/bin/env bash
# Disklordz Drum SaaS — single entrypoint for dev, smoke, and go-live automation.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

cmd="${1:-help}"
shift || true

run() {
  bash "$ROOT/scripts/$1" "${@:2}"
}

case "$cmd" in
  help|-h|--help)
    cat <<'EOF'
Disklordz SaaS CLI — ./scripts/saas.sh <command>

  dev-setup       npm ci, .env.local stub, stub WAVs
  smoke-local     build + start + verify (guest paths)
  preflight       check tools + env (.env.go-live or exported vars)
  preflight-local preflight for local smoke only

  migrate         supabase db push
  auth-urls       Supabase Management API: site URL + /auth/callback
  stripe-price    create Pro product/price (prints STRIPE_PRO_PRICE_ID)
  stripe-webhook  register Stripe webhook endpoint
  sync-vercel     push env vars to Vercel via API
  deploy          POST VERCEL_DEPLOY_HOOK_URL
  verify          curl smoke against DISKLORDZ_URL

  go-live         full pipeline (see go-live.sh --help)
  gh-secrets      list GitHub Actions secret names (gh auth required)
  gh-secret-cmds  print gh secret set commands

Env file: copy .env.go-live.example → .env.go-live (gitignored)
Docs: docs/DISKLORDZ_SAAS_FINISH.md
EOF
    ;;
  dev-setup) run dev-setup.sh ;;
  smoke-local) run smoke-local.sh "$@" ;;
  preflight) run preflight-go-live.sh production ;;
  preflight-local) run preflight-go-live.sh local ;;
  migrate) run apply-supabase-migrations.sh ;;
  auth-urls) run configure-supabase-auth.sh ;;
  stripe-price) run setup-stripe-pro-price.sh ;;
  stripe-webhook) run setup-stripe-webhook.sh ;;
  sync-vercel) run sync-vercel-env.sh ;;
  deploy)
    # shellcheck disable=SC1091
    source scripts/lib/common.sh
    load_go_live_env
    require_env VERCEL_DEPLOY_HOOK_URL
    curl -fsS -X POST "$VERCEL_DEPLOY_HOOK_URL"
    log "Deploy hook triggered."
    ;;
  verify) run verify-go-live.sh "$@" ;;
  go-live) run go-live.sh "$@" ;;
  gh-secrets) run check-github-secrets.sh ;;
  gh-secret-cmds) run print-secret-set-commands.sh "$@" ;;
  vercel-env-interactive) run push-vercel-env.sh ;;
  *)
    echo "Unknown command: $cmd"
    exec bash "$ROOT/scripts/saas.sh" help
    ;;
esac
