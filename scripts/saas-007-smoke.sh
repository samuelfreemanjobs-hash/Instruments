#!/usr/bin/env bash
# SaaS 007+ smoke: variations, async path, credits field (no prod secrets required for base checks)
set -euo pipefail
BASE="${DISKLORDZ_URL:-http://127.0.0.1:3000}"
BASE="${BASE%/}"
PRESET="${DISKLORDZ_GO_LIVE_PRESET:-boulevard-86}"

echo "SaaS 007 smoke → $BASE"

studio=$(curl -s -X POST "$BASE/api/generate" \
  -H "Content-Type: application/json" \
  -d "{\"prompt\":\"007 studio variations\",\"presetId\":\"$PRESET\",\"spec\":{\"engine\":\"studio\",\"mode\":\"one_shot\"}}")
studio_n=$(echo "$studio" | python3 -c "import sys,json; d=json.load(sys.stdin); print(len(d.get('variations',[])))")
[[ "$studio_n" == "2" ]] || { echo "FAIL studio expected 2 variations got $studio_n"; exit 1; }
echo "OK studio variations=$studio_n"

creative=$(curl -s -X POST "$BASE/api/generate" \
  -H "Content-Type: application/json" \
  -d "{\"prompt\":\"007 creative variations\",\"presetId\":\"$PRESET\",\"spec\":{\"engine\":\"creative\",\"mode\":\"one_shot\"}}")
creative_n=$(echo "$creative" | python3 -c "import sys,json; d=json.load(sys.stdin); print(len(d.get('variations',[])))")
[[ "$creative_n" == "3" ]] || { echo "FAIL creative expected 3 variations got $creative_n"; exit 1; }
echo "OK creative variations=$creative_n"

async=$(curl -s -X POST "$BASE/api/generate/async" \
  -H "Content-Type: application/json" \
  -d "{\"prompt\":\"007 async smoke\",\"presetId\":\"$PRESET\"}")
echo "$async" | python3 -c "
import json,sys
d=json.load(sys.stdin)
if d.get('status')=='queued':
  print('OK async queued', d.get('batchId'))
elif d.get('error')=='inngest_not_configured':
  print('SKIP async: inngest_not_configured (set INNGEST_* on server)')
else:
  raise SystemExit('unexpected async: '+str(d))
"

python3 disklordz/rag/scripts/embed_and_upsert.py --dry-run 2>/dev/null || \
  python3 disklordz/rag/scripts/embed_and_upsert.py --dry-run

echo "SaaS 007 smoke complete"
