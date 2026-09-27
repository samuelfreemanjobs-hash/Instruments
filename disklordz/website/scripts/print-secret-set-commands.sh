#!/usr/bin/env bash
# Print gh secret set commands (you paste values interactively — nothing stored in repo).
set -euo pipefail
# shellcheck disable=SC1091
source "$(dirname "$0")/lib/common.sh"

REPO="${1:-}"
if [ -z "$REPO" ]; then
  REPO=$(gh repo view --json nameWithOwner -q .nameWithOwner 2>/dev/null || echo "OWNER/Instruments")
fi

cat <<EOF
# Run from repo root with gh authenticated. Paste each value when prompted.
# See docs/DISKLORDZ_GO_LIVE_SECRETS.md

gh secret set SUPABASE_ACCESS_TOKEN --repo $REPO
gh secret set SUPABASE_PROJECT_REF --repo $REPO
gh secret set DISKLORDZ_URL --repo $REPO
gh secret set VERCEL_DEPLOY_HOOK_URL --repo $REPO
gh secret set VERCEL_TOKEN --repo $REPO
gh secret set VERCEL_PROJECT_ID --repo $REPO
gh secret set NEXT_PUBLIC_SUPABASE_URL --repo $REPO
gh secret set NEXT_PUBLIC_SUPABASE_ANON_KEY --repo $REPO
gh secret set SUPABASE_SERVICE_ROLE_KEY --repo $REPO
gh secret set STRIPE_SECRET_KEY --repo $REPO
gh secret set STRIPE_WEBHOOK_SECRET --repo $REPO
gh secret set STRIPE_PRO_PRICE_ID --repo $REPO
gh secret set SAAS_DAILY_GEN_LIMIT --repo $REPO --body "20"
EOF
