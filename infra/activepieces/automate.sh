#!/usr/bin/env bash
# Start Activepieces and provision the Disklordz factory webhook flow (no UI clicks).
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$ROOT/../.." && pwd)"

"$ROOT/bootstrap.sh"

set -a
# shellcheck disable=SC1091
source "$ROOT/.env"
set +a

python3 "$REPO/scripts/provision_disklordz_activepieces.py"

if [[ -f "$ROOT/.disklordz-webhook-url" ]]; then
  echo ""
  echo "Test ping:"
  curl -fsS -X POST "$(cat "$ROOT/.disklordz-webhook-url")" \
    -H "Content-Type: application/json" \
    -d '{"kit_name":"AUTOMATE_TEST","count":0,"batch_id":"bootstrap","source":"automate.sh"}' \
    && echo "Webhook OK"
fi
