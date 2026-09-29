#!/usr/bin/env python3
"""Audio Plugin Coder (APC) fleet agent — prints bootstrap checklist."""

from __future__ import annotations

import argparse
from pathlib import Path

REPO = Path(__file__).resolve().parents[3]
BOOTSTRAP = REPO / "docs" / "JUCE_APC_AGENT_BOOTSTRAP.md"


def main() -> int:
    parser = argparse.ArgumentParser(description="APC / JUCE agent bootstrap")
    parser.add_argument("--self-test", action="store_true")
    args = parser.parse_args()
    if not BOOTSTRAP.is_file():
        raise SystemExit(f"missing {BOOTSTRAP}")
    if args.self_test:
        print("audio-plugin-coder: ok", BOOTSTRAP)
        return 0
    print(BOOTSTRAP.read_text(encoding="utf-8")[:4000])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
