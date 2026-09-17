#!/usr/bin/env python3
"""Create DiskLordz Airtable base from base-schema.json via Metadata API."""

from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path

from _meta_api import create_base, list_bases, table_spec_from_schema

SCRIPT_DIR = Path(__file__).resolve().parent
DEFAULT_SCHEMA = SCRIPT_DIR.parent / "base-schema.json"


def load_token(cli_token: str | None) -> str:
    import os

    token = cli_token or os.environ.get("AIRTABLE_API_KEY")
    if not token:
        print("Set AIRTABLE_API_KEY or pass --token", file=sys.stderr)
        sys.exit(1)
    return token


def write_base_id_to_env(env_path: Path, base_id: str) -> None:
    text = env_path.read_text(encoding="utf-8") if env_path.is_file() else ""
    if re.search(r"^AIRTABLE_BASE_ID=.*$", text, flags=re.MULTILINE):
        text = re.sub(r"^AIRTABLE_BASE_ID=.*$", f"AIRTABLE_BASE_ID={base_id}", text, flags=re.MULTILINE)
    else:
        text = text.rstrip() + f"\nAIRTABLE_BASE_ID={base_id}\n"
    env_path.write_text(text, encoding="utf-8")
    print(f"Wrote AIRTABLE_BASE_ID to {env_path}")


def main() -> None:
    parser = argparse.ArgumentParser(description="Create DiskLordz OS base from JSON schema")
    parser.add_argument("--schema", type=Path, default=DEFAULT_SCHEMA)
    parser.add_argument("--base-name", default=None, help="Override base name from schema")
    parser.add_argument("--token", default=None)
    parser.add_argument("--write-env", type=Path, default=None, help="Append AIRTABLE_BASE_ID to this .env file")
    parser.add_argument("--dry-run", action="store_true")
    args = parser.parse_args()

    schema = json.loads(args.schema.read_text(encoding="utf-8"))
    base_name = args.base_name or schema.get("baseName") or "DiskLordz OS"
    token = load_token(args.token)

    tables = [table_spec_from_schema(t) for t in schema.get("tables") or []]
    if not tables:
        print("No tables in schema", file=sys.stderr)
        sys.exit(1)

    existing = [b for b in list_bases(token) if b.get("name") == base_name]
    if existing:
        base_id = existing[0]["id"]
        print(f"Base already exists: {base_name} ({base_id})")
        if args.write_env:
            write_base_id_to_env(args.write_env, base_id)
        return

    if args.dry_run:
        print(json.dumps({"name": base_name, "tables": tables}, indent=2)[:4000])
        print(f"... ({len(tables)} tables)")
        return

    result = create_base(token, base_name, tables)
    base_id = result["id"]
    print(f"Created base: {base_name}")
    print(f"Base ID: {base_id}")
    print(f"URL: https://airtable.com/{base_id}")
    if args.write_env:
        write_base_id_to_env(args.write_env, base_id)


if __name__ == "__main__":
    main()
