#!/usr/bin/env bash
# Disklordz bridge: Cursor Cloud / Linux / macOS ↔ Antigravity (Windows) via git handoff files.
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
INBOX="${REPO_ROOT}/disklordz/antigravity/inbox"
OUTBOX="${REPO_ROOT}/disklordz/antigravity/outbox"
SCHEMA="${REPO_ROOT}/disklordz/antigravity/handoff.schema.json"

usage() {
  cat <<'EOF'
Usage: antigravity-bridge.sh <command> [options]

Commands:
  send          Create inbox handoff for Antigravity (cursor_to_antigravity)
  list          List inbox/outbox handoffs
  install-docs  Print Antigravity setup steps (Windows)

send options:
  --wo ID           Work order id (required), e.g. WO-2026-HISE-001
  --title TEXT      WO title (required)
  --criteria TEXT   Acceptance criteria (optional)
  --branch NAME     Git branch Antigravity should use
  --issue NUMBER    GitHub issue to comment (optional, needs gh)
  --path RELPATH    Repo-relative context doc (repeatable)
  --notes TEXT      Freeform notes
  --push            git add, commit, push handoff file

Examples:
  ./scripts/antigravity-bridge/antigravity-bridge.sh send \
    --wo WO-2026-HISE-001 \
    --title "[Plugin][HISE] Lane bootstrap" \
    --branch main \
    --path docs/HISE_ANTIGRAVITY_LANE.md \
    --push
EOF
}

die() { echo "error: $*" >&2; exit 1; }

new_handoff_id() {
  local day rand
  day="$(date -u +"%Y%m%d")"
  rand="$(openssl rand -hex 2 | tr '[:lower:]' '[:upper:]')"
  echo "HO-${day}-${rand}"
}

cmd_send() {
  local wo="" title="" criteria="" branch="main" issue="" notes="" push=0
  local -a paths=("docs/HISE_ANTIGRAVITY_LANE.md" "disklordz/antigravity/AGENT_INSTRUCTIONS.md")

  while [[ $# -gt 0 ]]; do
    case "$1" in
      --wo) wo="$2"; shift 2 ;;
      --title) title="$2"; shift 2 ;;
      --criteria) criteria="$2"; shift 2 ;;
      --branch) branch="$2"; shift 2 ;;
      --issue) issue="$2"; shift 2 ;;
      --path) paths+=("$2"); shift 2 ;;
      --notes) notes="$2"; shift 2 ;;
      --push) push=1; shift ;;
      *) die "unknown send option: $1" ;;
    esac
  done

  [[ -n "$wo" && -n "$title" ]] || die "send requires --wo and --title"

  mkdir -p "$INBOX" "$OUTBOX"
  local id
  id="$(new_handoff_id | tr -d '\n')"
  local file="${INBOX}/${id}.json"

  python3 - "$file" "$id" "$wo" "$title" "$criteria" "$branch" "$notes" "${paths[@]}" <<'PY'
import json, sys
from datetime import datetime, timezone
file, hid, wo, title, criteria, branch, notes = sys.argv[1:8]
paths = sys.argv[8:]
doc = {
  "handoff_id": hid,
  "direction": "cursor_to_antigravity",
  "from_agent": "cursor-cloud",
  "to_agent": "antigravity-hise",
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

  if [[ -n "$issue" ]] && command -v gh >/dev/null 2>&1; then
    gh issue comment "$issue" --body "**Antigravity handoff** \`${id}\`

- WO: \`${wo}\`
- Branch: \`${branch}\`
- Inbox: \`disklordz/antigravity/inbox/${id}.json\`

On Windows, run \`sync-handoff.ps1\` in Antigravity workspace."
  fi

  if [[ "$push" -eq 1 ]]; then
    cd "$REPO_ROOT"
    git add "$file"
    git commit -m "chore(antigravity): handoff ${id} for ${wo}"
    git push -u origin "$(git branch --show-current)"
  fi
}

cmd_list() {
  echo "=== inbox (Cursor → Antigravity) ==="
  ls -1 "$INBOX"/*.json 2>/dev/null || echo "(empty)"
  echo "=== outbox (Antigravity → Cursor) ==="
  ls -1 "$OUTBOX"/*.json 2>/dev/null || echo "(empty)"
}

cmd_install_docs() {
  cat <<'EOF'
Antigravity IDE setup (Windows)
=============================

1. Clone this repo (or pull) to e.g. C:\dev\Instruments
2. In Antigravity: File → Open Folder → that repo root
3. Project instructions: paste disklordz/antigravity/AGENT_INSTRUCTIONS.md
4. Copy disklordz/antigravity/env.windows.example → env.windows.local (set HISE paths)
5. Run: .\scripts\antigravity-bridge\install-antigravity-workspace.ps1
6. After each git pull: .\scripts\antigravity-bridge\sync-handoff.ps1

Cursor Cloud cannot open Antigravity remotely; it drops work in disklordz/antigravity/inbox/.
EOF
}

main() {
  [[ $# -ge 1 ]] || { usage; exit 1; }
  case "$1" in
    send) shift; cmd_send "$@" ;;
    list) cmd_list ;;
    install-docs) cmd_install_docs ;;
    -h|--help|help) usage ;;
    *) die "unknown command: $1 (try --help)" ;;
  esac
}

main "$@"
