#!/usr/bin/env bash
# Emit next Antigravity inbox handoff from active-queue.json (active mode).
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
QUEUE="${REPO_ROOT}/disklordz/antigravity/active-queue.json"
BRIDGE="${REPO_ROOT}/scripts/antigravity-bridge/antigravity-bridge.sh"

usage() {
  cat <<'EOF'
Usage: dispatch-active.sh [--dry-run] [--push] [--wo WO-AG-001]

Picks highest-priority pending item from active-queue.json, writes inbox HO-*.json.
Use --push to commit and push (triggers antigravity-inbox-slack.yml on main).
EOF
}

dry_run=0
push=0
force_wo=""

while [[ $# -gt 0 ]]; do
  case "$1" in
    --dry-run) dry_run=1; shift ;;
    --push) push=1; shift ;;
    --wo) force_wo="$2"; shift 2 ;;
    -h|--help) usage; exit 0 ;;
    *) echo "unknown: $1" >&2; exit 1 ;;
  esac
done

[[ -f "$QUEUE" ]] || { echo "missing $QUEUE" >&2; exit 1; }

eval "$(python3 - "$QUEUE" "$REPO_ROOT" "$force_wo" <<'PY'
import json, sys
from pathlib import Path
q = json.loads(Path(sys.argv[1]).read_text())
root = Path(sys.argv[2])
force = sys.argv[3]
done = set(q.get("completed_wo_ids") or [])
items = sorted(q.get("items") or [], key=lambda x: x.get("priority", 99))
pick = None
if force:
    for it in items:
        if it.get("wo_id") == force:
            pick = it
            break
else:
    open_inbox = list((root / "disklordz/antigravity/inbox").glob("*.json"))
    open_wos = set()
    for p in open_inbox:
        try:
            d = json.loads(p.read_text())
            open_wos.add(d.get("work_order", {}).get("id"))
        except Exception:
            pass
    for it in items:
        wid = it.get("wo_id")
        if wid in done:
            continue
        if wid in open_wos:
            continue
        pick = it
        break
if not pick:
    print('PICK=')
    sys.exit(0)
lane = pick.get("lane", "batch")
to_agent = {
    "hise": "antigravity-hise",
    "batch": "antigravity-batch",
    "docs": "antigravity-docs",
    "prep": "antigravity-prep",
    "assets": "antigravity-assets",
}.get(lane, "antigravity-batch")
# shell-safe exports
import shlex
print(f"WO_ID={shlex.quote(pick['wo_id'])}")
print(f"TITLE={shlex.quote(pick['title'])}")
print(f"CRITERIA={shlex.quote(pick.get('criteria',''))}")
print(f"LANE={shlex.quote(lane)}")
print(f"TO_AGENT={shlex.quote(to_agent)}")
paths = pick.get("context_paths") or []
for i, p in enumerate(paths):
    print(f"PATH_{i}={shlex.quote(p)}")
print(f"PATH_COUNT={len(paths)}")
PY
)"

if [[ -z "${WO_ID:-}" ]]; then
  if [[ -n "$force_wo" ]]; then
    echo "WO $force_wo not found or already complete" >&2
    exit 1
  fi
  echo "No pending Antigravity queue item (all done or inbox already has open WO)."
  exit 0
fi

args=(send --wo "$WO_ID" --title "$TITLE" --criteria "$CRITERIA" --to-agent "$TO_AGENT" --notes "Lane: ${LANE} → ${TO_AGENT}. Active queue dispatch.")
for ((i=0; i<PATH_COUNT; i++)); do
  var="PATH_${i}"
  args+=(--path "${!var}")
done
[[ "$push" -eq 1 ]] && args+=(--push)

if [[ "$dry_run" -eq 1 ]]; then
  echo "Would run: antigravity-bridge.sh ${args[*]}"
  exit 0
fi

"$BRIDGE" "${args[@]}"

if [[ "$push" -eq 1 ]]; then
  python3 - "$QUEUE" "$WO_ID" <<'PY'
import json, sys
from pathlib import Path
path, wo = Path(sys.argv[1]), sys.argv[2]
q = json.loads(path.read_text())
done = list(q.get("completed_wo_ids") or [])
if wo not in done:
    done.append(wo)
q["completed_wo_ids"] = done
path.write_text(json.dumps(q, indent=2) + "\n", encoding="utf-8")
PY
  cd "$REPO_ROOT"
  git add "$QUEUE"
  if ! git diff --quiet --cached; then
    git commit -m "chore(antigravity): queue dispatched ${WO_ID}"
    git push -u origin "$(git branch --show-current)"
  fi
fi

echo "Dispatched ${WO_ID} for Antigravity. Windows: sync-handoff.ps1 or watch-handoffs.ps1"
