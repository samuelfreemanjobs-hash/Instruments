"""Cross-platform paths for VstiOneShotRender."""

from __future__ import annotations

import os
from pathlib import Path


def resolve_render_bin(repo_root: Path, override: Path | None = None) -> Path:
    if override is not None:
        return override

    env = os.environ.get("VSTI_ONE_SHOT_RENDER_BIN")
    if env:
        return Path(env)

    names = ("VstiOneShotRender.exe", "VstiOneShotRender") if os.name == "nt" else ("VstiOneShotRender",)
    subdirs = ("", "Release", "Debug", "RelWithDebInfo")

    for sub in subdirs:
        for name in names:
            candidate = repo_root / "build" / sub / name if sub else repo_root / "build" / name
            if candidate.is_file():
                return candidate

    # Expected location for error messages (Windows VS multi-config)
    if os.name == "nt":
        return repo_root / "build" / "Release" / "VstiOneShotRender.exe"
    return repo_root / "build" / "VstiOneShotRender"


def default_vst3_search_hints() -> list[str]:
    if os.name != "nt":
        return []
    return [
        r"C:\Program Files\Common Files\VST3",
        r"C:\Program Files\Native Instruments\VSTPlugins 64 bit",
    ]
