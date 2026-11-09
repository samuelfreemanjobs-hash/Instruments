"""Contract: every relative markdown link in DISKLORDZ_VST_FACTORY_PLATFORM.md resolves."""

from __future__ import annotations

import re
from pathlib import Path

import pytest

PLATFORM_DOC = Path(__file__).resolve().parents[3] / "docs" / "DISKLORDZ_VST_FACTORY_PLATFORM.md"
LINK_RE = re.compile(r"\[([^\]]+)\]\(([^)]+)\)")


def _repo_root() -> Path:
    return Path(__file__).resolve().parents[3]


def test_platform_doc_relative_links_resolve() -> None:
    """Contract: docs/DISKLORDZ_VST_FACTORY_PLATFORM.md relative targets exist on disk."""
    assert PLATFORM_DOC.is_file(), f"missing {PLATFORM_DOC}"
    text = PLATFORM_DOC.read_text(encoding="utf-8")
    docs_dir = PLATFORM_DOC.parent
    missing: list[str] = []
    checked = 0
    for _label, href in LINK_RE.findall(text):
        if href.startswith(("http://", "https://", "mailto:", "#")):
            continue
        # Strip optional anchors
        path_part = href.split("#", 1)[0]
        if not path_part:
            continue
        target = (docs_dir / path_part).resolve()
        checked += 1
        if not target.exists():
            missing.append(f"{href} → {target}")
    assert checked > 0
    assert not missing, "Broken PLATFORM.md links:\n" + "\n".join(missing)


def test_standards_and_knowledge_artifacts_exist() -> None:
    """Contract: platform-indexed standards, daw-matrix, features, and dsp knowledge exist."""
    root = _repo_root() / "disklordz" / "vst-factory"
    required = [
        root / "standards" / "ENGINEERING.md",
        root / "standards" / "FORMATS.md",
        root / "standards" / "MEMORY.md",
        root / "standards" / "COMMERCIAL_READINESS.md",
        root / "standards" / "OPERATING_MODES.md",
        root / "testing" / "daw-matrix.schema.json",
        root / "testing" / "daw-matrix.example.json",
        root / "modules" / "features.json",
        root / "modules" / "workflows" / "full-lifecycle.json",
        root / "knowledge" / "dsp" / "README.md",
        root / "knowledge" / "dsp" / "index.json",
        root / "gui" / "README.md",
    ]
    missing = [str(p) for p in required if not p.exists()]
    assert not missing, "Missing factory artifacts:\n" + "\n".join(missing)
