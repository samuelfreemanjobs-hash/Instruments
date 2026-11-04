#!/usr/bin/env python3
"""DSP & VST Factory Architect — repo sanity + optional fast QA for agent/CI."""

from __future__ import annotations

import argparse
import json
import re
import shutil
import subprocess
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[3]

REQUIRED_DOCS = (
    "ARCHITECTURE.md",
    "docs/ARCHITECTURE.md",
    "vst-testing-ops/ARCHITECTURE.md",
    "SP1200/ARCHITECTURE.md",
    "Wave9090/ARCHITECTURE.md",
)

PLUGIN_TARGET_HINTS = (
    "JDUpgraded_VST3",
    "Wave9090_VST3",
    "SP1200_VST3",
    "JDUpgraded_Standalone",
)


def _read_cmake_plugin_targets() -> list[str]:
    cmake = REPO / "CMakeLists.txt"
    if not cmake.is_file():
        return []
    text = cmake.read_text(encoding="utf-8", errors="replace")
    found: list[str] = []
    for hint in PLUGIN_TARGET_HINTS:
        if hint in text:
            found.append(hint)
    root = REPO / "CMakeLists.txt"
    scan_paths = [root] if root.is_file() else []
    scan_paths.extend(REPO.glob("*/CMakeLists.txt"))
    for path in scan_paths:
        chunk = path.read_text(encoding="utf-8", errors="replace")
        for match in re.finditer(r"juce_add_plugin\s*\(\s*(\w+)", chunk):
            name = match.group(1)
            if name not in found:
                found.append(name)
    return found


def self_test(*, quick: bool = True) -> dict[str, object]:
    missing_docs = [p for p in REQUIRED_DOCS if not (REPO / p).is_file()]
    targets = _read_cmake_plugin_targets()
    toolchain = {
        "cmake": shutil.which("cmake"),
        "g++12": shutil.which("g++-12") or shutil.which("g++"),
        "python3": shutil.which("python3"),
    }
    agent_entry = REPO / "disklordz/agents/profit/dsp-vst-architect/agent.md"
    ok = not missing_docs and bool(targets) and agent_entry.is_file()
    result: dict[str, object] = {
        "ok": ok,
        "repo": str(REPO),
        "missing_docs": missing_docs,
        "plugin_targets": targets,
        "toolchain": toolchain,
        "agent_entry": str(agent_entry.relative_to(REPO)) if agent_entry.is_file() else None,
    }
    if quick or not ok:
        return result

    pipeline = REPO / "vst-testing-ops/run_business.py"
    if pipeline.is_file():
        proc = subprocess.run(
            [sys.executable, str(pipeline), "--profile", "dsp-only"],
            cwd=REPO,
            capture_output=True,
            text=True,
            timeout=3600,
        )
        result["dsp_only_exit"] = proc.returncode
        result["dsp_only_tail"] = (proc.stderr or proc.stdout)[-2000:]
        result["ok"] = ok and proc.returncode == 0
    return result


def print_playbook() -> None:
    print(
        json.dumps(
            {
                "read_first": [
                    "ARCHITECTURE.md",
                    "docs/ARCHITECTURE.md",
                    "vst-testing-ops/ARCHITECTURE.md",
                    "disklordz/agents/profit/dsp-vst-architect/agent.md",
                ],
                "build": [
                    "cmake -B build -DCMAKE_BUILD_TYPE=Release "
                    "-DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12",
                    "cmake --build build -j --target JDUpgraded_VST3 JDUpgraded_Standalone",
                ],
                "verify": [
                    "python3 vst-testing-ops/run_business.py --profile ci",
                    "python3 vst-testing-ops/run_business.py --profile dsp-only",
                    "ctest --test-dir build -R SP1200 --output-on-failure",
                ],
                "golden_refresh": "tests/golden/refresh_golden.sh",
            },
            indent=2,
        )
    )


def main() -> int:
    parser = argparse.ArgumentParser(description="DSP & VST Factory Architect CLI")
    parser.add_argument("--self-test", action="store_true", help="Fast repo + target sanity (default)")
    parser.add_argument(
        "--self-test-full",
        action="store_true",
        help="Sanity then run vst-testing-ops --profile dsp-only (slow)",
    )
    parser.add_argument("--playbook", action="store_true", help="JSON command playbook for agents")
    parser.add_argument("--list-targets", action="store_true", help="List detected JUCE plugin targets")
    args = parser.parse_args()

    if args.playbook:
        print_playbook()
        return 0

    if args.list_targets:
        print(json.dumps(_read_cmake_plugin_targets(), indent=2))
        return 0

    if args.self_test_full:
        out = self_test(quick=False)
        print(json.dumps(out, indent=2))
        return 0 if out.get("ok") else 1

    if args.self_test or len(sys.argv) == 1:
        out = self_test(quick=True)
        print(json.dumps(out, indent=2))
        return 0 if out.get("ok") else 1

    parser.print_help()
    return 2


if __name__ == "__main__":
    raise SystemExit(main())
