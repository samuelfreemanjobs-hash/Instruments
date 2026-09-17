#!/usr/bin/env bash
set -euo pipefail
FERRETDB_BIN="${FERRETDB_BIN:-/tmp/ferretdb}"
LOG="${FERRETDB_LOG:-/tmp/ferretdb.log}"
if [[ ! -x "$FERRETDB_BIN" ]]; then
  echo "Missing FerretDB binary. Run: sudo ./scripts/setup-documentdb-ferretdb.sh"
  exit 1
fi
if pgrep -f "$FERRETDB_BIN" >/dev/null; then
  echo "FerretDB already running"
  exit 0
fi
nohup "$FERRETDB_BIN" --postgresql-url=postgres://ferret:ferret@127.0.0.1:5432/postgres >>"$LOG" 2>&1 &
sleep 2
echo "FerretDB started (log: $LOG)"
