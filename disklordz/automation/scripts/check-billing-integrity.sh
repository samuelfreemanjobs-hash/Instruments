#!/usr/bin/env bash
# Static billing integrity checks (WO-SAAS-020). No Stripe API calls.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
WEB="$ROOT/disklordz/website"
fail=0

note() { echo "$*"; }
bad() { echo "FAIL: $*"; fail=1; }

note "==> WO-SAAS-020 billing integrity (static)"

webhook="$WEB/src/app/api/stripe/webhook/route.ts"
for ev in checkout.session.completed customer.subscription.updated customer.subscription.deleted; do
  if grep -q "\"$ev\"" "$webhook" 2>/dev/null || grep -q "'$ev'" "$webhook" 2>/dev/null; then
    note "OK: webhook handles $ev"
  else
    bad "webhook missing handler for $ev"
  fi
done

env_example="$WEB/.env.example"
if [[ -f "$env_example" ]]; then
  for var in STRIPE_SECRET_KEY STRIPE_WEBHOOK_SECRET STRIPE_PRO_PRICE_ID; do
    if grep -q "$var" "$env_example"; then note "OK: .env.example documents $var"; else bad ".env.example missing $var"; fi
  done
else
  bad "missing $env_example"
fi

if [[ -f "$WEB/src/app/api/credits/route.ts" ]]; then
  note "OK: credits API route present"
else
  bad "missing credits route"
fi

if [[ "$fail" -ne 0 ]]; then exit 1; fi
note "All billing static checks passed."
