#!/usr/bin/env python3
"""V Voyager Preset Agent — regenerate factory bank and run VoyagerPresetTests."""

from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[3]
PLUGIN = REPO / "plugin-factory" / "plugins" / "moog-voyager"
GENERATOR = PLUGIN / "scripts" / "generate_factory_presets.py"


def regenerate() -> None:
    subprocess.run([sys.executable, str(GENERATOR)], check=True, cwd=str(REPO))


def run_qa() -> None:
    factory = REPO / "plugin-factory"
    build = factory / "build"
    test_bin = build / "plugins" / "moog-voyager" / "VoyagerPresetTests"
    if not test_bin.is_file():
        subprocess.run(
            ["cmake", "--build", str(build), "--target", "VoyagerPresetTests", "MoogVoyager"],
            check=True,
            cwd=str(factory),
        )
    subprocess.run([str(test_bin)], check=True, cwd=str(factory))


def main() -> int:
    parser = argparse.ArgumentParser(description="V Voyager Preset Agent QA")
    parser.add_argument("--regenerate", action="store_true", help="Run generate_factory_presets.py")
    parser.add_argument("--qa", action="store_true", help="Build if needed + VoyagerPresetTests")
    args = parser.parse_args()
    if not args.regenerate and not args.qa:
        args.regenerate = args.qa = True
    if args.regenerate:
        regenerate()
    if args.qa:
        run_qa()
    print("v-voyager-preset-agent: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
