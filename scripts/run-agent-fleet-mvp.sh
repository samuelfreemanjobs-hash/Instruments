#!/usr/bin/env bash
# MVP motion: sync fleet + run per-agent hooks + optional full plugin verify.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

export DISKLORDZ_URL="${DISKLORDZ_URL:-http://127.0.0.1:3000}"
FULL="${FLEET_MVP_FULL:-0}"

echo "=== MVP agent motion → DISKLORDZ_URL=$DISKLORDZ_URL (FLEET_MVP_FULL=$FULL) ==="
bash scripts/sync-disklordz-agent-fleet.sh

ARGS=()
if [[ "$FULL" == "1" || "$FULL" == "true" || "$FULL" == "yes" ]]; then
  ARGS+=(--full)
fi
python3 disklordz/agents/workflows/fleet_mvp_motion.py "${ARGS[@]}"

echo ""
echo "Activity: docs/fleet-activity-latest.md"
echo "Unblock prod agents: docs/NEXT.md"
