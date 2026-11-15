from __future__ import annotations

import platform
import shutil
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
    """Run factory-audio profile (skip-ok when no WAV fixture)."""
    root = repo_root()
    runner = root / "vst-testing-ops/run_business.py"
    if not runner.is_file():
        return False, "Missing vst-testing-ops/run_business.py\n"
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
    return proc.returncode == 0, out


def find_release_artifact(plugin_name: str) -> Path | None:
    """Return newest zip (or staging dir) under release/<Name>/."""
    root = repo_root()
    release_dir = root / "release" / plugin_name
    if not release_dir.is_dir():
        return None
    zips = sorted(release_dir.glob("*.zip"), key=lambda p: p.stat().st_mtime, reverse=True)
    if zips:
        return zips[0]
    staging = release_dir / "staging"
    if staging.is_dir():
        return staging
    return None


def run_package_ship(plugin_name: str, version: str = "1.0.0") -> tuple[str, bool, str]:
    """Run tools/package-vst.sh.

    Returns (status, ok, output) where status is ``ok``, ``skipped``, or ``failed``.
    Non-Linux hosts or missing toolchain produce ``skipped`` (not a crash).
    """
    root = repo_root()
    script = root / "tools" / "package-vst.sh"
    system = platform.system()
    if system != "Linux":
        msg = f"skipped: ship packaging requires Linux (host={system})\n"
        return "skipped", True, msg
    if not script.is_file():
        return "skipped", True, f"skipped: missing {script}\n"
    if shutil.which("bash") is None:
        return "skipped", True, "skipped: bash not found on PATH\n"
    if shutil.which("cmake") is None and not (root / "vst" / plugin_name / "build").is_dir():
        return "skipped", True, "skipped: cmake missing and no existing vst build tree\n"
    if shutil.which("zip") is None:
        return "skipped", True, "skipped: zip utility not found on PATH\n"
    try:
        proc = subprocess.run(
            ["bash", str(script), plugin_name, version],
            cwd=str(root),
            capture_output=True,
            text=True,
            check=False,
            timeout=3600,
        )
    except subprocess.TimeoutExpired as exc:
        return "failed", False, f"package-vst timed out: {exc}\n"
    except OSError as exc:
        return "skipped", True, f"skipped: could not invoke package-vst ({exc})\n"
    out = (proc.stdout or "") + (proc.stderr or "")
    if proc.returncode != 0:
        return "failed", False, out
    return "ok", True, out
