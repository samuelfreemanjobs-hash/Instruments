#!/usr/bin/env bash
# Copy DiskLordz OpenClaw agent identities (SOUL.md / AGENTS.md) into Gateway state dir.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SRC="$ROOT/openclaw"
DEST="${OPENCLAW_STATE_DIR:-$HOME/.openclaw}"

if [[ ! -d "$SRC/agents" ]]; then
  echo "missing $SRC/agents" >&2
  exit 1
fi

mkdir -p "$DEST/agents"
if [[ -f "$SRC/AGENTS.md" ]]; then
  cp "$SRC/AGENTS.md" "$DEST/AGENTS.md.disklordz"
  echo "wrote $DEST/AGENTS.md.disklordz (merge into your workspace AGENTS.md if needed)"
fi

count=0
for agent_dir in "$SRC/agents"/*; do
  [[ -d "$agent_dir" ]] || continue
  name="$(basename "$agent_dir")"
  mkdir -p "$DEST/agents/$name"
  for f in SOUL.md AGENTS.md; do
    if [[ -f "$agent_dir/$f" ]]; then
      cp "$agent_dir/$f" "$DEST/agents/$name/$f"
    fi
  done
  count=$((count + 1))
  echo "installed $name -> $DEST/agents/$name/"
done

echo "Done. $count agents. Restart OpenClaw Gateway and run: openclaw workboard dispatch"
echo "Docs: docs/OPENCLAW_VINTAGE_COLLECTIVE.md"
