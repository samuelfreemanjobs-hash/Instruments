#!/usr/bin/env bash
# Dev server for UltraNova Web Editor (Chrome/Edge + USB MIDI).
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
EDITOR="${ROOT}/hardware-lane/editors/ultranova-web"

cd "${EDITOR}"
if [[ ! -d node_modules ]]; then
  npm ci
fi
echo "UltraNova Web Editor → http://127.0.0.1:5174 (Web MIDI + sysex)"
exec npm run dev -- --host 127.0.0.1
