#!/usr/bin/env python3
"""On stop, surface a secret violation once. Do not loop the agent."""

from __future__ import annotations

import json
import sys
from pathlib import Path

STATE = Path(__file__).resolve().parent / "state" / "violation.json"


def main() -> int:
    raw = sys.stdin.read()
    try:
        payload = json.loads(raw) if raw.strip() else {}
    except json.JSONDecodeError:
        payload = {}
    loop_count = int(payload.get("loop_count") or 0)
    if loop_count > 0 or not STATE.exists():
        json.dump({}, sys.stdout)
        return 0
    report = STATE.read_text(encoding="utf-8")
    STATE.unlink(missing_ok=True)
    json.dump(
        {
            "followup_message": (
                "A hook found a live secret or an absolute sample-library path. "
                "Remove it before continuing. Details: " + report
            )
        },
        sys.stdout,
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
