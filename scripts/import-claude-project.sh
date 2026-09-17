#!/usr/bin/env bash
# Import a Claude Code workspace into ./<name> using claude-projects MCP logic.
set -euo pipefail
QUERY="${1:?usage: import-claude-project.sh <search-query> [dest-dir]}"
DEST="${2:-./${QUERY}}"
ROOT="${CLAUDE_PROJECTS_ROOT:-$HOME/.claude/projects}"
MCP_DIR="$(cd "$(dirname "$0")/../disklordz-factory/integrations/claude_projects_mcp" && pwd)"
PY="${MCP_DIR}/.venv/bin/python3"
[[ -x "$PY" ]] || "$MCP_DIR/setup.sh"

export CLAUDE_PROJECTS_ROOT="$ROOT"
mapfile -t HITS < <("$PY" -c "
import sys, json
sys.path.insert(0, '$MCP_DIR')
import server
hits = server.search_claude_projects('$QUERY', limit=5)
print(hits[0]['project_id'] if hits else '')
")

PROJECT_ID="${HITS[0]:-}"
if [[ -z "$PROJECT_ID" ]]; then
  echo "No Claude project match for '$QUERY' under $ROOT" >&2
  exit 1
fi

SRC="$ROOT/$PROJECT_ID"
echo "Importing $SRC -> $DEST"
rm -rf "$DEST"
mkdir -p "$DEST"
rsync -a --exclude '.git' "$SRC/" "$DEST/"
echo "Done. Files:"
find "$DEST" -type f | wc -l
