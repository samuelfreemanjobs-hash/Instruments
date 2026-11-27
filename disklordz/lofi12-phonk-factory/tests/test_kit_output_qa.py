"""Kit folder QA scanner (peaks, naming, non-zero WAVs)."""

from __future__ import annotations

import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

from drum_kit_builder import build_kit  # noqa: E402
from kit_output_qa import scan_kit  # noqa: E402


class KitOutputQaTests(unittest.TestCase):
    def test_demo_kit_passes_qa(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            out = Path(tmp) / "kit"
            build_kit(mode_id="occult_tape", out_dir=out, kit_size="demo", seed=3)
            issues = scan_kit(out)
            self.assertEqual(issues, [], msg=[f"{i.path}: {i.message}" for i in issues])


if __name__ == "__main__":
    unittest.main()