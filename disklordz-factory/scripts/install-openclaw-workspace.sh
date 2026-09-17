#!/usr/bin/env bash
# Copy DiskLordz OpenClaw workspaces + agent identities into Gateway state dir.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SRC="$ROOT/openclaw"
DEST="${OPENCLAW_STATE_DIR:-$HOME/.openclaw}"

if [[ ! -d "$SRC/agents" ]]; then
  echo "missing $SRC/agents" >&2
  exit 1
fi

mkdir -p "$DEST/agents" "$DEST/workspaces"
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
  echo "installed agent $name -> $DEST/agents/$name/"
done

ws=0
if [[ -d "$SRC/workspaces" ]]; then
  for ws_dir in "$SRC/workspaces"/*; do
    [[ -d "$ws_dir" ]] || continue
    name="$(basename "$ws_dir")"
    mkdir -p "$DEST/workspaces/$name"
    for f in "$ws_dir"/*; do
      [[ -f "$f" ]] || continue
      cp "$f" "$DEST/workspaces/$name/$(basename "$f")"
    done
    ws=$((ws + 1))
    echo "installed workspace $name -> $DEST/workspaces/$name/"
  done
fi

if [[ -f "$SRC/agents.config.disklordz.json" ]]; then
  cp "$SRC/agents.config.disklordz.json" "$DEST/agents.config.disklordz.json"
  echo "wrote $DEST/agents.config.disklordz.json"
  echo "  -> merge agents.entries into your openclaw.json (see openclaw/MERGE.md)"
fi
if [[ -f "$SRC/routing.config.disklordz.json" ]]; then
  cp "$SRC/routing.config.disklordz.json" "$DEST/routing.config.disklordz.json"
  echo "wrote $DEST/routing.config.disklordz.json (spawn protocol reference)"
fi

echo "Done. $count agent dirs, $ws workspaces."
echo "Restart OpenClaw Gateway; set default agent disklordz_maestro; run: openclaw workboard dispatch"
echo "Docs: docs/OPENCLAW_VINTAGE_COLLECTIVE.md"
