#!/usr/bin/env python3
"""Lightweight manifest checks (required fields). For full JSON Schema use an external validator."""
from __future__ import annotations

import json
import sys
from pathlib import Path

REQUIRED = (
    "projectId",
    "pluginName",
    "version",
    "type",
    "sampleless",
    "userContent",
    "formats",
    "p0Features",
    "presetPolicy",
    "repoPath",
)


def main() -> int:
    if len(sys.argv) != 2:
        print("Usage: validate_manifest.py path/to/plugin.manifest.json", file=sys.stderr)
        return 2

    path = Path(sys.argv[1])
    data = json.loads(path.read_text(encoding="utf-8"))
    missing = [k for k in REQUIRED if k not in data]
    if missing:
        print(f"FAIL missing keys: {missing}", file=sys.stderr)
        return 1

    policy = data["presetPolicy"]
    for k in ("rcMin", "betaMin", "retailMin"):
        if k not in policy:
            print(f"FAIL presetPolicy.{k} missing", file=sys.stderr)
            return 1

    if policy["rcMin"] > policy["betaMin"] or policy["betaMin"] > policy["retailMin"]:
        print("FAIL presetPolicy: rcMin <= betaMin <= retailMin required", file=sys.stderr)
        return 1

    print(f"OK {data['pluginName']} ({data['projectId']}) @ {path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
