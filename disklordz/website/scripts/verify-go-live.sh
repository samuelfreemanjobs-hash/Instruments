#!/usr/bin/env bash
# Smoke-check a deployed (or local) Disklordz site. See docs/DISKLORDZ_GO_LIVE.md
set -euo pipefail

BASE_URL="${DISKLORDZ_URL:-http://127.0.0.1:3000}"
BASE_URL="${BASE_URL%/}"
PRESET="${DISKLORDZ_GO_LIVE_PRESET:-boulevard-86}"

echo "Disklordz go-live verify → ${BASE_URL}"

code_home=$(curl -s -o /dev/null -w "%{http_code}" "${BASE_URL}/")
if [[ "$code_home" != "200" ]]; then
  echo "FAIL: GET / returned ${code_home}"
  exit 1
fi
echo "OK: GET / ${code_home}"

gen_body=$(curl -s -X POST "${BASE_URL}/api/generate" \
  -H "Content-Type: application/json" \
  -d "{\"prompt\":\"go-live smoke kit\",\"presetId\":\"${PRESET}\",\"spec\":{\"mode\":\"one_shot\",\"engine\":\"studio\"}}")

if ! echo "$gen_body" | python3 -c "import sys,json; d=json.load(sys.stdin); assert d.get('variations')"; then
  echo "FAIL: /api/generate one_shot"
  echo "$gen_body" | head -c 400
  exit 1
fi
echo "OK: /api/generate one_shot"

loop_body=$(curl -s -X POST "${BASE_URL}/api/generate" \
  -H "Content-Type: application/json" \
  -d "{\"prompt\":\"go-live loop\",\"presetId\":\"${PRESET}\",\"spec\":{\"mode\":\"loop\",\"engine\":\"creative\",\"bpm\":90,\"bars\":4}}")

sample_url=$(echo "$loop_body" | python3 -c "import sys,json; d=json.load(sys.stdin); m=d['variations'][0]['manifest']; assert any(s['name']=='loop_main' for s in m['samples']); print(m['samples'][0]['url'])")
sample_code=$(curl -s -o /dev/null -w "%{http_code}" "$sample_url")
if [[ "$sample_code" != "200" ]]; then
  echo "FAIL: sample preview ${sample_code} ${sample_url}"
  exit 1
fi
echo "OK: loop_main preview ${sample_code}"

pack_body=$(curl -s -X POST "${BASE_URL}/api/factory/batch" \
  -H "Content-Type: application/json" \
  -d "{\"prompt\":\"go-live pack\",\"presetId\":\"${PRESET}\",\"spec\":{\"engine\":\"studio\"}}")

if ! echo "$pack_body" | python3 -c "import sys,json; d=json.load(sys.stdin); p=d['productPack']; assert p['format']=='DISKLORDZ_PRODUCT_PACK_MANIFEST'"; then
  echo "FAIL: /api/factory/batch"
  echo "$pack_body" | head -c 400
  exit 1
fi
echo "OK: /api/factory/batch"

echo "All automated checks passed."
