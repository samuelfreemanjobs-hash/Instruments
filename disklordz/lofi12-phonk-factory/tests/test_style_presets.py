"""Style preset tests."""

from __future__ import annotations

import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

from phonk_style_presets import (  # noqa: E402
    all_presets_api,
    get_preset,
    list_preset_ids,
    merge_render_payload,
)


class StylePresetTests(unittest.TestCase):
    def test_trinity_preset_exists(self) -> None:
        self.assertIn("memphis_trinity", list_preset_ids())
        ps = get_preset("memphis_trinity")
        self.assertIsNotNone(ps)
        assert ps is not None
        self.assertEqual(ps.lane, "juicy_j")
        self.assertEqual(ps.engine, "juicy_j")

    def test_merge_render_payload(self) -> None:
        merged = merge_render_payload({"preset": "dj_toomp"})
        self.assertIn("dj toomp", merged["prompt"].lower())
        self.assertEqual(merged["engine"], "dj_toomp")

    def test_api_preset_list(self) -> None:
        self.assertGreaterEqual(len(all_presets_api()), 4)

    def test_display_names_no_artists(self) -> None:
        ps = get_preset("memphis_trinity")
        assert ps is not None
        self.assertEqual(ps.label, "Memphis Insanity")
        for item in all_presets_api():
            label = item["label"].lower()
            self.assertNotIn("juicy", label)
            self.assertNotIn("dj paul", label)
            self.assertNotIn("toomp", label)


if __name__ == "__main__":
    unittest.main()
