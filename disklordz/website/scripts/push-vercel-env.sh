#!/usr/bin/env bash
# Push env vars to Vercel from your shell (values never committed to git).
# Prereq: cd disklordz/website && vercel link
set -euo pipefail

cd "$(dirname "$0")/.."

if ! command -v vercel >/dev/null 2>&1; then
  echo "Install: npm i -g vercel"
  exit 1
fi

add_env() {
  local name="$1"
  local value="$2"
  local target="${3:-production}"
  if [ -z "$value" ]; then
    echo "Skip $name (empty)"
    return
  fi
  printf '%s' "$value" | vercel env add "$name" "$target" --force
  echo "Set $name ($target)"
}

# Read from your environment or paste when prompted
read -r -p "NEXT_PUBLIC_SUPABASE_URL: " NEXT_PUBLIC_SUPABASE_URL
read -r -p "NEXT_PUBLIC_SUPABASE_ANON_KEY: " NEXT_PUBLIC_SUPABASE_ANON_KEY
read -r -s -p "SUPABASE_SERVICE_ROLE_KEY: " SUPABASE_SERVICE_ROLE_KEY
echo
read -r -s -p "STRIPE_SECRET_KEY (optional): " STRIPE_SECRET_KEY
echo
read -r -s -p "STRIPE_WEBHOOK_SECRET (optional): " STRIPE_WEBHOOK_SECRET
echo
read -r -p "STRIPE_PRO_PRICE_ID (optional): " STRIPE_PRO_PRICE_ID
read -r -p "SAAS_DAILY_GEN_LIMIT [20]: " SAAS_DAILY_GEN_LIMIT
SAAS_DAILY_GEN_LIMIT="${SAAS_DAILY_GEN_LIMIT:-20}"

for target in production preview; do
  add_env NEXT_PUBLIC_SUPABASE_URL "$NEXT_PUBLIC_SUPABASE_URL" "$target"
  add_env NEXT_PUBLIC_SUPABASE_ANON_KEY "$NEXT_PUBLIC_SUPABASE_ANON_KEY" "$target"
  add_env SUPABASE_SERVICE_ROLE_KEY "$SUPABASE_SERVICE_ROLE_KEY" "$target"
  add_env SAAS_DAILY_GEN_LIMIT "$SAAS_DAILY_GEN_LIMIT" "$target"
  add_env STRIPE_SECRET_KEY "$STRIPE_SECRET_KEY" "$target"
  add_env STRIPE_WEBHOOK_SECRET "$STRIPE_WEBHOOK_SECRET" "$target"
  add_env STRIPE_PRO_PRICE_ID "$STRIPE_PRO_PRICE_ID" "$target"
done

echo "Done. Redeploy: vercel deploy --prod  (or Redeploy in Vercel UI)"
