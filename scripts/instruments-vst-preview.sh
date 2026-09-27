#!/usr/bin/env bash
# WO-PLUGIN-001: build plugins and write listenable preview WAVs for VS Code.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
exec python3 "$ROOT/vst-testing-ops/preview_agent.py" "$@"
