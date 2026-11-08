#!/usr/bin/env bash
# Complete post-merge integration tasks: migrations, RAG index, Vercel env, Inngest, optional engines.
# Safe to re-run; skips steps when required env vars are missing.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
WEB="$ROOT/disklordz/website"
INTEG="$ROOT/disklordz/integrations"

DO_MIGRATE=false
DO_RAG=false
DO_VERCEL=false
DO_DEPLOY=false
DO_INNGEST=false
DO_ENGINES=false
DO_VERIFY=false
DO_BUILD=true

usage() {
  cat <<'EOF'
Usage: activate-integrations.sh [options]

  --all           Enable migrate, rag, vercel, inngest, engines, verify (not deploy)
  --migrate       supabase db push (needs SUPABASE_ACCESS_TOKEN + SUPABASE_PROJECT_REF)
  --rag           chunk_corpus + embed_and_upsert (needs OPENAI + Supabase service role)
  --vercel        sync integration env vars to Vercel (needs VERCEL_TOKEN + PROJECT_ID)
  --deploy        POST VERCEL_DEPLOY_HOOK_URL
  --inngest       validate Inngest env + print sync URL
  --engines       docker compose engine stub profile
  --verify        curl /api/integrations/status (+ optional DISKLORDZ_URL)
  --no-build      skip npm ci && npm run build

Environment: see docs/DISKLORDZ_GO_LIVE_SECRETS.md and disklordz/website/.env.example
EOF
}

while [ $# -gt 0 ]; do
  case "$1" in
    --all) DO_MIGRATE=true; DO_RAG=true; DO_VERCEL=true; DO_INNGEST=true; DO_ENGINES=true; DO_VERIFY=true ;;
    --migrate) DO_MIGRATE=true ;;
    --rag) DO_RAG=true ;;
    --vercel) DO_VERCEL=true ;;
    --deploy) DO_DEPLOY=true ;;
    --inngest) DO_INNGEST=true ;;
    --engines) DO_ENGINES=true ;;
    --verify) DO_VERIFY=true ;;
    --no-build) DO_BUILD=false ;;
    -h|--help) usage; exit 0 ;;
    *) echo "Unknown: $1"; usage; exit 1 ;;
  esac
  shift
done

if ! $DO_MIGRATE && ! $DO_RAG && ! $DO_VERCEL && ! $DO_DEPLOY && ! $DO_INNGEST && ! $DO_ENGINES && ! $DO_VERIFY; then
  DO_MIGRATE=true
  DO_RAG=true
  DO_INNGEST=true
  DO_VERIFY=true
fi

echo "== 1/7 Corpus chunk (local) =="
python3 "$ROOT/disklordz/rag/scripts/chunk_corpus.py"

if $DO_BUILD; then
  echo "== 2/7 Website build =="
  (cd "$WEB" && npm ci && npm run build)
else
  echo "== 2/7 Website build (skipped) =="
fi

if $DO_MIGRATE; then
  echo "== 3/7 Supabase migrations =="
  if [ -n "${SUPABASE_ACCESS_TOKEN:-}" ] && [ -n "${SUPABASE_PROJECT_REF:-}" ]; then
    (cd "$WEB" && bash scripts/apply-supabase-migrations.sh)
  else
    echo "SKIP migrate: set SUPABASE_ACCESS_TOKEN and SUPABASE_PROJECT_REF"
  fi
else
  echo "== 3/7 Supabase migrations (skipped) =="
fi

if $DO_RAG; then
  echo "== 4/7 RAG embed + upsert =="
  if [ -n "${OPENAI_API_KEY:-}" ] && [ -n "${SUPABASE_SERVICE_ROLE_KEY:-}" ] \
    && [ -n "${NEXT_PUBLIC_SUPABASE_URL:-}" ]; then
    python3 "$ROOT/disklordz/rag/scripts/embed_and_upsert.py"
  else
    echo "SKIP rag index: set OPENAI_API_KEY, NEXT_PUBLIC_SUPABASE_URL, SUPABASE_SERVICE_ROLE_KEY"
  fi
else
  echo "== 4/7 RAG index (skipped) =="
fi

if $DO_VERCEL; then
  echo "== 5/7 Vercel env (integrations) =="
  if [ -n "${VERCEL_TOKEN:-}" ] && [ -n "${VERCEL_PROJECT_ID:-}" ]; then
    (cd "$WEB" && bash scripts/sync-vercel-env.sh)
    (cd "$WEB" && bash scripts/sync-integration-env-vercel.sh)
  else
    echo "SKIP vercel: set VERCEL_TOKEN and VERCEL_PROJECT_ID"
  fi
else
  echo "== 5/7 Vercel env (skipped) =="
fi

if $DO_INNGEST; then
  echo "== 6/7 Inngest =="
  bash "$INTEG/scripts/register-inngest.sh"
else
  echo "== 6/7 Inngest (skipped) =="
fi

if $DO_ENGINES; then
  echo "== 7/7 Engine stubs (docker) =="
  if command -v docker >/dev/null; then
    docker compose -f "$INTEG/docker-compose.optional.yml" --profile engines up -d
    echo "Stub workers: audiocraft :8765, stable-audio :8766"
    echo "Set DISKLORDZ_ENGINE=remote and DISKLORDZ_AUDIOCRAFT_ENGINE_URL=http://127.0.0.1:8765"
  else
    echo "SKIP engines: docker not available (Cloud Agent pods have no Docker — run locally)"
  fi
else
  echo "== 7/7 Engine stubs (skipped) =="
fi

if $DO_DEPLOY; then
  if [ -n "${VERCEL_DEPLOY_HOOK_URL:-}" ]; then
    curl -fsS -X POST "$VERCEL_DEPLOY_HOOK_URL" >/dev/null
    echo "Vercel deploy hook triggered."
  else
    echo "SKIP deploy: VERCEL_DEPLOY_HOOK_URL not set"
  fi
fi

if $DO_VERIFY; then
  bash "$INTEG/scripts/verify-integrations.sh"
fi

echo "activate-integrations finished."
