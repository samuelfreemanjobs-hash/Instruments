#!/usr/bin/env python3
"""Create disklordz/vst-factory/inbox handoff JSON from an Airtable Agent Work Order."""

from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
from pathlib import Path

from _airtable_common import airtable_get_wo, load_dotenv, patch_wo

REPO_ROOT = Path(__file__).resolve().parents[3]
BRIDGE = REPO_ROOT / "scripts" / "vst-factory-bridge" / "vst-factory-bridge.sh"


def main() -> None:
    load_dotenv()
    parser = argparse.ArgumentParser()
    parser.add_argument("--work-order-id", required=True)
    parser.add_argument("--branch", default=os.environ.get("HANDOFF_BRANCH", "main"))
    parser.add_argument("--patch-airtable", action="store_true")
    parser.add_argument("--dry-run", action="store_true")
    args = parser.parse_args()

    rec = airtable_get_wo(args.work_order_id)
    fields = rec.get("fields") or {}
    title = fields.get("title") or fields.get("Title") or args.work_order_id
    criteria = fields.get("acceptance_criteria") or fields.get("Acceptance Criteria") or ""
    owner = str(fields.get("owner_agent") or fields.get("Owner Agent") or "")
    status = str(fields.get("status") or "")

    if "[Plugin][JUCE]" not in title and "vst-plugin-factory" not in owner.lower():
        raise SystemExit(
            f"WO {args.work_order_id} is not VST Plugin Factory lane "
            f"(title must include [Plugin][JUCE] or owner_agent cursor-vst-plugin-factory)"
        )

    cmd = [
        str(BRIDGE),
        "send",
        "--wo",
        args.work_order_id,
        "--title",
        title,
        "--criteria",
        criteria,
        "--branch",
        args.branch,
        "--notes",
        f"Airtable status: {status}; owner: {owner}",
        "--path",
        "docs/VST_PLUGIN_FACTORY_AGENT.md",
        "--path",
        "disklordz/vst-factory/AGENT_INSTRUCTIONS.md",
    ]

    if args.dry_run:
        print("Would run:", " ".join(cmd))
        return

    if not BRIDGE.is_file():
        raise SystemExit(f"Missing bridge script: {BRIDGE}")

    subprocess.run(cmd, check=True, cwd=REPO_ROOT)

    inbox = REPO_ROOT / "disklordz" / "vst-factory" / "inbox"
    latest = sorted(inbox.glob("HO-*.json"), key=lambda p: p.stat().st_mtime, reverse=True)
    if not latest:
        raise SystemExit("Handoff file not created")
    handoff = json.loads(latest[0].read_text(encoding="utf-8"))
    hid = handoff.get("handoff_id")
    print(latest[0])
    print(json.dumps({"handoff_id": hid, "work_order_id": args.work_order_id}, indent=2))

    if args.patch_airtable and hid:
        try:
            patch_wo(rec["id"], {"vst_factory_handoff_id": hid, "status": "In Progress"})
        except Exception as exc:  # noqa: BLE001 — optional Airtable fields
            print(f"Note: Airtable patch skipped: {exc}", file=sys.stderr)


if __name__ == "__main__":
    main()
