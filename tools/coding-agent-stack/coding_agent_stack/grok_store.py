from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from coding_agent_stack.config import get_settings

_MAX_EVENTS = 20


def _store_path() -> Path:
    settings = get_settings()
    p = settings.repo_root / "tools/coding-agent-stack/.grok_events.json"
    p.parent.mkdir(parents=True, exist_ok=True)
    return p


def append_event(event: dict[str, Any]) -> None:
    path = _store_path()
    events: list[dict[str, Any]] = []
    if path.is_file():
        try:
            events = json.loads(path.read_text(encoding="utf-8"))
        except json.JSONDecodeError:
            events = []
    events.append(event)
    path.write_text(json.dumps(events[-_MAX_EVENTS:], indent=2), encoding="utf-8")


def last_event() -> dict[str, Any] | None:
    path = _store_path()
    if not path.is_file():
        return None
    try:
        events = json.loads(path.read_text(encoding="utf-8"))
    except json.JSONDecodeError:
        return None
    if not events:
        return None
    return events[-1]
