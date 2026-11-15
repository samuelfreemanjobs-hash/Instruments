"""Contract: every lifecycle stage points at a real SOP document."""
from __future__ import annotations

import json
from pathlib import Path

import pytest

REPO = Path(__file__).resolve().parents[3]
WORKFLOW = REPO / "disklordz" / "vst-factory" / "modules" / "workflows" / "full-lifecycle.json"
SOPS = REPO / "docs" / "disklordz" / "sops"


def test_every_stage_sop_file_exists():
    if not (SOPS / "SOP-VF-00-vst-factory-master.md").exists():
        pytest.skip("SOP-VF library not on this branch yet (lands with the design-system/SOP PR)")
    stages = json.loads(WORKFLOW.read_text(encoding="utf-8"))["stages"]
    missing = [s["id"] for s in stages if not list(SOPS.glob(f"{s['sop']}-*.md"))]
    assert missing == [], f"stages without an SOP file: {missing}"
