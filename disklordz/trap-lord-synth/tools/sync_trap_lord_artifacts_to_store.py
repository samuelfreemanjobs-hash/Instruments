#!/usr/bin/env python3
"""Copy latest Trap Lord audition/MPC artifacts into the writable Agent Store."""

from __future__ import annotations

import shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BUILD = ROOT / "build"
AGENT_STORE = Path("/cursor/stores/self/trap-lord-artifacts")

NAMES = (
    "trap-lord-synth-auditions-24.zip",
    "trap-lord-synth-auditions-metrics.csv",
    "trap-lord-mpc-live-pad-c1-c3.zip",
    "trap-lord-mpc-live-pad-c4-c5.zip",
    "trap-lord-mpc-live-pad-c6-c7.zip",
    "trap-lord-mpc-live-swell-c1-c3.zip",
    "trap-lord-mpc-live-swell-c4-c5.zip",
    "trap-lord-mpc-live-swell-c6-c7.zip",
    "trap-lord-mpc-live-brass.zip",
)


def main() -> int:
    AGENT_STORE.mkdir(parents=True, exist_ok=True)
    copied = 0
    for name in NAMES:
        src = BUILD / name
        if not src.exists():
            alt = BUILD / "mpc-live-release" / name
            src = alt if alt.exists() else src
        if not src.exists():
            print(f"SKIP missing {name}")
            continue
        dest = AGENT_STORE / name
        shutil.copy2(src, dest)
        print(f"STORE {dest} ({dest.stat().st_size} bytes)")
        copied += 1
    if copied == 0:
        raise SystemExit("No artifacts copied to Agent Store")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())