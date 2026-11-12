#!/usr/bin/env bash
# Run one factory category batch: regen → post pipeline → git commit (bank + manifest).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
BATCH="${1:?usage: run_priority_batch.sh <category-name>|priority-core|rest}"
SEED="${TRAP_LORD_REGEN_SEED:-301508}"

if [[ "$BATCH" == "priority-core" || "$BATCH" == "rest" ]]; then
  python3 tools/regen_bank_batch.py --batch "$BATCH" --seed "$SEED"
  CATS=("$BATCH")
else
  python3 tools/regen_bank_batch.py --categories "$BATCH" --seed "$SEED"
  CATS=("$BATCH")
fi
bash tools/post_regen_pipeline.sh
python3 -c "import json;b=json.load(open('assets/factory/preset_bank.json')); print('TOTAL',len(b['presets'])); print(json.dumps(b.get('achievedQuotas',{}),indent=2))"
git add assets/factory/preset_bank.json assets/factory/audition_manifest.json
git add tools/regen_bank_batch.py tools/run_priority_batch.sh 2>/dev/null || true
git commit -m "feat(trap-lord): factory batch ${BATCH} bank + audition manifest" || true
git push -u origin "$(git branch --show-current)"
