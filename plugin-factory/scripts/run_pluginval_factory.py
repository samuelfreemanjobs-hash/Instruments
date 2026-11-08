#!/usr/bin/env python3
"""Run pluginval on every registered Plugin Factory VST3 bundle."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

FACTORY_ROOT = Path(__file__).resolve().parents[1]
REPO_ROOT = FACTORY_ROOT.parent
ORCHESTRATOR = REPO_ROOT / "scripts" / "vst" / "run_pluginval.py"
FACTORY_OPS = FACTORY_ROOT / "scripts" / "factory_ops.py"


def main() -> int:
    proc = subprocess.run(
        [sys.executable, str(FACTORY_OPS), "discover"],
        capture_output=True,
        text=True,
        check=False,
    )
    if proc.returncode != 0:
        sys.stderr.write(proc.stderr or proc.stdout)
        return proc.returncode

    bundles = [line.strip() for line in proc.stdout.splitlines() if line.strip()]
    if not bundles:
        print("No factory VST3 bundles to validate.", file=sys.stderr)
        return 2

    cmd = [sys.executable, str(ORCHESTRATOR)]
    for bundle in bundles:
        cmd.extend(["--plugin", bundle])
    return subprocess.run(cmd, check=False).returncode


if __name__ == "__main__":
    raise SystemExit(main())
