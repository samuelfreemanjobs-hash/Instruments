#!/usr/bin/env python3
"""Copy latest Trap Lord audition/MPC artifacts into the writable Agent Store."""

from __future__ import annotations

import shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BUILD = ROOT / "build"
ARTIFACTS = Path("/opt/cursor/artifacts")
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


def _newest(paths: list[Path]) -> Path | None:
    existing = [p for p in paths if p.exists()]
    if not existing:
        return None
    return max(existing, key=lambda p: p.stat().st_mtime)


def resolve_source(name: str) -> Path | None:
    candidates = [
        BUILD / name,
        BUILD / "mpc-live-release" / name,
        ARTIFACTS / name,
    ]
    return _newest(candidates)


def main() -> int:
    AGENT_STORE.mkdir(parents=True, exist_ok=True)
    copied = 0
    for name in NAMES:
        src = resolve_source(name)
        if src is None:
            print(f"SKIP missing {name}")
            continue
        dest = AGENT_STORE / name
        shutil.copy2(src, dest)
        print(f"STORE {dest} ({dest.stat().st_size} bytes) <- {src}")
        copied += 1
    if copied == 0:
        raise SystemExit("No artifacts copied to Agent Store")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())