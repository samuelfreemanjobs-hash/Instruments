#!/usr/bin/env bash
# Create Stripe webhook for Disklordz if missing. Prints signing secret once.
set -euo pipefail

BASE="${DISKLORDZ_URL:?Set DISKLORDZ_URL (no trailing slash)}"
KEY="${STRIPE_SECRET_KEY:?Set STRIPE_SECRET_KEY}"
HOOK_URL="${BASE}/api/stripe/webhook"

EVENTS=(
  checkout.session.completed
  customer.subscription.updated
  customer.subscription.deleted
)

existing=$(curl -s -u "${KEY}:" "https://api.stripe.com/v1/webhook_endpoints?limit=100" \
  | HOOK_URL="$HOOK_URL" python3 -c "import os,sys,json; u=os.environ['HOOK_URL']; d=json.load(sys.stdin); print(next((x['id'] for x in d.get('data',[]) if x.get('url')==u), ''))")

if [ -n "$existing" ]; then
  echo "Webhook already exists: $existing ($HOOK_URL)"
  echo "Retrieve signing secret from Stripe Dashboard → Developers → Webhooks → $existing"
  exit 0
fi

args=(-d "url=${HOOK_URL}")
for ev in "${EVENTS[@]}"; do
  args+=(-d "enabled_events[]=${ev}")
done

resp=$(curl -s -u "${KEY}:" https://api.stripe.com/v1/webhook_endpoints "${args[@]}")
secret=$(echo "$resp" | python3 -c "import sys,json; d=json.load(sys.stdin); print(d.get('secret',''))" 2>/dev/null || true)
id=$(echo "$resp" | python3 -c "import sys,json; d=json.load(sys.stdin); print(d.get('id',''))" 2>/dev/null || true)

if [ -z "$id" ]; then
  echo "Failed to create webhook:"
  echo "$resp"
  exit 1
fi

echo "Created webhook: $id"
echo "STRIPE_WEBHOOK_SECRET=$secret"
echo "Add to GitHub Actions secrets and Vercel env, then redeploy."
