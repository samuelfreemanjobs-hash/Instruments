#!/usr/bin/env bash
# Run after DATABASE_URL is in .env.local (Atlas or local Mongo).
set -euo pipefail
cd "$(dirname "$0")/.."
if [[ ! -f .env.local ]] || ! grep -q DATABASE_URL .env.local; then
  echo "Missing .env.local with DATABASE_URL. See web/docs/DATABASE.md"
  exit 1
fi
set -a && source .env.local && set +a
npx prisma db push
npx tsx prisma/seed.ts
npx tsx scripts/e2e-quick-test.ts
