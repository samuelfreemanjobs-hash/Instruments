#!/usr/bin/env bash
# Release Notes agent — merged PR titles since last tag (needs gh)
set -euo pipefail
BASE="${1:-}"
if ! command -v gh >/dev/null; then
  echo "gh CLI required" >&2
  exit 1
fi
if [[ -z "$BASE" ]]; then
  BASE="$(git describe --tags --abbrev=0 2>/dev/null || echo "")"
fi
RANGE=""
if [[ -n "$BASE" ]]; then
  RANGE="${BASE}..HEAD"
  echo "## Changes since ${BASE}"
else
  echo "## Recent merges"
fi
echo ""
gh pr list --state merged --limit 30 ${RANGE:+--search "merged:>$BASE"} --json title,url --jq '.[] | "- [\(.title)](\(.url))"'
