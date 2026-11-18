#!/usr/bin/env python3
"""Generate + package the full Black Box Collection (all eras + Dirty Circuits)."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS = Path(__file__).resolve().parent
ERAS = ("bb93", "bb96", "bb98", "bb2000")


def run(args: list[str]) -> None:
    print("+", " ".join(args), flush=True)
    subprocess.check_call(args, cwd=str(ROOT))


def main() -> None:
    summary = {}
    for era in ERAS:
        run([sys.executable, str(SCRIPTS / "generate_era.py"), era])
        run([sys.executable, str(SCRIPTS / "package_era.py"), era])
        summary[era] = "ok"
    run([sys.executable, str(SCRIPTS / "package_dirty_circuits.py")])
    summary["dirty_circuits"] = "ok"
    out = Path("/opt/cursor/artifacts/black_box_collection_build.json")
    out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))
    print("artifacts:", list(Path("/opt/cursor/artifacts").glob("Black_Box_*.zip")))
    print("artifacts:", list(Path("/opt/cursor/artifacts").glob("Dirty_*.zip")))


if __name__ == "__main__":
    main()
