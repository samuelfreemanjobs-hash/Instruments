#!/usr/bin/env python3
"""Send DiskLordz Audio Coach daily nudge (Slack/generic webhook)."""

from __future__ import annotations

import argparse
import json
import os
import sys
import urllib.error
import urllib.request

from coach_engine import format_outreach_markdown, next_task


def post_webhook(url: str, body: dict) -> None:
    data = json.dumps(body).encode("utf-8")
    req = urllib.request.Request(
        url,
        data=data,
        headers={"Content-Type": "application/json"},
        method="POST",
    )
    try:
        with urllib.request.urlopen(req, timeout=30) as resp:
            if resp.status >= 300:
                raise RuntimeError(f"webhook status {resp.status}")
    except urllib.error.HTTPError as e:
        raise SystemExit(f"webhook failed: {e.code} {e.reason}") from e
    except urllib.error.URLError as e:
        raise SystemExit(f"webhook unreachable: {e.reason}") from e


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--send", action="store_true", help="POST to DISKLORDZ_COACH_WEBHOOK_URL")
    parser.add_argument(
        "--webhook",
        default=os.environ.get("DISKLORDZ_COACH_WEBHOOK_URL", ""),
        help="Slack incoming webhook or generic JSON URL",
    )
    args = parser.parse_args()

    task = next_task()
    text = format_outreach_markdown(task)
    payload = {"text": text}

    if not args.send:
        print(text)
        if not args.webhook:
            print("\n(dry-run — set DISKLORDZ_COACH_WEBHOOK_URL and --send to deliver)", file=sys.stderr)
        return

    if not args.webhook:
        raise SystemExit("DISKLORDZ_COACH_WEBHOOK_URL required for --send")

    post_webhook(args.webhook, payload)
    print("sent", file=sys.stderr)


if __name__ == "__main__":
    main()
