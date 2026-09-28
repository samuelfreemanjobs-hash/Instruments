#!/usr/bin/env bash
# Disklordz bridge: PM / Workflow Automation → Cursor VST Plugin Factory via git handoff files.
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
INBOX="${REPO_ROOT}/disklordz/vst-factory/inbox"
OUTBOX="${REPO_ROOT}/disklordz/vst-factory/outbox"

usage() {
  cat <<'EOF'
Usage: vst-factory-bridge.sh <command> [options]

Commands:
  send          Create inbox handoff for VST Plugin Factory (cursor_to_vst_plugin_factory)
  list          List inbox/outbox handoffs

send options:
  --wo ID           Work order id (required)
  --title TEXT      WO title (required)
  --criteria TEXT   Acceptance criteria (optional)
  --branch NAME     Git branch (default main)
  --path RELPATH    Repo-relative context doc (repeatable)
  --notes TEXT      Freeform notes
  --push            git add, commit, push handoff file

Example:
  ./scripts/vst-factory-bridge/vst-factory-bridge.sh send \
    --wo WO-2026-JUCE-001 \
    --title "[Plugin][JUCE] Add bus compressor MVP" \
    --branch cursor/bus-comp-mvp-ddc8 \
    --path docs/VST_PLUGIN_FACTORY_AGENT.md \
    --push
EOF
}

die() { echo "error: $*" >&2; exit 1; }

new_handoff_id() {
  local day rand
  day="$(date -u +"%Y%m%d")"
  rand="$(openssl rand -hex 2 | tr '[:upper:]' '[:lower:]')"
  echo "HO-${day}-${rand}"
}

cmd_send() {
  local wo="" title="" criteria="" branch="main" notes="" push=0
  local -a paths=("docs/VST_PLUGIN_FACTORY_AGENT.md" "AGENTS.md" "ARCHITECTURE.md")

  while [[ $# -gt 0 ]]; do
    case "$1" in
      --wo) wo="$2"; shift 2 ;;
      --title) title="$2"; shift 2 ;;
      --criteria) criteria="$2"; shift 2 ;;
      --branch) branch="$2"; shift 2 ;;
      --path) paths+=("$2"); shift 2 ;;
      --notes) notes="$2"; shift 2 ;;
      --push) push=1; shift ;;
      *) die "unknown send option: $1" ;;
    esac
  done

  [[ -n "$wo" && -n "$title" ]] || die "send requires --wo and --title"

  mkdir -p "$INBOX" "$OUTBOX"
  local id file
  id="$(new_handoff_id | tr -d '\n')"
  file="${INBOX}/${id}.json"

  python3 - "$file" "$id" "$wo" "$title" "$criteria" "$branch" "$notes" "${paths[@]}" <<'PY'
import json, sys
from datetime import datetime, timezone
file, hid, wo, title, criteria, branch, notes = sys.argv[1:8]
paths = sys.argv[8:]
doc = {
  "handoff_id": hid,
  "direction": "pm_to_vst_plugin_factory",
  "from_agent": "pm-agent",
  "to_agent": "cursor-vst-plugin-factory",
  "created_at": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
  "work_order": {
    "id": wo,
    "title": title,
    "acceptance_criteria": criteria,
    "branch": branch,
  },
  "context_paths": paths,
  "artifacts": [],
  "status": "open",
  "notes": notes,
}
with open(file, "w", encoding="utf-8") as f:
    json.dump(doc, f, indent=2)
    f.write("\n")
PY

  echo "Wrote ${file}"

  if [[ "$push" -eq 1 ]]; then
    git -C "$REPO_ROOT" add "$file"
    git -C "$REPO_ROOT" commit -m "chore(vst-factory): handoff ${id} for ${wo}"
    git -C "$REPO_ROOT" push
  fi
}

cmd_list() {
  echo "=== inbox ==="
  ls -1 "$INBOX"/HO-*.json 2>/dev/null || echo "(empty)"
  echo "=== outbox ==="
  ls -1 "$OUTBOX"/HO-*.json 2>/dev/null || echo "(empty)"
}

main() {
  [[ $# -ge 1 ]] || { usage; exit 1; }
  case "$1" in
    send) shift; cmd_send "$@" ;;
    list) cmd_list ;;
    -h|--help|help) usage ;;
    *) die "unknown command: $1" ;;
  esac
}

main "$@"
