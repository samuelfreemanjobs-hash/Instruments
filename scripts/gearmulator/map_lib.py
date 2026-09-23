"""Build and validate instrument.map.json v1 from capture manifests."""
from __future__ import annotations

import csv
import json
import re
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


MAP_VERSION = 1


def slugify(value: str) -> str:
    s = re.sub(r"[^a-zA-Z0-9]+", "_", value.strip().lower())
    return s.strip("_") or "instrument"


def read_manifest_rows(manifest_path: Path) -> list[dict[str, str]]:
    with manifest_path.open(encoding="utf-8") as f:
        return list(csv.DictReader(f, delimiter="\t"))


def manifest_to_map(
    session_dir: Path,
    rows: list[dict[str, str]],
    *,
    instrument_id: str | None = None,
) -> dict[str, Any]:
    if not rows:
        raise ValueError("empty manifest")

    first = rows[0]
    label = first.get("instrument_label") or session_dir.name
    iid = instrument_id or slugify(label)

    zones: list[dict[str, Any]] = []
    for row in rows:
        note = int(row["note"])
        vel = int(row.get("velocity", "100"))
        wav_path = Path(row["wav"])
        wav_name = wav_path.name if wav_path.name else str(wav_path)
        zones.append(
            {
                "zone_id": f"n{note:03d}_v{vel:03d}",
                "wav": wav_name,
                "midi_note": note,
                "root_key": note,
                "lo_key": note,
                "hi_key": note,
                "lo_vel": vel,
                "hi_vel": vel,
                "seconds": float(row.get("seconds", "0")),
            }
        )

    sample_rate = int(first.get("sample_rate", "48000"))
    engine = first.get("engine", "unknown")
    plugin_path = first.get("plugin_path") or ""
    if not plugin_path:
        sp = first.get("source_plugin", "")
        if sp and ("/" in sp or sp.endswith(".vst3")):
            plugin_path = sp

    map_data: dict[str, Any] = {
        "map_format_version": MAP_VERSION,
        "instrument_id": iid,
        "instrument_label": label,
        "source": {
            "plugin_path": plugin_path,
            "source_plugin": _short_plugin_name(first),
            "program": int(first.get("program", "0")),
            "preset_path": first.get("preset_path") or None,
            "rom_note": "user_rom_not_in_git",
        },
        "session": {
            "session_name": session_dir.name,
            "engine": engine,
            "captured_at_utc": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
        },
        "audio": {"sample_rate": sample_rate, "bit_depth": 24},
        "zones": zones,
    }

    fx_plugin_path = (first.get("fx_plugin_path") or "").strip()
    if fx_plugin_path:
        map_data["post_fx"] = {
            "fx_plugin_path": fx_plugin_path,
            "fx_preset_path": first.get("fx_preset_path") or None,
            "fx_plugin": _short_plugin_name({"source_plugin": Path(fx_plugin_path).stem}),
            "always_on_policy": "INSTRUMENTS_ALWAYS_OSIRUS_FX",
        }

    return map_data


def _short_plugin_name(row: dict[str, str]) -> str:
    if row.get("source_plugin") and "/" not in row["source_plugin"]:
        return row["source_plugin"]
    path = row.get("source_plugin") or row.get("plugin_path") or ""
    base = Path(path).name
    return base.replace(".vst3", "") or "unknown"


def write_map(session_dir: Path, map_data: dict[str, Any]) -> Path:
    out = session_dir / "instrument.map.json"
    out.write_text(json.dumps(map_data, indent=2) + "\n", encoding="utf-8")
    return out


def load_map(path: Path) -> dict[str, Any]:
    data = json.loads(path.read_text(encoding="utf-8"))
    if data.get("map_format_version") != MAP_VERSION:
        raise ValueError(f"unsupported map_format_version: {data.get('map_format_version')}")
    return data


def finalize_session_dir(
    session_dir: Path,
    *,
    instrument_id: str | None = None,
    write_sfz: bool = False,
) -> Path:
    """Write instrument.map.json from manifest.tsv; optionally instrument.sfz."""
    session = session_dir.resolve()
    manifest = session / "manifest.tsv"
    if not manifest.is_file():
        raise FileNotFoundError(manifest)

    rows = read_manifest_rows(manifest)
    map_data = manifest_to_map(session, rows, instrument_id=instrument_id)
    map_path = write_map(session, map_data)

    if write_sfz:
        from map_to_sfz import write_sfz_from_map

        write_sfz_from_map(map_path)

    return map_path
