#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
if [[ -d venv ]]; then
  # shellcheck source=/dev/null
  source venv/bin/activate
fi
exec python3 -m phonk_drum_studio "$@"
