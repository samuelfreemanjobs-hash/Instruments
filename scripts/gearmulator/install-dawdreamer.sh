#!/usr/bin/env bash
set -euo pipefail
REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
pip install -r "${REPO_ROOT}/gearmulator-lane/requirements-dawdreamer.txt"
