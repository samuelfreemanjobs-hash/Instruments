#!/usr/bin/env bash
# Concatenate migrations for Dashboard SQL editor or: supabase db push
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
DIR="$ROOT/supabase/migrations"

if command -v supabase >/dev/null 2>&1 && [[ -f "$ROOT/supabase/config.toml" ]]; then
  (cd "$ROOT" && supabase db push)
  exit $?
fi

echo "-- Run in Supabase Dashboard → SQL (order matters)"
for f in "$DIR"/*.sql; do
  echo ""
  echo "-- ===== $(basename "$f") ====="
  cat "$f"
done
