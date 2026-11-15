#!/usr/bin/env python3
"""Route Claude Code support requests and DPF catalog into Antigravity inbox."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
CC_INBOX = REPO / "disklordz/antigravity/claude-code-inbox"
CATALOG = REPO / "disklordz/antigravity/claude-code-support-catalog.json"
QUEUE = REPO / "disklordz/antigravity/active-queue.json"
ROUTING = REPO / "disklordz/antigravity/product-routing.json"
DISPATCH = REPO / "scripts/antigravity-bridge/dispatch-active.sh"
BRIDGE = REPO / "scripts/antigravity-bridge/antigravity-bridge.sh"
INBOX = REPO / "disklordz/antigravity/inbox"

KIND_TO_WO = {
    "windows_build": "WO-CC-001",
    "pytest_run": "WO-CC-002",
    "codegen_stub": "WO-CC-003",
    "midi_fixture": "WO-CC-004",
    "cmake_scaffold": "WO-CC-005",
}


def load(path: Path) -> dict | list:
    return json.loads(path.read_text(encoding="utf-8"))


def merge_catalog(catalog: dict, queue: dict) -> bool:
    existing = {it.get("wo_id") for it in queue.get("items", [])}
    changed = False
    for it in catalog.get("items", []):
        wo = it.get("wo_id")
        if not wo or wo in existing:
            continue
        queue.setdefault("items", []).append(
            {
                "wo_id": wo,
                "lane": it.get("lane", "batch"),
                "title": it["title"],
                "criteria": it.get("criteria", ""),
                "context_paths": it.get("context_paths", []),
                "priority": it.get("priority", 30),
            }
        )
        existing.add(wo)
        changed = True
    if changed:
        queue["items"] = sorted(queue["items"], key=lambda x: x.get("priority", 99))
    return changed


def open_requests() -> list[dict]:
    out = []
    for p in sorted(CC_INBOX.glob("CC-*.json")):
        try:
            d = json.loads(p.read_text(encoding="utf-8"))
            if d.get("status") == "open":
                d["_path"] = p
                out.append(d)
        except Exception:
            continue
    return out


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


def dispatch_wo(wo: str, push: bool) -> None:
    cmd = [str(DISPATCH), "--wo", wo]
    if push:
        cmd.append("--push")
    subprocess.run(cmd, cwd=REPO, check=False)


def dispatch_dynamic(req: dict, push: bool) -> None:
    """Direct handoff for ad-hoc Claude Code slice."""
    wo = req["request_id"]
    title = f"[ClaudeCode][DPF] {req.get('dpf_area','DPF')}: {req.get('slice','support')[:120]}"
    criteria = (
        f"Support kind: {req.get('support_kind')}. "
        f"Deliver artifacts under disklordz/antigravity/outbox-artifacts/claude-code/ "
        f"and publish-handoff.ps1 to claude-code. Notes: {req.get('notes','')}"
    )
    cmd = [
        str(BRIDGE),
        "send",
        "--wo",
        wo,
        "--title",
        title,
        "--criteria",
        criteria,
        "--to-agent",
        "antigravity-dpf-support",
        "--notes",
        f"Claude Code request {req['request_id']}",
    ]
    for p in req.get("context_paths") or []:
        cmd.extend(["--path", p])
    cmd.extend(
        [
            "--path",
            "docs/ANTIGRAVITY_CLAUDE_CODE_COPILOT.md",
        ]
    )
    if push:
        cmd.append("--push")
    subprocess.run(cmd, cwd=REPO, check=False)
    req_path = req.get("_path")
    if req_path and isinstance(req_path, Path):
        doc = json.loads(req_path.read_text(encoding="utf-8"))
        doc["status"] = "dispatched"
        doc["handoff_wo"] = KIND_TO_WO.get(req.get("support_kind"), wo)
        req_path.write_text(json.dumps(doc, indent=2) + "\n", encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--merge-only", action="store_true")
    parser.add_argument("--dispatch", action="store_true")
    parser.add_argument("--push", action="store_true")
    parser.add_argument("--max", type=int, default=2)
    parser.add_argument("--dry-run", action="store_true")
    args = parser.parse_args()

    if not CATALOG.is_file():
        print(f"missing {CATALOG}", file=sys.stderr)
        return 1

    queue = load(QUEUE) if QUEUE.is_file() else {"items": [], "completed_wo_ids": []}
    catalog = load(CATALOG)
    changed = merge_catalog(catalog, queue)
    if changed and not args.dry_run:
        QUEUE.write_text(json.dumps(queue, indent=2) + "\n", encoding="utf-8")
        print("merged claude-code catalog into active-queue")

    if args.merge_only and not args.dispatch:
        return 0

    reqs = open_requests()
    print(f"open_claude_requests={len(reqs)}")

    if args.dry_run or not args.dispatch:
        for r in reqs[: args.max]:
            kind = r.get("support_kind")
            print(f"  would dispatch kind={kind} -> {KIND_TO_WO.get(kind, 'dynamic')}")
        return 0

    open_wos = open_wo_ids()
    done = 0
    for req in reqs[: args.max]:
        kind = req.get("support_kind")
        mapped = KIND_TO_WO.get(kind)
        if mapped and mapped not in open_wos:
            dispatch_wo(mapped, args.push)
            req_path = req.get("_path")
            if req_path:
                doc = json.loads(req_path.read_text(encoding="utf-8"))
                doc["status"] = "dispatched"
                doc["handoff_wo"] = mapped
                req_path.write_text(json.dumps(doc, indent=2) + "\n", encoding="utf-8")
        else:
            dispatch_dynamic(req, args.push)
        done += 1
    print(f"dispatched={done}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
