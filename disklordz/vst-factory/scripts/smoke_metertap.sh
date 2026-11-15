#!/usr/bin/env bash
# Thin wrapper for Git Bash / Linux — runs complete_metertap.py
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
export VST_FACTORY_URL="${VST_FACTORY_URL:-http://127.0.0.1:8790}"
exec python3 "$ROOT/scripts/complete_metertap.py" "$@"
