#!/usr/bin/env python3
from __future__ import annotations

import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT))

from retro_arranger.midi_writer import export_stems_to_dir  # noqa: E402
from retro_arranger.pipeline import compose  # noqa: E402


def main() -> int:
    result = compose()
    assert result.spec.total_bars == 40
    assert len(result.stems) == 7
    with tempfile.TemporaryDirectory() as td:
        paths = export_stems_to_dir(result, Path(td))
        assert "01_Drums.mid" in paths
        assert paths["01_Drums.mid"].stat().st_size > 100
        assert paths["Full_Arrangement_Multitrack.mid"].stat().st_size > 200
    print("retro-arranger smoke: ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
