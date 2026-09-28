#!/usr/bin/env bash
# Run a command inside the win32 VST sandbox (network off when firejail is available).
set -euo pipefail

ROOT="$(cd "$(dirname "$0")" && pwd)"
REPO="$(cd "$ROOT/../../.." && pwd)"
CMD=("$@")
if [[ ${#CMD[@]} -eq 0 ]]; then
  echo "Usage: $0 <command ...>" >&2
  echo "Example: $0 ./run_vsthost.sh" >&2
  exit 1
fi

export WINEARCH=win32
export WINEPREFIX="${WINEPREFIX:-$ROOT/wineprefix}"
export DISPLAY="${DISPLAY:-:99}"

if command -v firejail >/dev/null 2>&1; then
  exec firejail \
    --net=none \
    --private-tmp \
    --whitelist="$ROOT" \
    --whitelist="$REPO/build" \
    --whitelist="$REPO/Junova-X/scripts" \
    --whitelist="$REPO/tests/golden" \
    "${CMD[@]}"
else
  echo "firejail not installed; running without sandbox (install firejail for --net=none)." >&2
  exec "${CMD[@]}"
fi
