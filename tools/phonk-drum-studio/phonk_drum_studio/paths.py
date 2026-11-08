"""Resolve app, factory, and stub paths for dev and PyInstaller bundles."""

from __future__ import annotations

import sys
from functools import lru_cache
from pathlib import Path


def is_frozen() -> bool:
    return bool(getattr(sys, "frozen", False))


@lru_cache(maxsize=1)
def app_root() -> Path:
    """Directory containing application resources (bundle root when frozen)."""
    if is_frozen():
        return Path(getattr(sys, "_MEIPASS"))  # type: ignore[arg-type]
    return Path(__file__).resolve().parent


@lru_cache(maxsize=1)
def project_root() -> Path:
    """tools/phonk-drum-studio (writable cwd for checkpoints in dev)."""
    if is_frozen():
        return Path.home() / "PhonkDrumStudio"
    return app_root().parent


@lru_cache(maxsize=1)
def factory_root() -> Path:
    """disklordz/sound-factory or bundled copy inside the desktop build."""
    bundled = app_root() / "sound-factory"
    if (bundled / "kit_from_samples.py").is_file():
        return bundled
    monorepo = app_root().parent.parent.parent / "disklordz" / "sound-factory"
    if (monorepo / "kit_from_samples.py").is_file():
        return monorepo
    return bundled


@lru_cache(maxsize=1)
def stub_samples_root() -> Path:
    """Website public sample stubs for --fill-stubs."""
    bundled = app_root() / "factory-stubs" / "samples"
    if bundled.is_dir():
        return bundled
    monorepo = app_root().parent.parent.parent / "disklordz" / "website" / "public" / "samples"
    if monorepo.is_dir():
        return monorepo
    return bundled


def ensure_user_dirs() -> None:
    """Create per-user data dir when running as a frozen desktop app."""
    if not is_frozen():
        return
    root = project_root()
    root.mkdir(parents=True, exist_ok=True)
    (root / "kits").mkdir(exist_ok=True)
    (root / "exports").mkdir(exist_ok=True)
