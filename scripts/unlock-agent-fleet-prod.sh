#!/usr/bin/env bash
# Unlock Tier C fleet agents in CI: verify prod URL, set GitHub secret, dispatch smoke workflows.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

REPO="${GITHUB_REPOSITORY:-samuelfreemanjobs-hash/Instruments}"
URL="${1:-}"
URL="${URL:-${DISKLORDZ_VERIFY_BASE_URL:-${DISKLORDZ_URL:-}}}"

usage() {
  cat <<'EOF'
Usage: unlock-agent-fleet-prod.sh <production-url>

Example:
  ./scripts/unlock-agent-fleet-prod.sh https://your-app.vercel.app

Steps:
  1. verify-go-live + verify-integrations against the URL
  2. gh secret set DISKLORDZ_VERIFY_BASE_URL (requires gh auth with admin)
  3. workflow_dispatch agent-fleet-mvp-motion + agent-fleet-health

If go-live already set secret DISKLORDZ_URL, merged workflows also fall back to it (no duplicate needed).
EOF
}

if [[ -z "$URL" ]]; then
  usage >&2
  exit 1
fi

URL="${URL%/}"
export DISKLORDZ_URL="$URL"

echo "=== 1/4 Go-live smoke ==="
bash disklordz/website/scripts/verify-go-live.sh

echo "=== 2/4 Integration status ==="
bash disklordz/integrations/scripts/verify-integrations.sh

echo "=== 3/4 GitHub secret ==="
if gh auth status >/dev/null 2>&1; then
  printf '%s' "$URL" | gh secret set DISKLORDZ_VERIFY_BASE_URL --repo "$REPO"
  echo "Set DISKLORDZ_VERIFY_BASE_URL on $REPO"
else
  echo "gh not authenticated — set manually:"
  echo "  gh secret set DISKLORDZ_VERIFY_BASE_URL --repo $REPO"
  echo "  (paste: $URL)"
fi

echo "=== 4/4 Dispatch fleet workflows ==="
if gh workflow run agent-fleet-mvp-motion.yml --repo "$REPO" 2>/dev/null; then
  gh workflow run agent-fleet-health.yml --repo "$REPO" 2>/dev/null || true
  echo "Dispatched MVP motion + fleet health (check Actions tab)."
else
  echo "Could not dispatch workflows (permissions). Run manually:"
  echo "  gh workflow run agent-fleet-mvp-motion.yml --repo $REPO"
  echo "  gh workflow run agent-fleet-health.yml --repo $REPO"
fi

echo ""
echo "Done. Tier C agents unlock on the next scheduled CI run."
