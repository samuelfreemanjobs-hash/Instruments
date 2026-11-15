#!/usr/bin/env bash
# Claude Code asks Antigravity (Windows) for DPF/build/test support.
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
INBOX="${REPO_ROOT}/disklordz/antigravity/claude-code-inbox"
BRIDGE="${REPO_ROOT}/scripts/antigravity-bridge/antigravity-bridge.sh"
SYNC="${REPO_ROOT}/scripts/antigravity-bridge/sync-claude-code-support.py"

usage() {
  cat <<'EOF'
Usage: request-claude-support.sh --slice TEXT --kind KIND [options]

Kinds: windows_build | pytest_run | codegen_stub | midi_fixture | cmake_scaffold

Options:
  --dpf AREA        DPF-Core | DPF-MIDI | DPF-DSP | Build | Tests
  --path RELPATH    Context doc (repeatable)
  --notes TEXT
  --dispatch        Immediately create HO- handoff via sync-claude-code-support.py
  --push            git add, commit, push request (+ dispatch if --dispatch)
EOF
}

slice="" kind="" dpf="DPF-MIDI" notes="" dispatch=0 push=0
declare -a paths=()

while [[ $# -gt 0 ]]; do
  case "$1" in
    --slice) slice="$2"; shift 2 ;;
    --kind) kind="$2"; shift 2 ;;
    --dpf) dpf="$2"; shift 2 ;;
    --path) paths+=("$2"); shift 2 ;;
    --notes) notes="$2"; shift 2 ;;
    --dispatch) dispatch=1; shift ;;
    --push) push=1; shift ;;
    -h|--help) usage; exit 0 ;;
    *) echo "unknown: $1" >&2; exit 1 ;;
  esac
done

[[ -n "$slice" && -n "$kind" ]] || { usage; exit 1; }

mkdir -p "$INBOX"
id="CC-$(date -u +%Y%m%d)-$(openssl rand -hex 2 | tr '[:lower:]' '[:upper:]')"
file="${INBOX}/${id}.json"

python3 - "$file" "$id" "$slice" "$kind" "$dpf" "$notes" "${paths[@]}" <<'PY'
import json, sys
from datetime import datetime, timezone
f, cid, slice_, kind, dpf, notes = sys.argv[1:7]
paths = sys.argv[7:]
doc = {
    "request_id": cid,
    "from_agent": "claude-code",
    "to_agent": "antigravity-dpf-support",
    "created_at": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
    "slice": slice_,
    "support_kind": kind,
    "dpf_area": dpf,
    "context_paths": paths or [
        "docs/DPF_TECHNICAL_ARCHITECTURE.md",
        "docs/ANTIGRAVITY_CLAUDE_CODE_COPILOT.md",
    ],
    "status": "open",
    "notes": notes,
}
with open(f, "w", encoding="utf-8") as out:
    json.dump(doc, out, indent=2)
    out.write("\n")
PY

echo "Wrote ${file}"

if [[ "$dispatch" -eq 1 ]]; then
  python3 "$SYNC" --dispatch ${push:+--push}
fi

if [[ "$push" -eq 1 ]]; then
  cd "$REPO_ROOT"
  git add "$file"
  git commit -m "chore(claude-code): Antigravity support request ${id}"
  git push -u origin "$(git branch --show-current)"
fi
