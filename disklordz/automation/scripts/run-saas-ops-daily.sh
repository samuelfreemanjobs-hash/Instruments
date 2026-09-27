#!/usr/bin/env bash
# WO-SAAS-019: SaaS reliability checks (build + optional live smoke).
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
WEB="$ROOT/disklordz/website"
SKIP_LIVE=0
SKIP_AGENT=0

while [[ $# -gt 0 ]]; do
  case "$1" in
    --skip-live) SKIP_LIVE=1; shift ;;
    --skip-agent) SKIP_AGENT=1; shift ;;
    *) echo "unknown: $1" >&2; exit 1 ;;
  esac
done

cd "$WEB"
npm ci
npm run factory:dsp-regression
npm run build
npm run lint
node "$ROOT/disklordz/automation/scripts/check-preset-lanes.mjs"
bash "$ROOT/disklordz/automation/scripts/check-billing-integrity.sh"

if [[ "$SKIP_LIVE" -eq 0 && -n "${DISKLORDZ_URL:-}" ]]; then
  npm run verify:go-live
elif [[ "$SKIP_LIVE" -eq 0 ]]; then
  echo "INFO: DISKLORDZ_URL unset — skip verify:go-live (set for prod smoke)"
fi

if [[ "$SKIP_AGENT" -eq 0 ]]; then
  bash "$ROOT/disklordz/automation/scripts/trigger_saas_ops_agent.sh"
fi

echo "==> SaaS ops daily checks complete"
