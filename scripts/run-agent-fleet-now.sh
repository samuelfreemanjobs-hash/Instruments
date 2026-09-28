#!/usr/bin/env bash
# Run all locally executable agent roles (PM Agent + fleet_execute + optional go-live).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

export DISKLORDZ_URL="${DISKLORDZ_URL:-http://127.0.0.1:3000}"
echo "Agent fleet run → DISKLORDZ_URL=$DISKLORDZ_URL"

bash scripts/sync-disklordz-agent-fleet.sh
python3 disklordz/agents/workflows/fleet_execute.py

echo "Fleet run complete. Roster: DISKLORDZ_AGENTS.md"
