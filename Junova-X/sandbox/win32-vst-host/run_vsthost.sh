#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")" && pwd)"
export WINEARCH=win32
export WINEPREFIX="${WINEPREFIX:-$ROOT/wineprefix}"
HOST="$ROOT/host/VSTHost.exe"

if [[ ! -f "$HOST" ]]; then
  echo "Missing $HOST" >&2
  echo "Download VSTHost from Hermann Seib (donationware) and copy the 32-bit VSTHost.exe to host/." >&2
  echo "See README.md" >&2
  exit 1
fi

if [[ ! -d "$WINEPREFIX" ]]; then
  echo "Run ./setup.sh first." >&2
  exit 1
fi

PLUGIN="${1:-}"
shift || true

cd "$ROOT"
if [[ -n "$PLUGIN" ]]; then
  exec xvfb-run -a wine "$HOST" "$PLUGIN" "$@"
else
  exec xvfb-run -a wine "$HOST" "$@"
fi
