#!/usr/bin/env bash
# Validate Inngest configuration and print the app sync URL for Inngest Cloud.
set -euo pipefail

BASE="${DISKLORDZ_URL:-http://127.0.0.1:3000}"
BASE="${BASE%/}"
SYNC_URL="${BASE}/api/inngest"

echo "Inngest app sync URL (add in Inngest Cloud → Apps → Sync new app):"
echo "  ${SYNC_URL}"
echo ""

if [ -n "${INNGEST_EVENT_KEY:-}" ] && [ -n "${INNGEST_SIGNING_KEY:-}" ]; then
  echo "OK: INNGEST_EVENT_KEY and INNGEST_SIGNING_KEY are set."
else
  echo "WARN: Missing Inngest keys. Create an app at https://app.inngest.com"
  echo "      Copy Event Key → INNGEST_EVENT_KEY, Signing Key → INNGEST_SIGNING_KEY"
  echo "      Add both to Vercel (production + preview) and redeploy."
fi

if [ -n "${INNGEST_DEV:-}" ]; then
  echo "Local dev: run 'cd disklordz/website && npx inngest-cli@latest dev -u ${SYNC_URL}'"
fi

code=$(curl -s -o /dev/null -w "%{http_code}" "${SYNC_URL}" || true)
echo "Probe GET ${SYNC_URL} → HTTP ${code} (200 expected when site is up)"
