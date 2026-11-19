#!/usr/bin/env bash
# Resume thin-first pipeline after Choir…Hits/FX (see run_sequential_factory_batches.sh).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
chmod +x tools/run_category_batch_commit.sh
ORDER=(
  "Bells"
  "Church Keys/EP"
  "Organ"
  "Brass"
  "Synth Keys"
  "Leads"
  "Swells"
)
BASE_SEED=301588
i=0
for cat in "${ORDER[@]}"; do
  seed=$((BASE_SEED + i * 17))
  i=$((i + 1))
  bash tools/run_category_batch_commit.sh "$cat" "$seed"
done
echo "Resume batches complete."
