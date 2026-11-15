#!/usr/bin/env python3
"""Single tick for DiskLordz Audio plugin agent lane (CI + local)."""

from __future__ import annotations

import argparse
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]


def run(cmd: list[str] | str, *, check: bool = False) -> int:
    if isinstance(cmd, str):
        return subprocess.run(cmd, shell=True, cwd=REPO, check=check).returncode
    return subprocess.run(cmd, cwd=REPO, check=check).returncode


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--weekly", action="store_true", help="Run weekly_review.py")
    parser.add_argument("--antigravity", action="store_true", help="sync-all-antigravity")
    parser.add_argument("--push", action="store_true")
    parser.add_argument("--golden-dsp", action="store_true", help="run_business.py --profile dsp-only")
    args = parser.parse_args()

    print("=== audio-plugin fleet_tick ===", datetime.now(timezone.utc).isoformat())

    rc = run([sys.executable, str(REPO / "scripts/audio-plugin/factory-queue.py")])
    run([sys.executable, str(REPO / "disklordz/audio-command/scripts/coach_outreach.py")])

    if args.weekly:
        run([sys.executable, str(REPO / "disklordz/audio-command/scripts/weekly_review.py")])

    if args.antigravity:
        sh = REPO / "scripts/antigravity-bridge/sync-all-antigravity.sh"
        cmd = f"{sh} {'--push' if args.push else ''}".strip()
        run(cmd)

    if args.golden_dsp:
        rb = REPO / "vst-testing-ops/run_business.py"
        if rb.is_file():
            run([sys.executable, str(rb), "--profile", "dsp-only"])
        else:
            print("skip golden_dsp: no run_business.py")

    run([sys.executable, str(REPO / "scripts/autonomous-coder/tick.py"), "--dry-run"])
    return rc


if __name__ == "__main__":
    raise SystemExit(main())
