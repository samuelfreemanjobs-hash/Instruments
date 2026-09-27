#!/usr/bin/env bash
# WO-SAAS-024/027: manifest license strings present
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
MANIFEST="$ROOT/disklordz/website/src/lib/manifest.ts"
FACTORY="$ROOT/disklordz/website/src/lib/generation/factory.ts"
grep -q "personal_and_commercial_v0_preview" "$FACTORY" || {
  echo "FAIL: factory license string missing"
  exit 1
}
grep -q "DISKLORDZ_DRUM_KIT_MANIFEST" "$MANIFEST" || exit 1
echo "OK: trust/licensing strings present"
