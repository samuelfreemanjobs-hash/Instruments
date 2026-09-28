#!/usr/bin/env bash
# KR-106 vs Junova for all fixture scenarios (optional CI / nightly).
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
FIXTURES="$ROOT/Junova-X/fixtures/kr106_scenarios.json"
KR106="${KR106_REF_DIR:-$ROOT/.reference/ultramaster_kr106}"
RENDER_KR="$KR106/tools/render-midi/render_midi"

if [[ ! -x "$RENDER_KR" ]]; then
  echo "Skip KR-106 golden batch — build render_midi after setup_kr106_reference.sh" >&2
  exit 0
fi

if [[ ! -f "$FIXTURES" ]]; then
  echo "Missing $FIXTURES" >&2
  exit 1
fi

ids="$(python3 -c "import json; print(' '.join(json.load(open('$FIXTURES'))['scenarios']))")"
for id in $ids; do
  echo "=== scenario $id ==="
  bash "$ROOT/tests/golden/junova/compare_kr106_reference.sh" "$id" || exit 1
done
echo "All KR-106 fixture scenarios passed."
