from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from jsonschema import Draft202012Validator

_FACTORY_ROOT = Path(__file__).resolve().parents[3]
SCHEMAS_DIR = _FACTORY_ROOT / "schemas"
FIXTURES_DIR = _FACTORY_ROOT / "database" / "fixtures" / "compiled_music_prompt"

LANE_DOWNSTREAM_REQUIRED: dict[str, list[str]] = {
    "DL001": ["french_touch"],
    "DL002": ["memphis_90s", "cloud_phonk"],
    "DL006": ["screw"],
    "DL004": ["cyber_funk"],
}


def _load_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def compiled_prompt_schema() -> dict[str, Any]:
    return _load_json(SCHEMAS_DIR / "compiled_music_prompt.json")


def validate_compiled_prompt(doc: dict[str, Any]) -> list[str]:
    """Return human-readable errors; empty list means valid schema + lane rules."""
    errors: list[str] = []
    schema = compiled_prompt_schema()
    validator = Draft202012Validator(schema)
    for err in sorted(validator.iter_errors(doc), key=lambda e: list(e.path)):
        path = ".".join(str(p) for p in err.path) or "(root)"
        errors.append(f"{path}: {err.message}")

    if errors:
        return errors

    artist_id = doc.get("artist_id", "")
    downstream = doc.get("downstream") or {}
    for key in LANE_DOWNSTREAM_REQUIRED.get(artist_id, []):
        if key not in downstream or not isinstance(downstream[key], dict):
            errors.append(f"lane rule: {artist_id} requires downstream.{key}")

    if artist_id == "DL004":
        cyber = downstream.get("cyber_funk") or {}
        if not cyber.get("vapor_aesthetic"):
            errors.append("lane rule: DL004 downstream.cyber_funk.vapor_aesthetic required")

    return errors


def load_fixture(artist_id: str) -> dict[str, Any]:
    path = FIXTURES_DIR / f"{artist_id.upper()}.json"
    if not path.is_file():
        raise FileNotFoundError(str(path))
    return _load_json(path)


def validate_all_fixtures() -> dict[str, list[str]]:
    out: dict[str, list[str]] = {}
    if not FIXTURES_DIR.is_dir():
        return out
    for path in sorted(FIXTURES_DIR.glob("DL*.json")):
        doc = _load_json(path)
        out[path.stem] = validate_compiled_prompt(doc)
    return out
