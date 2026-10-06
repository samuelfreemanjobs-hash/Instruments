#!/usr/bin/env bash
set -euo pipefail

BASE="${DISKLORDZ_URL:-http://127.0.0.1:3000}"
BASE="${BASE%/}"

echo "Integration verify → ${BASE}/api/integrations/status"

body=$(curl -s "${BASE}/api/integrations/status")
echo "$body" | python3 -c "
import json, sys
d = json.load(sys.stdin)
assert d.get('repoCount') == 35, d
r = d.get('runtime', {})
print('repoCount:', d['repoCount'])
print('integrated:', d.get('integrated'))
print('runtime:', json.dumps(r, indent=2))
"

echo "OK: integrations status"

rag=$(curl -s -X POST "${BASE}/api/rag/suggest" \
  -H "Content-Type: application/json" \
  -d '{"mode":"random","presetId":"boulevard-86"}')
echo "$rag" | python3 -c "import json,sys; d=json.load(sys.stdin); assert d.get('prompt'); print('OK: rag/suggest', d.get('retrieval'))"

if [ -n "${INNGEST_EVENT_KEY:-}" ] || [ -n "${INNGEST_DEV:-}" ]; then
  async=$(curl -s -X POST "${BASE}/api/generate/async" \
    -H "Content-Type: application/json" \
    -d '{"prompt":"async integration smoke","presetId":"boulevard-86"}')
  echo "$async" | python3 -c "
import json,sys
d=json.load(sys.stdin)
if d.get('status')=='queued':
  print('OK: generate/async queued', d.get('batchId'))
elif d.get('error')=='inngest_not_configured':
  print('SKIP async: inngest_not_configured on server')
else:
  raise SystemExit('unexpected async response: '+str(d))
"
else
  echo "SKIP async generate (no INNGEST_EVENT_KEY in shell env)"
fi

echo "All integration checks done."
