#!/usr/bin/env bash
# Verify WO-SAAS-018 automation prerequisites (local or CI diagnostics).
set -euo pipefail

ok=0
warn=0

check() {
  if command -v "$1" >/dev/null 2>&1; then
    echo "OK: $1"
  else
    echo "MISSING: $1"
    ok=1
  fi
}

check node
check npm
check curl
check jq

ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
for f in \
  "$ROOT/disklordz/website/scripts/factory-dsp-regression.ts" \
  "$ROOT/disklordz/automation/scripts/trigger_cursor_factory_daily_agent.sh" \
  "$ROOT/.github/workflows/disklordz-factory-daily.yml"; do
  if [[ -f "$f" ]]; then
    echo "OK: file $(basename "$f")"
  else
    echo "MISSING: $f"
    ok=1
  fi
done

if [[ -n "${CURSOR_API_KEY:-}" ]]; then
  echo "OK: CURSOR_API_KEY is set (agent launch enabled)"
else
  echo "WARN: CURSOR_API_KEY not set — regression/build still work; agent launch skipped"
  warn=1
fi

if gh auth status >/dev/null 2>&1; then
  if gh secret list 2>/dev/null | grep -q CURSOR_API_KEY; then
    echo "OK: GitHub secret CURSOR_API_KEY present on remote (if repo configured)"
  else
    echo "WARN: GitHub secret CURSOR_API_KEY not listed (add in repo Settings → Secrets)"
    warn=1
  fi
else
  echo "INFO: gh not authenticated — skip remote secret check"
fi

echo "---"
if [[ "$ok" -ne 0 ]]; then
  echo "Setup incomplete (missing tools/files)."
  exit 1
fi
if [[ "$warn" -ne 0 ]]; then
  echo "Setup OK for regression; add CURSOR_API_KEY for full daily automation."
  exit 0
fi
echo "Full daily automation ready."
exit 0
