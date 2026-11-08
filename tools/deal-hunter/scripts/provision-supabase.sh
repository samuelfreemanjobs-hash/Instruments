#!/usr/bin/env bash
# Provision or link a Supabase project, apply migrations, write local .env (not Cursor secrets).
#
# Prerequisites (one of):
#   - supabase login   (interactive browser)
#   - export SUPABASE_ACCESS_TOKEN=sbp_...   (https://supabase.com/dashboard/account/tokens)
#
# Optional:
#   SUPABASE_PROJECT_REF=abcd1234     existing project (skip create)
#   SUPABASE_ORG_ID=...               required to create a new project
#   BFM_PROJECT_NAME=black-friday-madness
#   BFM_REGION=us-east-1
#
# Cursor Apps: after this script, open the app's Secrets form and add the same
# three names printed at the end (values stay on your machine in .env).

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
ENV_FILE="$ROOT/.env"
WEB_SETUP="$ROOT/scripts/setup-web-env.sh"

BFM_PROJECT_NAME="${BFM_PROJECT_NAME:-black-friday-madness}"
BFM_REGION="${BFM_REGION:-us-east-1}"

run_supabase() {
  if command -v supabase >/dev/null 2>&1; then
    supabase "$@"
    return
  fi
  if command -v npx >/dev/null 2>&1; then
    npx --yes supabase@latest "$@"
    return
  fi
  echo "Install Supabase CLI: https://supabase.com/docs/guides/cli" >&2
  exit 1
}

if [[ -z "${SUPABASE_ACCESS_TOKEN:-}" ]] && ! run_supabase projects list &>/dev/null; then
  echo "Run: supabase login"
  echo "Or:  export SUPABASE_ACCESS_TOKEN=sbp_..."
  exit 1
fi

PROJECT_REF="${SUPABASE_PROJECT_REF:-}"
if [[ -z "$PROJECT_REF" ]]; then
  if [[ -z "${SUPABASE_ORG_ID:-}" ]]; then
    echo "Pick a project ref from: supabase projects list"
    echo "Or create one in the dashboard, then: SUPABASE_PROJECT_REF=xxx $0"
    exit 1
  fi
  echo "Creating project ${BFM_PROJECT_NAME}..."
  CREATE_JSON=$(run_supabase projects create "$BFM_PROJECT_NAME" --org-id "$SUPABASE_ORG_ID" --region "$BFM_REGION" --db-pass "$(openssl rand -base64 24)" -o json)
  PROJECT_REF=$(echo "$CREATE_JSON" | sed -n 's/.*"id"[[:space:]]*:[[:space:]]*"\([^"]*\)".*/\1/p' | head -1)
  if [[ -z "$PROJECT_REF" ]]; then
    echo "Create failed or could not parse project ref. Output:" >&2
    echo "$CREATE_JSON" >&2
    exit 1
  fi
fi

echo "Linking project ${PROJECT_REF}..."
(cd "$ROOT" && run_supabase link --project-ref "$PROJECT_REF")

echo "Applying migrations..."
(cd "$ROOT" && run_supabase db push)

API_JSON=$(run_supabase projects api-keys --project-ref "$PROJECT_REF" -o json)
ANON_KEY=$(echo "$API_JSON" | tr ',' '\n' | grep -A1 '"name".*anon' | grep api_key | sed 's/.*"api_key"[[:space:]]*:[[:space:]]*"\([^"]*\)".*/\1/' | head -1)
if [[ -z "$ANON_KEY" ]]; then
  ANON_KEY=$(echo "$API_JSON" | sed -n 's/.*"anon"[[:space:]]*:[[:space:]]*"\([^"]*\)".*/\1/p' | head -1)
fi

SUPABASE_URL="https://${PROJECT_REF}.supabase.co"

touch "$ENV_FILE"
upsert_env() {
  local key=$1 val=$2
  if grep -q "^${key}=" "$ENV_FILE" 2>/dev/null; then
    sed -i "s|^${key}=.*|${key}=${val}|" "$ENV_FILE"
  else
    echo "${key}=${val}" >> "$ENV_FILE"
  fi
}

upsert_env SUPABASE_URL "$SUPABASE_URL"
upsert_env SUPABASE_ANON_KEY "$ANON_KEY"
upsert_env NEXT_PUBLIC_SUPABASE_URL "$SUPABASE_URL"
upsert_env NEXT_PUBLIC_SUPABASE_ANON_KEY "$ANON_KEY"
upsert_env USE_SUPABASE_CATALOG "1"
upsert_env NEXT_PUBLIC_USE_SUPABASE_CATALOG "1"

bash "$WEB_SETUP" 2>/dev/null || true
if [[ -f "$ROOT/web/.env.local" ]]; then
  upsert_env_file() {
    local f="$ROOT/web/.env.local" key=$1 val=$2
    if grep -q "^${key}=" "$f"; then sed -i "s|^${key}=.*|${key}=${val}|" "$f"; else echo "${key}=${val}" >> "$f"; fi
  }
  upsert_env_file SUPABASE_URL "$SUPABASE_URL"
  upsert_env_file SUPABASE_ANON_KEY "$ANON_KEY"
  upsert_env_file NEXT_PUBLIC_SUPABASE_URL "$SUPABASE_URL"
  upsert_env_file NEXT_PUBLIC_SUPABASE_ANON_KEY "$ANON_KEY"
  upsert_env_file USE_SUPABASE_CATALOG "1"
fi

echo ""
echo "=== Supabase ready (local .env updated) ==="
echo "Project: ${SUPABASE_URL}"
echo "Auth redirect: https://black-friday-madness.apps.grok.bot/auth/callback"
echo "             http://localhost:3000/auth/callback"
echo ""
echo "Optional: npm run sync:supabase  (needs SUPABASE_SERVICE_ROLE_KEY in ${ENV_FILE})"
echo ""
echo "Cursor Apps Secrets (paste from ${ENV_FILE} on your machine — not in chat):"
echo "  SUPABASE_URL"
echo "  SUPABASE_ANON_KEY"
echo "  USE_SUPABASE_CATALOG = 1"
echo "Then republish black-friday-madness."
