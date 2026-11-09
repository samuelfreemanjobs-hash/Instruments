from __future__ import annotations

import subprocess
import sys
from enum import Enum
from pathlib import Path

from vst_factory.apc_bridge import repo_root


class BuildMode(str, Enum):
    LINUX_COMPILE_ONLY = "linux_compile_only"
    LOCAL_WINDOWS = "local_windows"
    GITHUB_ACTIONS = "github_actions"


def windows_chord_instructions() -> dict[str, str]:
    return {
        "mode": BuildMode.LOCAL_WINDOWS.value,
        "script": "DiskLordz-Audio/build-windows.ps1",
        "ci_workflow": "DiskLordz Audio (Windows VST3)",
        "note": "Greenfield APC plugins use tools/build-vst.sh on Windows too.",
    }


def run_linux_build(plugin_name: str) -> tuple[bool, str]:
    root = repo_root()
    script = root / "tools/build-vst.sh"
    if not script.is_file():
        return False, f"Missing {script}\n"
    juce = root / "vst/JUCE/CMakeLists.txt"
    if not juce.is_file():
        return False, "JUCE submodule missing. Run: git submodule update --init vst/JUCE\n"
    proc = subprocess.run(
        ["bash", str(script), plugin_name],
        cwd=str(root),
        capture_output=True,
        text=True,
        check=False,
        timeout=3600,
    )
    out = (proc.stdout or "") + (proc.stderr or "")
    return proc.returncode == 0, out


def find_vst3_bundle(plugin_name: str) -> Path | None:
    root = repo_root()
    vst_build = root / "vst" / plugin_name / "build"
    if not vst_build.is_dir():
        return None
    for path in sorted(vst_build.rglob("*.vst3")):
        if path.is_dir():
            return path
    return None


def run_factory_qa(plugin_name: str) -> tuple[bool, str]:
    root = repo_root()
    runner = root / "vst-testing-ops/run_business.py"
    if not runner.is_file():
        return False, "Missing vst-testing-ops/run_business.py\n"
    proc = subprocess.run(
        [
            sys.executable,
            str(runner),
            "--profile",
            "factory",
            "--plugin",
            plugin_name,
        ],
        cwd=str(root),
        capture_output=True,
        text=True,
        check=False,
        timeout=3600,
    )
    out = (proc.stdout or "") + (proc.stderr or "")
    return proc.returncode == 0, out


def run_factory_audio_check(plugin_name: str) -> tuple[bool, str]:
    root = repo_root()
    runner = root / "vst-testing-ops/run_business.py"
    if not runner.is_file():
        return True, "skip: no runner\n"
    proc = subprocess.run(
        [
            sys.executable,
            str(runner),
            "--profile",
            "factory-audio",
            "--plugin",
            plugin_name,
        ],
        cwd=str(root),
        capture_output=True,
        text=True,
        check=False,
        timeout=600,
    )
    out = (proc.stdout or "") + (proc.stderr or "")
    if proc.returncode == 0:
        return True, out
    return False, out
