#!/usr/bin/env bash
# Install entropyvortex/meta-llm-charter into this monorepo (core + optional skills).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
CHARTER_DIR="$ROOT/disklordz/agents/charter"
BASE="https://raw.githubusercontent.com/entropyvortex/meta-llm-charter/main"

mkdir -p "$CHARTER_DIR/vendor-skills"
curl -fsSL "$BASE/CLAUDE.md" -o "$ROOT/CLAUDE.md"
cp "$ROOT/CLAUDE.md" "$CHARTER_DIR/META_CHARTER.md"

for skill in zero-pause weave premortem; do
  mkdir -p "$CHARTER_DIR/vendor-skills/$skill"
  curl -fsSL "$BASE/.claude/skills/$skill/SKILL.md" \
    -o "$CHARTER_DIR/vendor-skills/$skill/SKILL.md"
done

mkdir -p "$CHARTER_DIR/vendor-skills/weave/hooks"
curl -fsSL "$BASE/.claude/skills/weave/hooks/scope-guard.sh" \
  -o "$CHARTER_DIR/vendor-skills/weave/hooks/scope-guard.sh" || true

# Claude Code install paths
mkdir -p "$ROOT/.claude/skills"
for skill in zero-pause weave premortem; do
  dest="$ROOT/.claude/skills/$skill"
  mkdir -p "$dest"
  cp "$CHARTER_DIR/vendor-skills/$skill/SKILL.md" "$dest/SKILL.md"
done
if [ -f "$CHARTER_DIR/vendor-skills/weave/hooks/scope-guard.sh" ]; then
  mkdir -p "$ROOT/.claude/skills/weave/hooks"
  cp "$CHARTER_DIR/vendor-skills/weave/hooks/scope-guard.sh" \
    "$ROOT/.claude/skills/weave/hooks/scope-guard.sh"
fi

BYTES=$(wc -c < "$ROOT/CLAUDE.md" | tr -d ' ')
if [ "$BYTES" -gt 2400 ]; then
  echo "ERROR: CLAUDE.md core exceeds 2400 bytes ($BYTES)" >&2
  exit 1
fi
SHA=$(curl -s "https://api.github.com/repos/entropyvortex/meta-llm-charter/commits/main" \
  | python3 -c "import sys,json; print(json.load(sys.stdin).get('sha','unknown')[:12])")
python3 - <<PY
import json
from pathlib import Path
p = Path("$CHARTER_DIR/UPSTREAM.json")
data = json.loads(p.read_text())
data["pinned_commit"] = "$SHA"
data["core_bytes"] = int("$BYTES")
p.write_text(json.dumps(data, indent=2) + "\n")
PY

echo "Synced meta-llm-charter @ $SHA (core ${BYTES} bytes)"
echo "Claude Code: CLAUDE.md + .claude/skills/{zero-pause,weave,premortem}"
