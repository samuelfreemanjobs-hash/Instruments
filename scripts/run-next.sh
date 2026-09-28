#!/usr/bin/env bash
# Next ops beat after full business: automation + SaaS smoke + CI-verify parity.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
export DISKLORDZ_URL="${DISKLORDZ_URL:-http://127.0.0.1:3000}"

echo "=== 1/4 Roadmap automation ==="
bash scripts/complete-roadmap-automation.sh

echo "=== 2/4 Inngest probe ==="
bash disklordz/integrations/scripts/register-inngest.sh || true

echo "=== 3/4 Go-live smoke ==="
bash disklordz/website/scripts/verify-go-live.sh

echo "=== 4/4 CI-verify (plugin parity, no rebuild) ==="
python3 vst-testing-ops/run_business.py --profile ci-verify

echo ""
echo "Next: docs/NEXT.md (production secrets + deploy)"
