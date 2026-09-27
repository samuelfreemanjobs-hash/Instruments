#!/usr/bin/env bash
# Smoke-check a deployed (or local) Disklordz site. See docs/DISKLORDZ_GO_LIVE.md
set -euo pipefail

REQUIRE_ACCOUNTS=false
REQUIRE_BILLING=false
while [ $# -gt 0 ]; do
  case "$1" in
    --require-accounts) REQUIRE_ACCOUNTS=true ;;
    --require-billing) REQUIRE_BILLING=true ;;
    -h|--help)
      echo "Usage: verify-go-live.sh [--require-accounts] [--require-billing]"
      exit 0
      ;;
    *) echo "Unknown: $1"; exit 1 ;;
  esac
  shift
done

BASE_URL="${DISKLORDZ_URL:-http://127.0.0.1:3000}"
BASE_URL="${BASE_URL%/}"
PRESET="${DISKLORDZ_GO_LIVE_PRESET:-boulevard-86}"

echo "Disklordz go-live verify → ${BASE_URL}"

health_body=$(curl -s "${BASE_URL}/api/health")
if ! echo "$health_body" | python3 -c "import sys,json; d=json.load(sys.stdin); assert d.get('status') in ('ok','degraded'); assert d.get('features',{}).get('guestGenerate')"; then
  echo "FAIL: GET /api/health"
  echo "$health_body" | head -c 400
  exit 1
fi
echo "OK: GET /api/health"

if $REQUIRE_ACCOUNTS; then
  if ! echo "$health_body" | python3 -c "import sys,json; d=json.load(sys.stdin); assert d.get('features',{}).get('accountsReady')"; then
    echo "FAIL: accountsReady (set Supabase env on server)"
    exit 1
  fi
  echo "OK: accountsReady"
fi

if $REQUIRE_BILLING; then
  if ! echo "$health_body" | python3 -c "import sys,json; d=json.load(sys.stdin); assert d.get('features',{}).get('billingReady')"; then
    echo "FAIL: billingReady (set Stripe env + webhook)"
    exit 1
  fi
  echo "OK: billingReady"
fi

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

download_payload=$(echo "$gen_body" | python3 -c "import sys,json; d=json.load(sys.stdin); print(json.dumps({'manifest': d['variations'][0]['manifest']}))")
zip_code=$(curl -s -o /tmp/disklordz-smoke.zip -w "%{http_code}" -X POST "${BASE_URL}/api/download" \
  -H "Content-Type: application/json" \
  -d "$download_payload")

if [[ "$zip_code" != "200" ]]; then
  echo "FAIL: /api/download ${zip_code}"
  exit 1
fi
if ! python3 -c "open('/tmp/disklordz-smoke.zip','rb').read(2)==b'PK'"; then
  echo "FAIL: download is not a ZIP"
  exit 1
fi
echo "OK: /api/download ZIP"

echo "All automated checks passed."
