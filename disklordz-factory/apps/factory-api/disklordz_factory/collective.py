from __future__ import annotations

import json
from functools import lru_cache
from pathlib import Path
from typing import Any

_COLLECTIVE_PATH = (
    Path(__file__).resolve().parents[3] / "database" / "artist_collective_seed.json"
)


@lru_cache(maxsize=1)
def load_collective_manifest() -> dict[str, Any]:
    if not _COLLECTIVE_PATH.is_file():
        return {"artists": [], "error": f"missing {_COLLECTIVE_PATH}"}
    with _COLLECTIVE_PATH.open(encoding="utf-8") as f:
        return json.load(f)


def list_collective_artists() -> list[dict[str, Any]]:
    manifest = load_collective_manifest()
    return list(manifest.get("artists", []))


def default_vintage_mission() -> str:
    manifest = load_collective_manifest()
    openclaw = manifest.get("openclaw") or {}
    return str(
        openclaw.get(
            "default_mission",
            "Vintage collective batch — four artist lanes (research-gated)",
        )
    )


def four_artist_ids() -> list[str]:
    return [a["artist_id"] for a in list_collective_artists() if a.get("artist_id")]
