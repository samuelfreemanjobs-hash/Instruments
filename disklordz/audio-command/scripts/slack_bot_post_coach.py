#!/usr/bin/env python3
"""Post coach nudge via Slack Web API (chat.postMessage) — for GitHub Actions."""

from __future__ import annotations

import argparse
import json
import os
import sys
import urllib.error
import urllib.request

from coach_engine import format_outreach_markdown, next_task
from coach_outreach import slack_payload

DEFAULT_CHANNEL = os.environ.get("SLACK_COACH_CHANNEL_ID", "C0C2AK80XCN")


def post_message(token: str, channel: str, body: dict) -> None:
    req = urllib.request.Request(
        "https://slack.com/api/chat.postMessage",
        data=json.dumps({"channel": channel, **body}).encode("utf-8"),
        headers={
            "Authorization": f"Bearer {token}",
            "Content-Type": "application/json; charset=utf-8",
        },
        method="POST",
    )
    try:
        with urllib.request.urlopen(req, timeout=30) as resp:
            data = json.loads(resp.read().decode("utf-8"))
    except urllib.error.HTTPError as e:
        raise SystemExit(f"Slack HTTP {e.code}: {e.read().decode()}") from e
    if not data.get("ok"):
        raise SystemExit(f"Slack API error: {data.get('error', data)}")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--send", action="store_true")
    parser.add_argument("--channel", default=DEFAULT_CHANNEL)
    parser.add_argument("--token", default=os.environ.get("SLACK_BOT_TOKEN", ""))
    args = parser.parse_args()

    task = next_task()
    text = format_outreach_markdown(task)
    body = slack_payload(text)

    if not args.send:
        print(text)
        if not args.token:
            print("\n(dry-run — set SLACK_BOT_TOKEN)", file=sys.stderr)
        return

    if not args.token:
        raise SystemExit("SLACK_BOT_TOKEN required for --send")

    post_message(args.token, args.channel, body)
    print("sent", file=sys.stderr)


if __name__ == "__main__":
    main()
