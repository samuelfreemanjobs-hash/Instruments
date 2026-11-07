#!/usr/bin/env bash
# Automate everything repo-side for 808 Lab sprint (no MPC hardware).
# Usage: npm run automate:808-lab-repo [-- --skip-regenerate]
set -euo pipefail
cd "$(dirname "$0")/.."
SKIP_REGEN=0
for arg in "$@"; do
  if [[ "$arg" == "--skip-regenerate" ]]; then SKIP_REGEN=1; fi
done

echo "== 808 Lab repo automation =="
if [[ "$SKIP_REGEN" -eq 0 ]]; then
  echo "→ Regenerating sprint kits 1–5 (factory WAV + verify:spark)…"
  bash scripts/export-sprint-1-5.sh
else
  echo "→ Skipping regenerate (--skip-regenerate)"
  npm run verify:spark
fi

echo "→ MPC500 card folders (in-repo)…"
npm run export:mpc500-sprint-all

echo "→ Phonk engine manifest contract…"
ROOT="$(cd ../.. && pwd)"
python3 "$ROOT/disklordz/phonk-box/engine/test_manifest_load.py"

echo "→ Next.js production build…"
npm run build

echo ""
echo "automate-808-lab-repo: REPO COMPLETE"
echo "Human gate: run npm run verify:producer-signoff after MPC500 sessions."
echo "Optional: copy each sprint-kits/session-*/MPC500/<code>/ to CF card."
