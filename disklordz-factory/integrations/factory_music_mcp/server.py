#!/usr/bin/env python3
"""MCP skills for DiskLordz ensemble: lane BPM, brief validation, MIDI map shell."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from mcp.server.fastmcp import FastMCP

ROOT = Path(__file__).resolve().parents[2]
COLLECTIVE = ROOT / "database" / "artist_collective_seed.json"
RECORDING_CHAINS = ROOT / "openclaw" / "personas" / "recording_chains.json"
DL_BRF_SCHEMA = ROOT / "schemas" / "dl_brf.json"
SECTION_PAYLOAD_SCHEMA = ROOT / "schemas" / "dl_section_payload.json"
COMPILED_PROMPT_SCHEMA = ROOT / "schemas" / "compiled_music_prompt.json"
SONIC_MOVEMENTS = ROOT / "database" / "sonic_archaeology_movements.json"
ARTIST_LANES = ROOT / "database" / "artist_lane_processing.json"
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
def get_recording_chain(agent_identifier: str) -> str:
    """Return instrument domain and emulated chain for bandleader|keys_arranger|rhythm_section|vocal_topline."""
    if not RECORDING_CHAINS.is_file():
        return json.dumps({"ok": False, "error": "recording_chains.json missing"})
    with RECORDING_CHAINS.open(encoding="utf-8") as f:
        data = json.load(f)
    block = (data or {}).get("agents", {}).get(agent_identifier)
    if not block:
        return json.dumps({"ok": False, "error": f"unknown agent_identifier: {agent_identifier}"})
    return json.dumps({"ok": True, "agent_identifier": agent_identifier, **block}, indent=2)


@mcp.tool()
def dl_brf_schema() -> str:
    """Return DL-BRF JSON Schema for Maestro/section agents."""
    return DL_BRF_SCHEMA.read_text(encoding="utf-8")


@mcp.tool()
def section_payload_schema() -> str:
    """Return dl_section_payload.json schema for sectional fork outputs."""
    return SECTION_PAYLOAD_SCHEMA.read_text(encoding="utf-8")


@mcp.tool()
def compiled_prompt_schema() -> str:
    """Return compiled_music_prompt.json schema for Maestro merge output."""
    return COMPILED_PROMPT_SCHEMA.read_text(encoding="utf-8")


@mcp.tool()
def get_sonic_movement(movement_id: str) -> str:
    """Return screw|memphis_90s|cloud_phonk processing profile for downstream prompt compile."""
    if not SONIC_MOVEMENTS.is_file():
        return json.dumps({"ok": False, "error": "sonic_archaeology_movements.json missing"})
    with SONIC_MOVEMENTS.open(encoding="utf-8") as f:
        data = json.load(f)
    for m in data.get("movements", []):
        if m.get("id") == movement_id:
            return json.dumps({"ok": True, "movement": m}, indent=2)
    return json.dumps({"ok": False, "error": f"unknown movement_id: {movement_id}"})


@mcp.tool()
def get_artist_lane(artist_id: str) -> str:
    """Return full lane processing profile for DL001|DL002|DL004|DL006 (parity registry)."""
    if not ARTIST_LANES.is_file():
        return json.dumps({"ok": False, "error": "artist_lane_processing.json missing"})
    with ARTIST_LANES.open(encoding="utf-8") as f:
        data = json.load(f)
    block = (data or {}).get("artists", {}).get(artist_id.upper())
    if not block:
        return json.dumps({"ok": False, "error": f"unknown artist_id: {artist_id}"})
    return json.dumps({"ok": True, "artist_id": artist_id.upper(), **block}, indent=2)


@mcp.tool()
def validate_section_payload(payload_json: str) -> str:
    """Validate sectional JSON (required fields) before Maestro merge."""
    try:
        body = json.loads(payload_json)
    except json.JSONDecodeError as e:
        return json.dumps({"ok": False, "valid": False, "error": str(e)})
    required = ["agent_identifier", "artist_id", "movement_target", "analog_perspective", "payload"]
    missing = [k for k in required if k not in body]
    if missing:
        return json.dumps({"ok": False, "valid": False, "missing_fields": missing})
    ap = body.get("analog_perspective") or {}
    if ap.get("reject_modern_digital") is not True:
        return json.dumps({"ok": False, "valid": False, "error": "analog_perspective.reject_modern_digital must be true"})
    aid = body.get("agent_identifier")
    pl = body.get("payload") or {}
    section_required = {
        "keys_arranger": ["key", "voicing_types", "hardware_emulation"],
        "rhythm_section": ["pocket_description", "hardware_emulation"],
        "vocal_topline": ["delivery", "harmony_stack"],
    }.get(aid, [])
    sec_missing = [k for k in section_required if k not in pl]
    if sec_missing:
        return json.dumps({"ok": False, "valid": False, "missing_payload_fields": sec_missing})
    return json.dumps({"ok": True, "valid": True, "agent_identifier": aid})


if __name__ == "__main__":
    mcp.run()
