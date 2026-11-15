#!/usr/bin/env bash
# Creates tools/deal-hunter/web/.env.local from templates and optional root .env
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
WEB_ENV="$ROOT/web/.env.local"
ROOT_ENV="$ROOT/.env"
EXAMPLE="$ROOT/web/.env.example"

if [[ -f "$WEB_ENV" ]]; then
  echo "Exists: $WEB_ENV (delete to recreate)"
  exit 0
fi

cp "$EXAMPLE" "$WEB_ENV"

# Promote shared keys from API .env when present (local dev only — never commit .env)
if [[ -f "$ROOT_ENV" ]]; then
  while IFS='=' read -r key val; do
    [[ -z "$key" ]] && continue
    if grep -q "^${key}=" "$WEB_ENV" 2>/dev/null; then
      sed -i "s|^${key}=.*|${key}=${val}|" "$WEB_ENV"
    elif grep -q "^# ${key}" "$WEB_ENV" 2>/dev/null; then
      :
    else
      echo "${key}=${val}" >> "$WEB_ENV"
    fi
  done < <(grep -E '^(SUPABASE_URL|SUPABASE_ANON_KEY|NEXT_PUBLIC_SUPABASE_URL|NEXT_PUBLIC_SUPABASE_ANON_KEY|DEAL_API_ORIGIN|USE_SUPABASE_CATALOG|NEXT_PUBLIC_USE_SUPABASE_CATALOG)=' "$ROOT_ENV" 2>/dev/null || true)

  # Mirror service URL to NEXT_PUBLIC for local Next if only SUPABASE_* set on API env
  if grep -q '^SUPABASE_URL=' "$ROOT_ENV" 2>/dev/null; then
    url=$(grep '^SUPABASE_URL=' "$ROOT_ENV" | cut -d= -f2-)
    sed -i "s|^NEXT_PUBLIC_SUPABASE_URL=.*|NEXT_PUBLIC_SUPABASE_URL=${url}|" "$WEB_ENV" || echo "NEXT_PUBLIC_SUPABASE_URL=${url}" >> "$WEB_ENV"
  fi
  if grep -q '^SUPABASE_ANON_KEY=' "$ROOT_ENV" 2>/dev/null; then
    anon=$(grep '^SUPABASE_ANON_KEY=' "$ROOT_ENV" | cut -d= -f2-)
    sed -i "s|^NEXT_PUBLIC_SUPABASE_ANON_KEY=.*|NEXT_PUBLIC_SUPABASE_ANON_KEY=${anon}|" "$WEB_ENV" || echo "NEXT_PUBLIC_SUPABASE_ANON_KEY=${anon}" >> "$WEB_ENV"
  fi
fi

# Sensible local defaults
grep -q '^DEAL_API_ORIGIN=' "$WEB_ENV" && sed -i 's|^DEAL_API_ORIGIN=.*|DEAL_API_ORIGIN=http://127.0.0.1:3847|' "$WEB_ENV"

echo "Wrote $WEB_ENV"
echo "Next: apply SQL in supabase/migrations (see supabase/README.md), then npm run dev"
