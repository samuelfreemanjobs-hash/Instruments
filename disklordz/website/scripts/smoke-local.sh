#!/usr/bin/env bash
# Production build + start + verify-go-live (mirrors CI smoke step).
set -euo pipefail
# shellcheck disable=SC1091
source "$(dirname "$0")/lib/common.sh"
cd_website

PORT="${DISKLORDZ_SMOKE_PORT:-3099}"
export DISKLORDZ_URL="http://127.0.0.1:${PORT}"

log "Building Next.js app"
npm run build

log "Starting server on port $PORT"
npm run start -- -p "$PORT" &
pid=$!
cleanup() { kill "$pid" 2>/dev/null || true; }
trap cleanup EXIT

for i in $(seq 1 45); do
  if curl -sf "${DISKLORDZ_URL}/api/health" >/dev/null; then
    break
  fi
  sleep 1
done

curl -sf "${DISKLORDZ_URL}/api/health" >/dev/null || die "Server did not become ready"

bash scripts/verify-go-live.sh "$@"
log "Local smoke passed."
