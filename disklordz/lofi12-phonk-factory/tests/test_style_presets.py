"""Style preset tests."""

from __future__ import annotations

import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

from phonk_style_presets import get_preset, list_preset_ids  # noqa: E402


class StylePresetTests(unittest.TestCase):
    def test_trinity_preset_exists(self) -> None:
        self.assertIn("memphis_trinity", list_preset_ids())
        ps = get_preset("memphis_trinity")
        self.assertIsNotNone(ps)
        assert ps is not None
        self.assertEqual(ps.lane, "juicy_j")
        self.assertEqual(ps.engine, "juicy_j")


if __name__ == "__main__":
    unittest.main()
