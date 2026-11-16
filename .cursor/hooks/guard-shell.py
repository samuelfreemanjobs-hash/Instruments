#!/usr/bin/env python3
"""Deny destructive shell commands. Reads the Cursor hook payload on stdin."""

from __future__ import annotations

import json
import re
import sys


def decide(command: str) -> dict:
    normalized = command.strip()
    if re.search(r"\brm\s+(-[^\s]*f[^\s]*r|-[^\s]*r[^\s]*f|--recursive)\b", normalized):
        if re.search(
            r"(^|\s)(/|~|~/)(\s|$)"
            r"|sound-factory/kits"
            r"|sample-library"
            r"|/masters(\s|$)"
            r"|raw-bounces",
            normalized,
        ):
            return {
                "permission": "deny",
                "user_message": "Blocked a recursive delete of a sample master or filesystem root.",
                "agent_message": "Do not rm -rf sample masters, sound-factory/kits, or the filesystem root. Delete a named build directory instead.",
            }
    if re.search(r"\bgit\s+push\b", normalized) and re.search(r"(--force|-f)\b", normalized):
        if "--force-with-lease" not in normalized or re.search(r"\b(main|master)\b", normalized):
            return {
                "permission": "deny",
                "user_message": "Blocked a force push.",
                "agent_message": "Force-push to main/master is refused. A feature-branch force-with-lease is the only lease this hook allows, and not onto main.",
            }
    if re.search(r"\bgit\s+tag\s+-d\b|\bgit\s+push\b.*--delete", normalized):
        return {
            "permission": "deny",
            "user_message": "Blocked a tag or remote-branch delete.",
            "agent_message": "Do not delete tags or remote branches from the agent.",
        }
    return {"permission": "allow"}


def main() -> int:
    raw = sys.stdin.read()
    try:
        payload = json.loads(raw) if raw.strip() else {}
    except json.JSONDecodeError:
        payload = {"command": raw}
    command = str(payload.get("command") or "")
    json.dump(decide(command), sys.stdout)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
