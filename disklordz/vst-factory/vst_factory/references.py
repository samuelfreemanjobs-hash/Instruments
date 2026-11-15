from __future__ import annotations

import json
from pathlib import Path


def package_data() -> Path:
    return Path(__file__).resolve().parents[1] / "data"


def load_chord_engine_reference() -> dict:
    path = package_data() / "chord-engine-reference.json"
    return json.loads(path.read_text(encoding="utf-8"))


def liaison_status_path() -> Path:
    return package_data() / "liaison-status.json"


def update_liaison_greenfield_count(count: int) -> None:
    path = liaison_status_path()
    data = json.loads(path.read_text(encoding="utf-8"))
    from datetime import datetime, timezone

    data["active_greenfield_count"] = count
    data["updated_at"] = datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
    path.write_text(json.dumps(data, indent=2) + "\n", encoding="utf-8")
