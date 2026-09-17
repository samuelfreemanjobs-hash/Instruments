#!/usr/bin/env python3
"""MCP skills for DiskLordz ensemble: lane BPM, brief validation, MIDI map shell."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from mcp.server.fastmcp import FastMCP

ROOT = Path(__file__).resolve().parents[2]
COLLECTIVE = ROOT / "database" / "artist_collective_seed.json"
DL_BRF_SCHEMA = ROOT / "schemas" / "dl_brf.json"
PROMPTS = ROOT / "prompts" / "artists"

mcp = FastMCP(
    "factory-music",
    instructions=(
        "DiskLordz vintage collective MCP: validate BPM against greenlit artist lanes, "
        "validate DL-BRF production brief JSON, format MIDI map shells, compile artist prompts."
    ),
)


def _load_collective() -> dict[str, Any]:
    with COLLECTIVE.open(encoding="utf-8") as f:
        return json.load(f)


def _artist_row(artist_id: str) -> dict[str, Any] | None:
    for a in _load_collective().get("artists", []):
        if a.get("artist_id") == artist_id:
            return a
    return None


@mcp.tool()
def get_lane_constraints(artist_id: str) -> str:
    """Return JSON lane rules (BPM, vintage source, forbidden tags) for DL001/DL002/DL004/DL006."""
    row = _artist_row(artist_id.upper())
    if not row:
        return json.dumps({"ok": False, "error": "unknown or non-greenlit artist_id"})
    return json.dumps({"ok": True, "lane": row}, indent=2)


@mcp.tool()
def check_artist_bpm(artist_id: str, bpm: float) -> str:
    """Scale-check BPM against artist lane (harmonic/tempo gate before arrangement)."""
    row = _artist_row(artist_id.upper())
    if not row:
        return json.dumps({"ok": False, "valid": False, "error": "unknown artist_id"})
    lo, hi = float(row["bpm_min"]), float(row["bpm_max"])
    valid = lo <= bpm <= hi
    return json.dumps(
        {
            "ok": True,
            "valid": valid,
            "artist_id": artist_id.upper(),
            "bpm": bpm,
            "allowed_range": [lo, hi],
            "youtube_brand": row.get("youtube_brand_name"),
        }
    )


@mcp.tool()
def validate_dl_brf(brief_json: str) -> str:
    """Validate a DL-BRF production brief JSON string against dl_brf.json required fields and BPM lane."""
    try:
        brief = json.loads(brief_json)
    except json.JSONDecodeError as e:
        return json.dumps({"ok": False, "valid": False, "error": str(e)})
    required = ["artist_id", "bpm", "key", "vintage_source_tags", "transform_summary"]
    missing = [k for k in required if k not in brief]
    if missing:
        return json.dumps({"ok": False, "valid": False, "missing_fields": missing})
    bpm_check = json.loads(check_artist_bpm(brief["artist_id"], float(brief["bpm"])))
    if not bpm_check.get("valid"):
        return json.dumps({"ok": True, "valid": False, "bpm_check": bpm_check})
    return json.dumps({"ok": True, "valid": True, "bpm_check": bpm_check})


@mcp.tool()
def format_midi_map_shell(artist_id: str, key: str, bpm: float, bars: int = 8) -> str:
    """Return a JSON shell for sectional MIDI maps (keys/rhythm) aligned to lane BPM."""
    bpm_check = json.loads(check_artist_bpm(artist_id, bpm))
    row = _artist_row(artist_id.upper()) or {}
    return json.dumps(
        {
            "artist_id": artist_id.upper(),
            "sku_prefix": row.get("sku_prefix"),
            "key": key,
            "bpm": bpm,
            "bars": bars,
            "bpm_valid": bpm_check.get("valid"),
            "tracks": [
                {"role": "chords", "channel": 1, "notes": []},
                {"role": "bass", "channel": 2, "notes": []},
                {"role": "drums", "channel": 10, "notes": []},
            ],
        },
        indent=2,
    )


@mcp.tool()
def compile_artist_prompt(artist_id: str) -> str:
    """Compile SOUL/prompt text for an artist lane guardian (prompt compiling schema)."""
    path = PROMPTS / f"{artist_id.upper()}.md"
    if not path.is_file():
        return json.dumps({"ok": False, "error": f"missing {path}"})
    return json.dumps({"ok": True, "artist_id": artist_id.upper(), "prompt": path.read_text(encoding="utf-8")})


@mcp.tool()
def dl_brf_schema() -> str:
    """Return DL-BRF JSON Schema for Maestro/section agents."""
    return DL_BRF_SCHEMA.read_text(encoding="utf-8")


if __name__ == "__main__":
    mcp.run()
