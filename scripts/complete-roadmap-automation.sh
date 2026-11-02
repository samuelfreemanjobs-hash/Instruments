#!/usr/bin/env bash
# Run all roadmap items that do not require production secrets (best-effort).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

echo "=== 1) Integration wiring verify + promote ==="
python3 disklordz/integrations/scripts/promote-integrations.py --write

echo "=== 2) Agent fleet sync ==="
bash scripts/sync-disklordz-agent-fleet.sh

echo "=== 3) Stub agent env checklist ==="
bash disklordz/agents/workflows/scripts/stub-agent-env-check.sh

echo "=== 4) Optional integration CLI stubs ==="
python3 disklordz/integrations/engines/modal/deploy_stub.py >/dev/null
python3 disklordz/integrations/agents/openai_factory_agent.py >/dev/null
bash disklordz/integrations/engines/cog/predict_stub.sh >/dev/null

echo "=== 5) SaaS 007 smoke (needs dev server on DISKLORDZ_URL) ==="
if curl -sf "${DISKLORDZ_URL:-http://127.0.0.1:3000}/api/integrations/status" >/dev/null; then
  bash scripts/saas-007-smoke.sh
else
  echo "SKIP saas-007-smoke (start disklordz/website dev server)"
fi

echo "=== 6) Roadmap metrics ==="
python3 scripts/update-roadmap-progress.py

echo "Done. See disklordz/integrations/WIRING_REPORT.md and docs/ROADMAP.md"
