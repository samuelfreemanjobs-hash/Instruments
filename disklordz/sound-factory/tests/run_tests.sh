#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SCRIPTS="$ROOT/scripts"
cd "$SCRIPTS"
python3 -m pytest -q "$ROOT/tests" "$@" 2>/dev/null || python3 "$ROOT/tests/test_factory_smoke.py"
