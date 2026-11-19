#!/usr/bin/env bash
# Run thin-first categories then top-up; each step commits to the current branch.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
chmod +x tools/run_category_batch_commit.sh

ORDER=(
  "Choir"
  "Bass"
  "Strings"
  "Hits/FX"
  "Bells"
  "Church Keys/EP"
  "Organ"
  "Brass"
  "Synth Keys"
  "Leads"
  "Swells"
)
BASE_SEED=301520
i=0
for cat in "${ORDER[@]}"; do
  seed=$((BASE_SEED + i * 17))
  i=$((i + 1))
  bash tools/run_category_batch_commit.sh "$cat" "$seed" \
    || { echo "FAILED on $cat"; exit 1; }
done
echo "All sequential batches done."
