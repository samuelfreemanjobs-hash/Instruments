#!/usr/bin/env python3
"""Merge product-prep catalog into active-queue and dispatch eligible prep work."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
ROUTING = REPO / "disklordz/antigravity/product-routing.json"
CATALOG = REPO / "disklordz/antigravity/product-prep-catalog.json"
QUEUE = REPO / "disklordz/antigravity/active-queue.json"
PROGRESS = REPO / "disklordz/audio-command/data/progress.json"
PLAYBOOKS = REPO / "disklordz/audio-command/data/task-playbooks.json"
DISPATCH = REPO / "scripts/antigravity-bridge/dispatch-active.sh"
INBOX = REPO / "disklordz/antigravity/inbox"


def load(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def next_playbook_task(completed: set[str], playbooks: dict) -> str | None:
    order = list(playbooks.keys())
    for tid in order:
        if tid not in completed:
            return tid
    return None


def open_wo_ids() -> set[str]:
    ids: set[str] = set()
    for p in INBOX.glob("HO-*.json"):
        try:
            d = json.loads(p.read_text(encoding="utf-8"))
            wid = (d.get("work_order") or {}).get("id")
            if wid:
                ids.add(wid)
        except Exception:
            continue
    return ids


def catalog_item_map(catalog: dict) -> dict[str, dict]:
    return {it["wo_id"]: it for it in catalog.get("items", [])}


def eligible_wo_ids(
    routing: dict,
    catalog: dict,
    progress: dict,
    current_task: str | None,
) -> list[str]:
    completed = set(progress.get("completed_ids") or [])
    completed_wo = set(load(QUEUE).get("completed_wo_ids") or [])
    open_inbox = open_wo_ids()
    limits = routing.get("dispatch_limits") or {}
    respect_open = limits.get("respect_open_inbox_per_wo", True)
    item_map = catalog_item_map(catalog)
    sprint_map = routing.get("sprint_task_routing") or {}

    candidates: list[tuple[int, str]] = []

    def add_ids(ids: list[str] | None, source: str) -> None:
        if not ids:
            return
        for wo in ids:
            it = item_map.get(wo)
            if not it:
                continue
            if wo in completed_wo:
                continue
            if respect_open and wo in open_inbox:
                continue
            req = it.get("requires_completed_task")
            if req and req not in completed:
                continue
            pri = int(it.get("priority", 50))
            candidates.append((pri, wo))

    if current_task and current_task in sprint_map:
        rule = sprint_map[current_task]
        add_ids(rule.get("antigravity_before"), "before")
        add_ids(rule.get("antigravity_parallel"), "parallel")
        if current_task in completed:
            add_ids(rule.get("antigravity_after_done"), "after_done")

    for it in catalog.get("items", []):
        wo = it["wo_id"]
        st = it.get("sprint_task")
        if st == current_task or (st and st in completed and not it.get("requires_completed_task")):
            add_ids([wo], "catalog")

    # Dedupe by wo, sort priority
    seen: set[str] = set()
    ordered: list[str] = []
    for _, wo in sorted(candidates, key=lambda x: x[0]):
        if wo in seen:
            continue
        seen.add(wo)
        ordered.append(wo)
    return ordered


def merge_catalog_into_queue(catalog: dict, queue: dict) -> bool:
    """Append catalog items not already in queue.items (by wo_id)."""
    existing = {it.get("wo_id") for it in queue.get("items", [])}
    changed = False
    for it in catalog.get("items", []):
        wo = it.get("wo_id")
        if not wo or wo in existing:
            continue
        queue.setdefault("items", []).append(
            {
                "wo_id": wo,
                "lane": it.get("lane", "prep"),
                "title": it["title"],
                "criteria": it.get("criteria", ""),
                "context_paths": it.get("context_paths", []),
                "priority": it.get("priority", 50),
            }
        )
        existing.add(wo)
        changed = True
    if changed:
        queue["items"] = sorted(queue["items"], key=lambda x: x.get("priority", 99))
    return changed


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--merge-only", action="store_true", help="Update active-queue.json from catalog")
    parser.add_argument("--dispatch", action="store_true", help="Dispatch up to N handoffs after merge")
    parser.add_argument("--push", action="store_true", help="Pass --push to dispatch-active.sh")
    parser.add_argument("--max", type=int, default=0, help="Override max dispatches (0 = routing default)")
    parser.add_argument("--dry-run", action="store_true")
    args = parser.parse_args()

    for path in (ROUTING, CATALOG, QUEUE, PROGRESS, PLAYBOOKS):
        if not path.is_file():
            print(f"missing required file: {path}", file=sys.stderr)
            return 1

    routing = load(ROUTING)
    catalog = load(CATALOG)
    queue = load(QUEUE)
    progress = load(PROGRESS)
    playbooks = load(PLAYBOOKS)

    current = next_playbook_task(set(progress.get("completed_ids") or []), playbooks)
    print(f"current_sprint_task={current}")

    changed = merge_catalog_into_queue(catalog, queue)
    if changed and not args.dry_run:
        QUEUE.write_text(json.dumps(queue, indent=2) + "\n", encoding="utf-8")
        print(f"merged catalog into {QUEUE}")

    if args.merge_only and not args.dispatch:
        return 0

    max_n = args.max or int((routing.get("dispatch_limits") or {}).get("max_new_handoffs_per_run", 2))
    eligible = eligible_wo_ids(routing, catalog, progress, current)
    print("eligible:", " ".join(eligible[:max_n]) or "(none)")

    if args.dry_run or not args.dispatch:
        return 0

    dispatched = 0
    for wo in eligible[:max_n]:
        cmd = [str(DISPATCH), "--wo", wo]
        if args.push:
            cmd.append("--push")
        print("run:", " ".join(cmd))
        subprocess.run(cmd, check=False, cwd=REPO)
        dispatched += 1

    print(f"dispatched_count={dispatched}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
