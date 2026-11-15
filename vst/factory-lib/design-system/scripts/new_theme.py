#!/usr/bin/env python3
"""Create a new theme by copying an existing one (default: the _template starter).

Usage:
    python3 vst/factory-lib/design-system/scripts/new_theme.py acme --name "Acme Audio" --brand "Acme"
    python3 vst/factory-lib/design-system/scripts/new_theme.py acme-dark --from disklordz

The copy passes the checker immediately; then edit themes/<id>/tokens.json and THEME.md
following docs/disklordz/sops/SOP-VF-03-design-system-theme.md.
"""
from __future__ import annotations

import argparse
import json
import shutil
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from check_tokens import THEME_ID, THEMES, check, load_contract  # noqa: E402


def create(theme_id: str, name: str, brand: str, source: str = "_template") -> Path:
    if not THEME_ID.match(theme_id) or theme_id.startswith("_"):
        raise ValueError(f"theme id {theme_id!r} must be lowercase-kebab and not start with '_'")
    src = THEMES / source
    dst = THEMES / theme_id
    if not (src / "tokens.json").exists():
        raise FileNotFoundError(f"source theme {source!r} not found")
    if dst.exists():
        raise FileExistsError(f"theme {theme_id!r} already exists")
    shutil.copytree(src, dst)
    tokens_path = dst / "tokens.json"
    tokens = json.loads(tokens_path.read_text(encoding="utf-8"))
    tokens.update({"id": theme_id, "name": name, "brandName": brand, "version": "0.1.0"})
    tokens.pop("brandSource", None)
    tokens_path.write_text(json.dumps(tokens, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    theme_md = dst / "THEME.md"
    if theme_md.exists():
        text = theme_md.read_text(encoding="utf-8")
        title = text.splitlines()[0] if text else ""
        theme_md.write_text(text.replace(title, f"# {name} theme (`{theme_id}`)", 1), encoding="utf-8")
    errors = check(tokens, load_contract())
    if errors:
        raise RuntimeError("new theme fails the contract:\n" + "\n".join(errors))
    return dst


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("theme_id")
    ap.add_argument("--name", help="display name (default: id)")
    ap.add_argument("--brand", help="brand name shown in the header (default: name)")
    ap.add_argument("--from", dest="source", default="_template", help="theme to copy (default: _template)")
    args = ap.parse_args()
    name = args.name or args.theme_id
    try:
        dst = create(args.theme_id, name, args.brand or name, args.source)
    except (ValueError, FileNotFoundError, FileExistsError, RuntimeError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1
    print(f"created {dst}")
    print("next: edit tokens.json + THEME.md, then run scripts/check_tokens.py and scripts/render_preview.py")
    return 0


if __name__ == "__main__":
    sys.exit(main())
