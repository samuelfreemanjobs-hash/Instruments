#!/usr/bin/env python3
"""Flag live secrets written into source. Documentation examples are ignored."""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path

STATE = Path(__file__).resolve().parent / "state" / "violation.json"
PATTERNS = (
    re.compile(r"sk_live_[A-Za-z0-9]{8,}"),
    re.compile(r"whsec_[A-Za-z0-9]{8,}"),
    re.compile(r"-----BEGIN (?:RSA |EC |OPENSSH )?PRIVATE KEY-----"),
    re.compile(r"(?:/Users/|/home/)[^\s\"']+/(?:Samples|Library/Audio)"),
    re.compile(r"[A-Za-z]:\\Users\\[^\s\"']+\\Samples"),
)
SKIP_SUFFIXES = {".md", ".mdc", ".txt", ".example"}


def main() -> int:
    raw = sys.stdin.read()
    try:
        payload = json.loads(raw) if raw.strip() else {}
    except json.JSONDecodeError:
        return 0
    file_path = str(payload.get("file_path") or "")
    path = Path(file_path)
    if path.suffix.lower() in SKIP_SUFFIXES or not path.is_file():
        return 0
    text = path.read_text(encoding="utf-8", errors="ignore")
    hits = []
    for pattern in PATTERNS:
        match = pattern.search(text)
        if match:
            secret = match.group(0)
            redacted = secret[:4] + "…" + secret[-4:] if len(secret) > 12 else "[redacted]"
            hits.append({"line": text[: match.start()].count("\n") + 1, "match": redacted})
    if not hits:
        return 0
    STATE.parent.mkdir(parents=True, exist_ok=True)
    STATE.write_text(
        json.dumps({"file": file_path, "hits": hits}, indent=2),
        encoding="utf-8",
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
