#!/usr/bin/env bash
# Synth Keys → Leads → Swells → Pads; fail fast if any step errors (tee does not preserve exit status).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
REPO="$(cd "$ROOT/../.." && pwd)"
cd "$REPO"
export PYTHONUNBUFFERED=1
chmod +x "$ROOT/tools/run_category_batch_commit.sh"

run_one() {
  local category="$1"
  local seed="$2"
  local log="$ROOT/build/foreground-batch-${category//\//_}.log"
  set +e
  bash "$ROOT/tools/run_category_batch_commit.sh" "$category" "$seed" 6 > >(tee "$log") 2>&1
  local ec=$?
  set -e
  if [[ $ec -ne 0 ]]; then
    echo "FAILED: $category (exit $ec)" >&2
    exit "$ec"
  fi
}

run_one "Synth Keys" 301840
run_one Leads 301841
run_one Swells 301842
run_one Pads 301843
echo "ALL_TOPUP_DONE"