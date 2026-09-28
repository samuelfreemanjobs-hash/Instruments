#!/usr/bin/env python3
"""Print note and seconds for a KR-106 golden scenario fixture."""

from __future__ import annotations

import json
import sys
from pathlib import Path

FIXTURES = Path(__file__).resolve().parents[1] / "fixtures" / "kr106_scenarios.json"


def main() -> None:
    scenario_id = sys.argv[1]
    data = json.loads(FIXTURES.read_text(encoding="utf-8"))
    spec = data["scenarios"][scenario_id]
    print(int(spec.get("note", 60)), float(spec.get("seconds", 3.0)))


if __name__ == "__main__":
    main()
