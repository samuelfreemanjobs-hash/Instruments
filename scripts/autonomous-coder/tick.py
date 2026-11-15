#!/usr/bin/env python3
"""24/7 autonomous coding queue — deterministic tick + Cloud slice emitter."""

from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path

_SCRIPT_DIR = Path(__file__).resolve().parent
if str(_SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(_SCRIPT_DIR))

from cursor_cloud import launch_slice_agent
from github_pr import fetch_pr, github_token, pr_is_open, search_open_pr_by_title

REPO = _SCRIPT_DIR.parents[1]
QUEUE = REPO / "disklordz/autonomous-coder/coding-queue.json"
PLUGIN_QUEUE = REPO / "disklordz/autonomous-coder/plugin-pr-queue.json"
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


def github_repo_parts() -> tuple[str, str, str]:
    """Return (owner, repo, full_name_url)."""
    env_repo = (os.environ.get("GITHUB_REPOSITORY") or "").strip()
    if env_repo and "/" in env_repo:
        owner, name = env_repo.split("/", 1)
    else:
        owner, name = "samuelfreemanjobs-hash", "Instruments"
    plugin_q = load(PLUGIN_QUEUE) if PLUGIN_QUEUE.is_file() else {}
    url = (plugin_q.get("repo_url") or f"https://github.com/{owner}/{name}").rstrip("/")
    return owner, name, url


def resolve_plugin_pr(item: dict, owner: str, repo: str, token: str | None) -> dict | None:
    number = item.get("pr")
    if number is not None:
        pr = fetch_pr(owner, repo, int(number), token)
        return pr if pr_is_open(pr) else None
    keywords = item.get("title_match") or [item.get("title") or ""]
    return search_open_pr_by_title(owner, repo, keywords, token)


def pick_next_plugin_pr(state: dict) -> tuple[dict, dict | None] | None:
    if not PLUGIN_QUEUE.is_file():
        return None
    plugin_q = load(PLUGIN_QUEUE)
    owner, repo, _ = github_repo_parts()
    token = github_token()
    items = sorted(plugin_q.get("items") or [], key=lambda x: x.get("priority", 99))

    for it in items:
        pr = resolve_plugin_pr(it, owner, repo, token)
        if not pr:
            continue
        return it, pr
    return None


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
    cloud_items = [
        it
        for it in sorted(queue.get("items") or [], key=lambda x: x.get("priority", 99))
        if it.get("owner") == "cloud"
    ]
    pending_cloud = [
        it for it in cloud_items if it["id"] not in completed or it.get("recurrence_hours")
    ]
    for it in cloud_items:
        if it["id"] in completed and not it.get("recurrence_hours"):
            continue
        if not gate_open(it):
            blocked = it
            break
    progress = load(PROGRESS) if PROGRESS.is_file() else {}
    done = progress.get("completed_ids") or []
    coach_done = set(done)
    lines = [
        "# Autonomous Coding Agent — NEXT SLICE",
        "",
        "**Status:** `waiting` — no cloud slice runnable",
        "",
    ]
    if not pending_cloud:
        lines += [
            "**Reason:** Cloud coding queue idle (all slices complete).",
            f"**Coach completed_ids:** `{done}`",
            "",
            "## Human critical path",
            "1. Coach sprint **`plugin-skeleton`**: `build-windows.ps1` + REAPER/FL load.",
            "2. Mark done in Coach → add `plugin-skeleton` to `progress.json`.",
            "3. Re-run: `python3 scripts/autonomous-coder/tick.py --emit-slice` after queue grows.",
        ]
    else:
        gate = (blocked or {}).get("gate_completed_task", "dev-environment")
        lines += [
            f"**Blocked by:** Coach gate `{gate}`"
            + (" (not in completed_ids)" if gate not in coach_done else ""),
            f"**Completed sprint ids:** `{done}`",
            "",
            "## Your move (human)",
            f"1. Finish Coach task **`{gate}`**.",
            "2. Mark done in Coach PWA or update `disklordz/audio-command/data/progress.json`.",
            "3. Re-run: `python3 scripts/autonomous-coder/tick.py --emit-slice`",
        ]
        if blocked:
            lines += [
                "",
                f"**Next cloud slice when unblocked:** `{blocked['id']}` — {blocked.get('title', '')}",
            ]
    lines += [
        "",
        "## Meanwhile (this agent may still run)",
        "```bash",
        "python3 scripts/autonomous-coder/tick.py          # Antigravity sync / progress log",
        "python3 scripts/audio-plugin/factory-queue.py     # factory PASTE_TO_FACTORY refresh",
        "```",
    ]
    SLICE_OUT.parent.mkdir(parents=True, exist_ok=True)
    SLICE_OUT.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"Wrote {SLICE_OUT} (waiting)")


def emit_slice(item: dict, *, pr: dict | None = None) -> None:
    SLICE_OUT.parent.mkdir(parents=True, exist_ok=True)
    lines = [
        "# Autonomous Coding Agent — NEXT SLICE",
        "",
        f"**Queue id:** `{item['id']}`",
        f"**Title:** {item.get('title', '')}",
        "",
    ]
    if pr:
        lines += [
            f"**GitHub PR:** #{pr.get('number')}",
            f"**Branch:** `{pr.get('head', {}).get('ref') or pr.get('headRefName', '')}`",
            "",
        ]
    lines += [
        "## Acceptance",
        item.get("acceptance", "See title — plugin MVP + green CI + Windows VST3 artifact."),
        "",
    ]
    allowlist = item.get("allowlist")
    if allowlist:
        lines += [
            "## Allowlist (only these paths)",
            "```json",
            json.dumps(allowlist, indent=2),
            "```",
            "",
        ]
    else:
        lines += [
            "## Scope",
            "Plugin / VST engineering slice — edit only paths required for this PR (prefer PR branch).",
            "",
        ]
    lines.append("## Read first")
    for p in item.get("context_paths") or []:
        lines.append(f"- `{p}`")
    lines += [
        "",
        "## Agent",
        "- Skill: `disklordz-autonomous-coding-agent`",
        "- Soul: `disklordz/agents/profit/autonomous-coding-agent/soul.md`",
        "",
    ]
    if item["id"].startswith("plugin-"):
        lines += [
            "When done: push branch; ensure PR checks pass; do **not** merge.",
        ]
    else:
        lines += [
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


def pick_emit_item(state: dict) -> tuple[dict, dict | None] | tuple[dict, None]:
    plugin = pick_next_plugin_pr(state)
    if plugin:
        return plugin
    queue = load(QUEUE)
    item = pick_next(queue, state, owner="cloud")
    if item:
        return item, None
    return {"id": "waiting", "title": "waiting"}, None


def launch_agent_from_slice(state: dict) -> int:
    if not SLICE_OUT.is_file():
        print("SKIP launch: NEXT_SLICE.md missing — run --emit-slice first")
        return 0
    slice_md = SLICE_OUT.read_text(encoding="utf-8")
    if "**Status:** `waiting`" in slice_md:
        print("SKIP launch: queue waiting — no runnable slice")
        return 0

    pending = state.get("pending_launch") or {}
    item_id = pending.get("slice_id")
    if not item_id:
        print("SKIP launch: no pending_launch in state (re-run --emit-slice)")
        return 0

    plugin_q = load(PLUGIN_QUEUE) if PLUGIN_QUEUE.is_file() else {"items": []}
    item = next((i for i in plugin_q.get("items") or [] if i["id"] == item_id), None)
    if not item:
        queue = load(QUEUE)
        item = next((i for i in queue.get("items") or [] if i["id"] == item_id), None)
    if not item:
        print(f"SKIP launch: unknown slice id {item_id}")
        return 1

    owner, repo, repo_url = github_repo_parts()
    token = github_token()
    pr_number = pending.get("pr")
    pr = fetch_pr(owner, repo, int(pr_number), token) if pr_number else None
    plugin_q_data = load(PLUGIN_QUEUE) if PLUGIN_QUEUE.is_file() else {}
    default_ref = plugin_q_data.get("default_ref") or "main"
    ref = default_ref
    if pr:
        ref = (pr.get("head") or {}).get("ref") or pr.get("headRefName") or default_ref

    ok = launch_slice_agent(
        item,
        slice_md,
        repo_url=repo_url,
        ref=ref,
        pr=pr,
        state=state,
    )
    if ok:
        state.setdefault("active_plugin_slices", {})[item_id] = datetime.now(
            timezone.utc
        ).strftime("%Y-%m-%dT%H:%M:%SZ")
        state["last_tick_at"] = state["active_plugin_slices"][item_id]
        save_state(state)
    return 0 if ok else 0


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--emit-slice", action="store_true", help="Write NEXT_SLICE.md for Cursor Cloud")
    parser.add_argument("--launch-agent", action="store_true", help="Launch Cursor Cloud agent for current slice")
    parser.add_argument("--complete", metavar="ID", help="Mark queue item complete")
    parser.add_argument("--check-progress", action="store_true")
    parser.add_argument("--dry-run", action="store_true")
    args = parser.parse_args()

    if args.check_progress:
        return check_progress()
    if args.complete:
        mark_complete(args.complete)
        return 0

    state = load(STATE) if STATE.is_file() else {"completed_ids": [], "last_completed": {}}

    if args.launch_agent:
        return launch_agent_from_slice(state)

    if not QUEUE.is_file():
        print(f"missing {QUEUE}", file=sys.stderr)
        return 1

    queue = load(QUEUE)

    if args.emit_slice:
        item, pr = pick_emit_item(state)
        if item.get("id") == "waiting":
            emit_waiting(queue, state)
            state.pop("pending_launch", None)
            save_state(state)
            return 0
        emit_slice(item, pr=pr)
        pr_num = (pr or {}).get("number")
        state["pending_launch"] = {"slice_id": item["id"], "pr": pr_num}
        state["last_tick_at"] = datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
        save_state(state)
        if item["id"].startswith("plugin-"):
            print("Plugin slice ready — run tick.py --launch-agent (or CI step)")
        else:
            print("Cloud slice ready — run tick.py --launch-agent")
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
            mark_complete(item["id"])
        return rc

    return 0


if __name__ == "__main__":
    raise SystemExit(main())