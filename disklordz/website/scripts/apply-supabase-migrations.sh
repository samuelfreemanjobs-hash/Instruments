#!/usr/bin/env bash
# Apply migrations in supabase/migrations/ to a linked Supabase project.
# Local: export SUPABASE_ACCESS_TOKEN=... && supabase link --project-ref YOUR_REF
set -euo pipefail
cd "$(dirname "$0")/.."

if ! command -v supabase >/dev/null; then
  echo "Install Supabase CLI: https://supabase.com/docs/guides/cli/getting-started"
  exit 1
fi

if [ -z "${SUPABASE_ACCESS_TOKEN:-}" ]; then
  echo "Set SUPABASE_ACCESS_TOKEN (dashboard → Account → Access tokens)"
  exit 1
fi

if [ ! -f supabase/.temp/project-ref ] && [ -z "${SUPABASE_PROJECT_REF:-}" ]; then
  echo "Run: supabase link --project-ref YOUR_PROJECT_REF"
  exit 1
fi

if [ -n "${SUPABASE_PROJECT_REF:-}" ]; then
  supabase link --project-ref "$SUPABASE_PROJECT_REF"
fi

supabase db push
echo "Migrations applied."
