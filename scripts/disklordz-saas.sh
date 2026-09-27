#!/usr/bin/env bash
# Repo-root wrapper → disklordz/website/scripts/saas.sh
set -euo pipefail
REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
exec bash "$REPO_ROOT/disklordz/website/scripts/saas.sh" "$@"
