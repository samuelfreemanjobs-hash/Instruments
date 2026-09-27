#!/usr/bin/env bash
# Create Disklordz Pro product + monthly price in Stripe (test or live key). Prints STRIPE_PRO_PRICE_ID.
set -euo pipefail
# shellcheck disable=SC1091
source "$(dirname "$0")/lib/common.sh"
load_go_live_env

require_env STRIPE_SECRET_KEY

AMOUNT_CENTS="${STRIPE_PRO_AMOUNT_CENTS:-1299}"
CURRENCY="${STRIPE_PRO_CURRENCY:-usd}"
PRODUCT_NAME="${STRIPE_PRO_PRODUCT_NAME:-Disklordz Pro}"

log "Creating Stripe product: $PRODUCT_NAME"

prod_resp=$(curl -s -u "${STRIPE_SECRET_KEY}:" https://api.stripe.com/v1/products \
  -d "name=${PRODUCT_NAME}" \
  -d "description=Unlimited drum kit generations + saved library")

prod_id=$(echo "$prod_resp" | python3 -c "import sys,json; d=json.load(sys.stdin); print(d.get('id',''))")
if [ -z "$prod_id" ]; then
  echo "$prod_resp"
  die "Failed to create product"
fi

log "Product id: $prod_id"

price_resp=$(curl -s -u "${STRIPE_SECRET_KEY}:" https://api.stripe.com/v1/prices \
  -d "product=${prod_id}" \
  -d "unit_amount=${AMOUNT_CENTS}" \
  -d "currency=${CURRENCY}" \
  -d "recurring[interval]=month")

price_id=$(echo "$price_resp" | python3 -c "import sys,json; d=json.load(sys.stdin); print(d.get('id',''))")
if [ -z "$price_id" ]; then
  echo "$price_resp"
  die "Failed to create price"
fi

echo ""
echo "STRIPE_PRO_PRICE_ID=$price_id"
echo "Add to .env.go-live, GitHub Actions secrets, and Vercel env."
