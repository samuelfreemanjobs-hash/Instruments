#!/usr/bin/env python3
"""Minimal Airtable REST client for DiskLordz agents and automation scripts."""

from __future__ import annotations

import argparse
import json
import os
import sys
import urllib.error
import urllib.parse
import urllib.request

API_ROOT = "https://api.airtable.com/v0"


def _env(name: str, default: str | None = None) -> str:
    val = os.environ.get(name, default)
    if not val:
        print(f"Missing env: {name}", file=sys.stderr)
        sys.exit(1)
    return val


def request(method: str, path: str, body: dict | None = None) -> dict:
    api_key = _env("AIRTABLE_API_KEY")
    base_id = _env("AIRTABLE_BASE_ID")
    url = f"{API_ROOT}/{base_id}/{urllib.parse.quote(path)}"
    data = None
    headers = {
        "Authorization": f"Bearer {api_key}",
        "Content-Type": "application/json",
    }
    if body is not None:
        data = json.dumps(body).encode("utf-8")
    req = urllib.request.Request(url, data=data, headers=headers, method=method)
    try:
        with urllib.request.urlopen(req, timeout=60) as resp:
            return json.loads(resp.read().decode("utf-8"))
    except urllib.error.HTTPError as e:
        err_body = e.read().decode("utf-8", errors="replace")
        print(f"Airtable HTTP {e.code}: {err_body}", file=sys.stderr)
        sys.exit(1)


def list_records(table: str, max_records: int = 100) -> dict:
    params = urllib.parse.urlencode({"maxRecords": max_records})
    path = f"{table}?{params}"
    return request("GET", path)


def create_record(table: str, fields: dict) -> dict:
    return request("POST", table, {"fields": fields})


def update_record(table: str, record_id: str, fields: dict) -> dict:
    return request("PATCH", f"{table}/{record_id}", {"fields": fields})


def main() -> None:
    parser = argparse.ArgumentParser(description="DiskLordz Airtable CLI")
    sub = parser.add_subparsers(dest="cmd", required=True)

    p_list = sub.add_parser("list", help="List records in a table")
    p_list.add_argument("table")
    p_list.add_argument("--max", type=int, default=20)

    p_create = sub.add_parser("create", help="Create a record")
    p_create.add_argument("table")
    p_create.add_argument("fields_json", help='JSON object of fields, e.g. \'{"title":"..."}\'')

    p_patch = sub.add_parser("patch", help="Update a record")
    p_patch.add_argument("table")
    p_patch.add_argument("record_id")
    p_patch.add_argument("fields_json")

    args = parser.parse_args()
    if args.cmd == "list":
        result = list_records(args.table, args.max)
        print(json.dumps(result, indent=2))
    elif args.cmd == "create":
        fields = json.loads(args.fields_json)
        result = create_record(args.table, fields)
        print(json.dumps(result, indent=2))
    elif args.cmd == "patch":
        fields = json.loads(args.fields_json)
        result = update_record(args.table, args.record_id, fields)
        print(json.dumps(result, indent=2))


if __name__ == "__main__":
    main()
