#!/usr/bin/env bash
# Run Alapatch microKORG web editor locally (Create React App / craco).
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
APP="${REPO_ROOT}/hardware-lane/editors/alapatch"

if [[ ! -f "${APP}/package.json" ]]; then
  echo "Init submodule: git submodule update --init hardware-lane/editors/alapatch" >&2
  exit 1
fi

cd "${APP}"
if [[ ! -d node_modules ]]; then
  npm ci
fi

echo "Starting Alapatch at http://localhost:3000 (Ctrl+C to stop)"
exec npm start
