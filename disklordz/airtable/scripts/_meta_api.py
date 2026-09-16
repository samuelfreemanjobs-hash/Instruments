"""Airtable Metadata API helpers."""

from __future__ import annotations

import json
import urllib.error
import urllib.request

META_ROOT = "https://api.airtable.com/v0/meta/bases"

COLORS = ["blueLight2", "cyanLight2", "tealLight2", "greenLight2", "yellowLight2", "orangeLight2", "redLight2", "pinkLight2", "purpleLight2", "grayLight2"]


def meta_request(method: str, url: str, token: str, body: dict | None = None) -> dict:
    data = json.dumps(body).encode("utf-8") if body is not None else None
    req = urllib.request.Request(
        url,
        data=data,
        headers={
            "Authorization": f"Bearer {token}",
            "Content-Type": "application/json",
        },
        method=method,
    )
    try:
        with urllib.request.urlopen(req, timeout=120) as resp:
            return json.loads(resp.read().decode("utf-8"))
    except urllib.error.HTTPError as e:
        err = e.read().decode("utf-8", errors="replace")
        raise RuntimeError(f"Airtable Meta API {e.code}: {err}") from e


def schema_field_to_airtable(field: dict, color_idx: int) -> dict:
    at_type = field["type"]
    name = field["name"]
    out: dict = {"name": name, "type": at_type}
    if at_type == "singleSelect":
        opts = field.get("options") or []
        out["options"] = {
            "choices": [{"name": o, "color": COLORS[i % len(COLORS)]} for i, o in enumerate(opts)]
        }
    elif at_type == "multipleSelects":
        opts = field.get("options") or []
        out["options"] = {
            "choices": [{"name": o, "color": COLORS[i % len(COLORS)]} for i, o in enumerate(opts)]
        }
    elif at_type == "checkbox":
        out["options"] = {"icon": "check", "color": "greenBright"}
    elif at_type == "date":
        out["options"] = {"dateFormat": {"name": "iso", "format": "YYYY-MM-DD"}}
    elif at_type == "number":
        out["options"] = {"precision": 0}
    return out


def table_spec_from_schema(table: dict) -> dict:
    fields = []
    for i, f in enumerate(table.get("fields") or []):
        fields.append(schema_field_to_airtable(f, i))
    return {"name": table["name"], "fields": fields}


def list_bases(token: str) -> list[dict]:
    data = meta_request("GET", META_ROOT, token)
    return data.get("bases") or []


def create_base(token: str, name: str, tables: list[dict]) -> dict:
    body = {"name": name, "tables": tables}
    return meta_request("POST", META_ROOT, token, body)


def list_tables(token: str, base_id: str) -> list[dict]:
    data = meta_request("GET", f"{META_ROOT}/{base_id}/tables", token)
    return data.get("tables") or []
