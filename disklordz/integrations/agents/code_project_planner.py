#!/usr/bin/env python3
"""Code Project Planner — emit PRD template path and validation hints."""

from __future__ import annotations

import argparse
from pathlib import Path

REPO = Path(__file__).resolve().parents[3]
TEMPLATE = REPO / "docs" / "templates" / "PRD_TEMPLATE.md"
MEMORY = REPO / "docs" / "COMPANY_MEMORY_INDEX.md"


def main() -> int:
    parser = argparse.ArgumentParser(description="PRD planner agent helper")
    parser.add_argument("--product", default="unnamed", help="Product slug for PRD filename hint")
    parser.add_argument("--self-test", action="store_true")
    args = parser.parse_args()
    if args.self_test:
        assert TEMPLATE.is_file() and MEMORY.is_file()
        print("code-project-planner: ok")
        return 0
    out = REPO / "docs" / "products" / f"{args.product.upper()}_PRD.md"
    print(f"Template: {TEMPLATE}")
    print(f"Suggested output: {out}")
    print(f"Company index: {MEMORY}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
