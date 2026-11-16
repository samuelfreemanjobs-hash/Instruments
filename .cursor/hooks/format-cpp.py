#!/usr/bin/env python3
"""Format drum-engine C++ after an agent edit. Other products keep their own style."""

from __future__ import annotations

import json
import shutil
import subprocess
import sys
from pathlib import Path


def main() -> int:
    raw = sys.stdin.read()
    try:
        payload = json.loads(raw) if raw.strip() else {}
    except json.JSONDecodeError:
        return 0
    file_path = str(payload.get("file_path") or "")
    path = Path(file_path)
    if path.suffix not in {".cpp", ".h", ".hpp"}:
        return 0
    if "disklordz/drum-engine" not in file_path.replace("\\", "/"):
        return 0
    clang_format = shutil.which("clang-format")
    if clang_format is None or not path.is_file():
        return 0
    subprocess.run([clang_format, "-i", str(path)], check=False)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
