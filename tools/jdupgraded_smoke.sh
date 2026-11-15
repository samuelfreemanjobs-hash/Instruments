#!/usr/bin/env bash
set -euo pipefail
OFFLINE_RENDER="${1:?Usage: jdupgraded_smoke.sh <OfflineRender>}"
"$OFFLINE_RENDER" --smoke-lazy-check
"$OFFLINE_RENDER" --smoke-check
echo "PASS jdupgraded_smoke"