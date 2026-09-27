#!/usr/bin/env bash
# Run static checks for all Disklordz business agents (WO-SAAS-018–023).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
WEB="$ROOT/disklordz/website"
AUTO="$ROOT/disklordz/automation/scripts"

cmd="${1:-check}"
case "$cmd" in
  check)
    cd "$WEB" && npm ci && npm run factory:dsp-regression && npm run build && npm run lint
    node "$AUTO/check-preset-lanes.mjs"
    bash "$AUTO/check-billing-integrity.sh"
    echo "All business-agent static checks passed."
    ;;
  classify)
    shift || true
    echo "${*:-$(cat)}" | node "$AUTO/pm-router-classify.mjs"
    ;;
  *)
    echo "Usage: $0 check | classify [text...]" >&2
    exit 1
    ;;
esac
