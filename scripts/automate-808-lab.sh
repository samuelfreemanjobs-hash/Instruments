#!/usr/bin/env bash
# Run full 808 Lab repo automation from repository root.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT/disklordz/website"
exec npm run automate:808-lab-repo -- "$@"
