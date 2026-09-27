#!/usr/bin/env bash
# Repo-root wrapper for WO-SAAS-018 daily Factory pipeline.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
exec bash "$ROOT/disklordz/automation/scripts/run-factory-daily.sh" "$@"
