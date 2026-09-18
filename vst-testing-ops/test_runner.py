#!/usr/bin/env python3
"""
VST testing command center — drop a .vst3 in my_plugins/, run this script.

On failure, writes error_log.txt for Cursor / Claude Code to read and fix C++.
Uses Tracktion pluginval (strictness 5, in-process). Delegates to the monorepo
orchestrator in scripts/vst/run_pluginval.py for download + parsing.
"""

from __future__ import annotations

import os
import subprocess
import sys
from pathlib import Path

OPS_ROOT = Path(__file__).resolve().parent
REPO_ROOT = OPS_ROOT.parent
MY_PLUGINS = OPS_ROOT / "my_plugins"
BIN_DIR = OPS_ROOT / "bin"
ERROR_LOG = OPS_ROOT / "error_log.txt"
ORCHESTRATOR = REPO_ROOT / "scripts" / "vst" / "run_pluginval.py"

# Override with env: VST_NAME=MyPlugin.vst3
DEFAULT_VST_NAME = os.environ.get("VST_NAME", "JD Upgraded.vst3")


def resolve_plugin_path(vst_name: str) -> Path:
    local = MY_PLUGINS / vst_name
    if local.is_dir():
        return local
    # Convenience: monorepo Release artefacts without copying
    candidates = [
        REPO_ROOT
        / "build"
        / "JDUpgraded_artefacts"
        / "Release"
        / "VST3"
        / vst_name,
        REPO_ROOT
        / "build"
        / "Wave909_artefacts"
        / "Release"
        / "VST3"
        / vst_name,
        REPO_ROOT / "MyFirstPlugin" / "build" / "MyFirstPlugin_artefacts" / "Release" / "VST3" / vst_name,
    ]
    for path in candidates:
        if path.is_dir():
            return path
    return local


def ensure_pluginval_in_bin() -> Path:
    """Prefer ./bin/pluginval; otherwise let the orchestrator cache under build/tools."""
    for name in ("pluginval", "pluginval.exe"):
        candidate = BIN_DIR / name
        if candidate.is_file():
            os.environ.setdefault("PLUGINVAL_BIN", str(candidate))
            return candidate
    return BIN_DIR / "pluginval"


def run_automated_test(vst_name: str | None = None) -> int:
    vst_name = vst_name or DEFAULT_VST_NAME
    plugin = resolve_plugin_path(vst_name)

    print(f"Initiating automated stress test for: {vst_name}\n")
    print(f"Plugin bundle: {plugin}\n")

    if not plugin.is_dir():
        print(
            f"Plugin not found. Copy your .vst3 folder into:\n  {MY_PLUGINS}/\n"
            f"Or build JD Upgraded and set VST_NAME='JD Upgraded.vst3'.\n",
            file=sys.stderr,
        )
        return 2

    ensure_pluginval_in_bin()

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
    print(f"Detailed report saved to '{ERROR_LOG.name}' (for AI debugging).")
    print(
        "\nSuggested agent prompt:\n"
        f'  "Read {ERROR_LOG} and the plugin source under Source/. '
        'Explain why pluginval failed and apply a minimal fix."\n'
    )
    return proc.returncode or 1


if __name__ == "__main__":
    name = sys.argv[1] if len(sys.argv) > 1 else None
    raise SystemExit(run_automated_test(name))
