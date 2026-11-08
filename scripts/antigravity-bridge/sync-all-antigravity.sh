#!/usr/bin/env bash
set -euo pipefail
REPO="$(cd "$(dirname "$0")/../.." && pwd)"
args=(--merge-only --dispatch)
[[ "${1:-}" == "--push" ]] && args+=(--push)
[[ "${1:-}" == "--dry-run" ]] && args=(--merge-only --dispatch --dry-run)

python3 "$REPO/scripts/antigravity-bridge/sync-product-prep.py" "${args[@]}"
python3 "$REPO/scripts/antigravity-bridge/sync-claude-code-support.py" "${args[@]}"
