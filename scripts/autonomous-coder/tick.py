#!/usr/bin/env python3
"""24/7 autonomous coding queue — deterministic tick + Cloud slice emitter."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
QUEUE = REPO / "disklordz/autonomous-coder/coding-queue.json"
STATE = REPO / "disklordz/autonomous-coder/state.json"
PROGRESS = REPO / "disklordz/audio-command/data/progress.json"
SLICE_OUT = REPO / ".cursor/autonomous-coder/NEXT_SLICE.md"
ESCALATION_DIR = REPO / "disklordz/autonomous-coder/factory-escalation"


def load(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def save_state(state: dict) -> None:
    STATE.write_text(json.dumps(state, indent=2) + "\n", encoding="utf-8")


def gate_open(item: dict) -> bool:
    gate = item.get("gate_completed_task")
    if not gate:
        return True
    if not PROGRESS.is_file():
        return False
    completed = set(load(PROGRESS).get("completed_ids") or [])
    return gate in completed


def due_recurring(item: dict, state: dict) -> bool:
    hours = item.get("recurrence_hours")
    if not hours:
        return True
    last = (state.get("last_completed") or {}).get(item["id"])
    if not last:
        return True
    try:
        then = datetime.fromisoformat(last.replace("Z", "+00:00"))
    except ValueError:
        return True
    delta_h = (datetime.now(timezone.utc) - then).total_seconds() / 3600
    return delta_h >= float(hours)


def pick_next(queue: dict, state: dict, *, owner: str | None = None) -> dict | None:
    completed = set(state.get("completed_ids") or [])
    items = sorted(queue.get("items") or [], key=lambda x: x.get("priority", 99))
    if owner:
        items = [it for it in items if it.get("owner") == owner]
    for it in items:
        if it["id"] in completed and not it.get("recurrence_hours"):
            continue
        if not gate_open(it):
            continue
        if not due_recurring(it, state):
            continue
        return it
    # recurring: allow re-run
    for it in items:
        if it.get("recurrence_hours") and gate_open(it) and due_recurring(it, state):
            return it
    return None


def run_shell(cmd: str, dry_run: bool) -> int:
    if dry_run:
        print(f"dry-run shell: {cmd}")
        return 0
    return subprocess.run(cmd, shell=True, cwd=REPO).returncode


def emit_waiting(queue: dict, state: dict) -> None:
    """Write NEXT_SLICE when no runnable cloud item (usually sprint gate)."""
    blocked = None
    completed = set(state.get("completed_ids") or [])
    for it in sorted(queue.get("items") or [], key=lambda x: x.get("priority", 99)):
        if it.get("owner") != "cloud":
            continue
        if it["id"] in completed and not it.get("recurrence_hours"):
            continue
        if not gate_open(it):
            blocked = it
            break
    progress = load(PROGRESS) if PROGRESS.is_file() else {}
    done = progress.get("completed_ids") or []
    gate = (blocked or {}).get("gate_completed_task", "dev-environment")
    lines = [
        "# Autonomous Coding Agent — NEXT SLICE",
        "",
        "**Status:** `waiting` — no cloud slice runnable",
        "",
        f"**Blocked by:** Coach sprint task `{gate}` not in progress completed_ids",
        f"**Completed sprint ids:** `{done}`",
        "",
        "## Your move (human)",
        f"1. Finish Coach task **`{gate}`** (Windows dev box).",
        "2. Mark done in Coach PWA or update `disklordz/audio-command/data/progress.json`.",
        "3. Re-run: `python3 scripts/autonomous-coder/tick.py --emit-slice`",
        "",
        "## Meanwhile (this agent may still run)",
        "```bash",
        "python3 scripts/autonomous-coder/tick.py          # Antigravity sync / progress log",
        "./scripts/run-audio-plugin-fleet.sh               # factory PASTE_TO_FACTORY refresh",
        "```",
    ]
    if blocked:
        lines += [
            "",
            f"**Next cloud slice when unblocked:** `{blocked['id']}` — {blocked.get('title', '')}",
        ]
    SLICE_OUT.parent.mkdir(parents=True, exist_ok=True)
    SLICE_OUT.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"Wrote {SLICE_OUT} (waiting)")


def emit_slice(item: dict) -> None:
    SLICE_OUT.parent.mkdir(parents=True, exist_ok=True)
    lines = [
        "# Autonomous Coding Agent — NEXT SLICE",
        "",
        f"**Queue id:** `{item['id']}`",
        f"**Title:** {item.get('title', '')}",
        "",
        "## Acceptance",
        item.get("acceptance", "See title."),
        "",
        "## Allowlist (only these paths)",
        "```json",
        json.dumps(item.get("allowlist") or [], indent=2),
        "```",
        "",
        "## Read first",
    ]
    for p in item.get("context_paths") or []:
        lines.append(f"- `{p}`")
    lines += [
        "",
        "## Agent",
        "- Skill: `disklordz-autonomous-coding-agent`",
        "- Soul: `disklordz/agents/profit/autonomous-coding-agent/soul.md`",
        "",
        "When done: `python3 scripts/autonomous-coder/tick.py --complete " + item["id"] + "`",
    ]
    SLICE_OUT.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"Wrote {SLICE_OUT}")


def mark_complete(item_id: str) -> None:
    state = load(STATE) if STATE.is_file() else {"completed_ids": [], "last_completed": {}}
    if item_id not in state.setdefault("completed_ids", []):
        state["completed_ids"].append(item_id)
    state.setdefault("last_completed", {})[item_id] = datetime.now(timezone.utc).strftime(
        "%Y-%m-%dT%H:%M:%SZ"
    )
    state["last_tick_at"] = state["last_completed"][item_id]
    save_state(state)
    print(f"completed {item_id}")


def check_progress() -> int:
    if not PROGRESS.is_file():
        print("progress.json missing")
        return 1
    p = load(PROGRESS)
    print(json.dumps({"updated_at": p.get("updated_at"), "completed_ids": p.get("completed_ids")}, indent=2))
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--emit-slice", action="store_true", help="Write NEXT_SLICE.md for Cursor Cloud")
    parser.add_argument("--complete", metavar="ID", help="Mark queue item complete")
    parser.add_argument("--check-progress", action="store_true")
    parser.add_argument("--dry-run", action="store_true")
    args = parser.parse_args()

    if args.check_progress:
        return check_progress()
    if args.complete:
        mark_complete(args.complete)
        return 0

    if not QUEUE.is_file():
        print(f"missing {QUEUE}", file=sys.stderr)
        return 1

    queue = load(QUEUE)
    state = load(STATE) if STATE.is_file() else {"completed_ids": [], "last_completed": {}}

    if args.emit_slice:
        item = pick_next(queue, state, owner="cloud")
        if not item:
            emit_waiting(queue, state)
            return 0
        emit_slice(item)
        return 0

    item = pick_next(queue, state)
    if not item:
        print("queue idle")
        return 0

    print(f"next_item={item['id']} owner={item.get('owner')} kind={item.get('kind')}")
    kind = item.get("kind")
    owner = item.get("owner")

    if owner == "cloud" and kind in ("code_slice", "factory_escalation"):
        emit_slice(item)
        print("Cloud slice ready — start Cursor Cloud with skill disklordz-autonomous-coding-agent")
        return 0

    if kind == "antigravity_dispatch":
        cmd = item.get("command", "./scripts/antigravity-bridge/sync-all-antigravity.sh")
        rc = run_shell(cmd, args.dry_run)
        if rc == 0 and not args.dry_run:
            mark_complete(item["id"])
        return rc

    if kind == "shell":
        cmd = item.get("command", "true")
        rc = run_shell(cmd, args.dry_run)
        if rc == 0 and not args.dry_run and not item.get("recurrence_hours"):
            mark_complete(item["id"])
        elif rc == 0 and not args.dry_run and item.get("recurrence_hours"):
            mark_complete(item["id"])  # updates last_completed via mark_complete
        return rc

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
