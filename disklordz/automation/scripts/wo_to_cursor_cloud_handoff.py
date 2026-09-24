#!/usr/bin/env python3
"""Create disklordz/cursor-cloud/inbox handoff JSON from an Airtable Agent Work Order."""

from __future__ import annotations

import argparse
import json
import os
import secrets
import sys
from datetime import datetime, timezone
from pathlib import Path

from _airtable_common import airtable_get_wo, load_dotenv, patch_wo

REPO_ROOT = Path(__file__).resolve().parents[3]
INBOX = REPO_ROOT / "disklordz" / "cursor-cloud" / "inbox"
DEFAULT_PATHS = [
    "docs/V_VOYAGER_PM.md",
    "disklordz/cursor-cloud/AGENT_INSTRUCTIONS.md",
    "plugin-factory/plugins/moog-voyager/README.md",
    "plugin-factory/ARCHITECTURE.md",
]


def new_handoff_id() -> str:
    day = datetime.now(timezone.utc).strftime("%Y%m%d")
    rand = secrets.token_hex(2).upper()
    return f"CO-{day}-{rand}"


def main() -> None:
    load_dotenv()
    parser = argparse.ArgumentParser()
    parser.add_argument("--work-order-id", required=True)
    parser.add_argument("--branch", default=os.environ.get("HANDOFF_BRANCH", "main"))
    parser.add_argument(
        "--patch-airtable",
        action="store_true",
        help="Set cursor_cloud_handoff_id on WO if field exists",
    )
    parser.add_argument("--dry-run", action="store_true")
    args = parser.parse_args()

    rec = airtable_get_wo(args.work_order_id)
    fields = rec.get("fields") or {}
    title = fields.get("title") or fields.get("Title") or args.work_order_id
    criteria = fields.get("acceptance_criteria") or fields.get("Acceptance Criteria") or ""
    owner = str(fields.get("owner_agent") or fields.get("Owner Agent") or "")
    status = str(fields.get("status") or "")
    product_id = str(fields.get("product_id") or fields.get("Product ID") or "DL-PLUGIN-V-VOYAGER")

    title_ok = "[Plugin][Factory]" in title
    owner_ok = "cursor-v-voyager" in owner.lower() or "cursor-cloud" in owner.lower()
    if not title_ok and not owner_ok:
        raise SystemExit(
            f"WO {args.work_order_id} is not Cursor Cloud factory lane "
            f"(title must include [Plugin][Factory] or owner_agent cursor-v-voyager)"
        )

    hid = new_handoff_id()
    prompt = (
        f"Work order {args.work_order_id}: {title}\n\n"
        f"Acceptance criteria:\n{criteria}\n\n"
        "You are cursor-v-voyager. Run plugin-factory factory.sh release for V Voyager changes."
    )
    doc = {
        "handoff_id": hid,
        "direction": "airtable_to_cursor_cloud",
        "from_agent": "airtable-disklordz-os",
        "to_agent": "cursor-v-voyager",
        "created_at": datetime.now(timezone.utc).isoformat(),
        "product_id": product_id,
        "work_order": {
            "id": args.work_order_id,
            "title": title,
            "acceptance_criteria": criteria,
            "branch": args.branch,
        },
        "cloud_agent_prompt": prompt,
        "context_paths": DEFAULT_PATHS,
        "status": "open",
        "notes": f"Airtable status: {status}; owner: {owner}",
    }

    out = INBOX / f"{hid}.json"
    if args.dry_run:
        print(json.dumps(doc, indent=2))
        return

    INBOX.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(doc, indent=2) + "\n", encoding="utf-8")
    print(out)
    print(json.dumps({"handoff_id": hid, "work_order_id": args.work_order_id}, indent=2))

    if args.patch_airtable:
        try:
            patch_wo(rec["id"], {"cursor_cloud_handoff_id": hid, "status": "In Progress"})
        except Exception as exc:  # noqa: BLE001 — optional Airtable fields
            print(f"warn: Airtable patch skipped ({exc})", file=sys.stderr)


if __name__ == "__main__":
    main()
