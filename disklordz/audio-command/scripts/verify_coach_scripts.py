#!/usr/bin/env python3
"""Smoke: py_compile + import coach_engine/coach_outreach from repo root."""

from __future__ import annotations

import importlib.util
import py_compile
import sys
from pathlib import Path

SCRIPTS = Path(__file__).resolve().parent
ROOT = SCRIPTS.parents[2]


def main() -> int:
    errors = 0
    for py in sorted(SCRIPTS.glob("*.py")):
        try:
            py_compile.compile(str(py), doraise=True)
        except py_compile.PyCompileError as exc:
            print(f"compile fail: {py.name}: {exc}", file=sys.stderr)
            errors += 1

    for name in ("coach_engine", "coach_outreach"):
        path = SCRIPTS / f"{name}.py"
        spec = importlib.util.spec_from_file_location(name, path)
        if not spec or not spec.loader:
            print(f"import spec fail: {name}", file=sys.stderr)
            errors += 1
            continue
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)

    if errors:
        return 1
    print("coach scripts OK", SCRIPTS)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
