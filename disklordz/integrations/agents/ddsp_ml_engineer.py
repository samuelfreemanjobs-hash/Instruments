#!/usr/bin/env python3
"""DDSP / ML engineer agent — run drum blueprint smoke."""

from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path

BLUEPRINT = Path(__file__).resolve().parents[3] / "tools" / "drum-synth-blueprint"


def main() -> int:
    parser = argparse.ArgumentParser(description="DDSP / ML engineer verification")
    parser.add_argument("--self-test", action="store_true")
    args = parser.parse_args()
    if not BLUEPRINT.is_dir():
        raise SystemExit(f"missing {BLUEPRINT}")
    if args.self_test:
        subprocess.run(
            [sys.executable, "-m", "pytest", "tests", "-q"],
            cwd=BLUEPRINT,
            check=True,
        )
        subprocess.run(
            [sys.executable, str(BLUEPRINT / "drum_synth_blueprint" / "synth_808_generator.py")],
            cwd=BLUEPRINT,
            check=True,
        )
        train_script = BLUEPRINT / "scripts" / "train_ddsp_808.py"
        if train_script.is_file():
            try:
                import torch  # noqa: F401
            except ImportError:
                print("ddsp-ml-engineer: torch not installed; skip train smoke")
            else:
                subprocess.run(
                    [
                        sys.executable,
                        str(train_script),
                        "--steps",
                        "12",
                        "--export",
                        str(BLUEPRINT / "artifacts" / "ddsp_808_encoder.onnx"),
                    ],
                    cwd=BLUEPRINT,
                    check=True,
                )
        print("ddsp-ml-engineer: ok")
        return 0
    print(f"Run: cd {BLUEPRINT} && pytest tests -v")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
