#!/usr/bin/env python3
"""
VST testing command center — drop a .vst3 in my_plugins/, run this script.

On failure, writes error_log.txt for Cursor / Claude Code to read and fix C++.
Uses Tracktion pluginval (strictness 5, in-process). Delegates to the monorepo
orchestrator in scripts/vst/run_pluginval.py for download + parsing.

Usage (from repo root):
  python3 vst-testing-ops/test_runner.py
  python3 vst-testing-ops/test_runner.py --watch
  python3 vst-testing-ops/test_runner.py --install-pluginval
"""

from __future__ import annotations

import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path

OPS_ROOT = Path(__file__).resolve().parent
REPO_ROOT = OPS_ROOT.parent
MY_PLUGINS = OPS_ROOT / "my_plugins"
BIN_DIR = OPS_ROOT / "bin"
ERROR_LOG = OPS_ROOT / "error_log.txt"
ORCHESTRATOR = REPO_ROOT / "scripts" / "vst" / "run_pluginval.py"

DEFAULT_VST_NAME = os.environ.get("VST_NAME", "JD Upgraded.vst3")


def resolve_plugin_path(vst_name: str) -> Path:
    local = MY_PLUGINS / vst_name
    if local.is_dir():
        return local
    candidates = [
        REPO_ROOT / "build" / "JDUpgraded_artefacts" / "Release" / "VST3" / vst_name,
        REPO_ROOT / "build" / "Wave909_artefacts" / "Release" / "VST3" / vst_name,
        REPO_ROOT
        / "MyFirstPlugin"
        / "build"
        / "MyFirstPlugin_artefacts"
        / "Release"
        / "VST3"
        / vst_name,
    ]
    for path in candidates:
        if path.is_dir():
            return path
    return local


def pluginval_in_bin() -> Path | None:
    for name in ("pluginval", "pluginval.exe"):
        candidate = BIN_DIR / name
        if candidate.is_file():
            return candidate
    return None


def orchestrator_env_with_bin() -> dict[str, str]:
    env = os.environ.copy()
    local = pluginval_in_bin()
    if local:
        env["PLUGINVAL_BIN"] = str(local)
    return env


def install_pluginval_to_bin() -> int:
    if not ORCHESTRATOR.is_file():
        print(f"Missing orchestrator: {ORCHESTRATOR}", file=sys.stderr)
        return 2
    proc = subprocess.run(
        [sys.executable, str(ORCHESTRATOR), "--install-only"],
        capture_output=True,
        text=True,
        cwd=str(REPO_ROOT),
        check=False,
    )
    cached = (proc.stdout or "").strip().splitlines()[-1] if proc.stdout else ""
    if proc.returncode != 0 or not cached:
        print(proc.stderr or proc.stdout, file=sys.stderr)
        return proc.returncode or 1
    src = Path(cached)
    if not src.is_file():
        print(f"Install failed: {src}", file=sys.stderr)
        return 1
    BIN_DIR.mkdir(parents=True, exist_ok=True)
    dest = BIN_DIR / src.name
    shutil.copy2(src, dest)
    dest.chmod(0o755)
    print(f"Installed pluginval → {dest}")
    print("Phase 1 complete: bin/ is ready for offline runs.")
    return 0


def run_automated_test(vst_name: str) -> int:
    plugin = resolve_plugin_path(vst_name)

    print(f"Initiating automated stress test for: {vst_name}\n")
    print(f"Plugin bundle: {plugin}\n")

    if not plugin.is_dir():
        print(
            f"Plugin not found. Copy your .vst3 folder into:\n  {MY_PLUGINS}/\n"
            f"Or build the plugin and set VST_NAME, e.g. VST_NAME='JD Upgraded.vst3'.\n",
            file=sys.stderr,
        )
        return 2

    if not ORCHESTRATOR.is_file():
        print(f"Missing orchestrator: {ORCHESTRATOR}", file=sys.stderr)
        return 2

    proc = subprocess.run(
        [
            sys.executable,
            str(ORCHESTRATOR),
            "--plugin",
            str(plugin),
            "--strictness-level",
            "5",
            "--timeout-ms",
            "120000",
            "--validate-in-process",
        ],
        capture_output=True,
        text=True,
        cwd=str(REPO_ROOT),
        env=orchestrator_env_with_bin(),
        check=False,
    )
    combined = (proc.stdout or "") + (proc.stderr or "")
    print(combined, end="" if combined.endswith("\n") else "\n")

    if proc.returncode == 0:
        ERROR_LOG.unlink(missing_ok=True)
        print("SUCCESS: The VST passed all automated stress tests.")
        return 0

    ERROR_LOG.write_text(combined, encoding="utf-8")
    print("FAILED: The VST crashed or flagged an error.")
    print(f"Detailed report saved to '{ERROR_LOG}' (for AI debugging).")
    print(
        "\nSuggested agent prompt:\n"
        f'  "Read {ERROR_LOG} and the plugin source under Source/. '
        'Identify why pluginval failed and apply a minimal fix."\n'
    )
    return proc.returncode or 1


def watch_my_plugins(vst_name: str, poll_seconds: float) -> int:
    if not ORCHESTRATOR.is_file():
        print(f"Missing orchestrator: {ORCHESTRATOR}", file=sys.stderr)
        return 2
    MY_PLUGINS.mkdir(parents=True, exist_ok=True)
    print(f"Watching {MY_PLUGINS} for .vst3 changes (poll {poll_seconds}s). Ctrl+C to stop.")
    return subprocess.call(
        [
            sys.executable,
            str(ORCHESTRATOR),
            "--watch",
            str(MY_PLUGINS),
            "--poll-seconds",
            str(poll_seconds),
            "--strictness-level",
            "5",
            "--timeout-ms",
            "120000",
            "--validate-in-process",
        ],
        cwd=str(REPO_ROOT),
        env=orchestrator_env_with_bin(),
    )


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="VST testing ops command center")
    parser.add_argument(
        "vst_name",
        nargs="?",
        default=None,
        help=f".vst3 bundle name (default: {DEFAULT_VST_NAME} or VST_NAME env)",
    )
    parser.add_argument(
        "--watch",
        action="store_true",
        help="Re-run pluginval when my_plugins/ .vst3 bundles change",
    )
    parser.add_argument("--poll-seconds", type=float, default=2.0)
    parser.add_argument(
        "--install-pluginval",
        action="store_true",
        help="Download pluginval into bin/ (Phase 1 setup)",
    )
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    if args.install_pluginval:
        return install_pluginval_to_bin()
    vst_name = args.vst_name or DEFAULT_VST_NAME
    if args.watch:
        return watch_my_plugins(vst_name, args.poll_seconds)
    return run_automated_test(vst_name)


if __name__ == "__main__":
    raise SystemExit(main())
