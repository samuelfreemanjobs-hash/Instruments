#!/usr/bin/env bash
# Local entrypoint — same lane as .github/workflows/audio-plugin-fleet.yml
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
args=()
[[ "${1:-}" == "--weekly" ]] && args+=(--weekly)
[[ "${1:-}" == "--push" ]] && args+=(--antigravity --push)
python3 scripts/audio-plugin/fleet_tick.py "${args[@]}"
