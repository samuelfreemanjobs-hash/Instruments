#!/usr/bin/env bash
# One category: regen → post_regen_pipeline → commit bank + manifest (repo root = Instruments).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
REPO="$(cd "$ROOT/../.." && pwd)"
CATEGORY="${1:?usage: run_category_batch_commit.sh <Category> [seed]}"
SEED="${2:-301520}"
export PYTHONUNBUFFERED=1

cd "$ROOT"
echo "=== Regen: $CATEGORY seed=$SEED ==="
python3 -u tools/regen_bank_batch.py --categories "$CATEGORY" --seed "$SEED" \
  2>&1 | tee "build/batch-${CATEGORY//\//_}-regen.log"
bash tools/post_regen_pipeline.sh

cd "$REPO"
git add disklordz/trap-lord-synth/assets/factory/preset_bank.json \
  disklordz/trap-lord-synth/assets/factory/audition_manifest.json
git diff --cached --quiet && { echo "Nothing to commit for $CATEGORY"; exit 0; }
git commit -m "feat(trap-lord): batch ${CATEGORY} regen with culled bank"
git push -u origin "$(git branch --show-current)"
