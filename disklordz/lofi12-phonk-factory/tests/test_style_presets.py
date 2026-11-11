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
    def test_insanity_pack_preset(self) -> None:
        self.assertIn("memphis_insanity_pack", list_preset_ids())
        ps = get_preset("memphis_insanity_pack")
        self.assertIsNotNone(ps)
        assert ps is not None
        self.assertEqual(ps.lane, "bounce_cowbell")
        self.assertEqual(ps.engine, "hard_dirt_808")

    def test_merge_render_payload_traproom(self) -> None:
        merged = merge_render_payload({"preset": "phonk_trap"})
        self.assertIn("trap", merged["prompt"].lower())
        self.assertEqual(merged["engine"], "traproom_tape")

    def test_api_preset_list(self) -> None:
        self.assertGreaterEqual(len(all_presets_api()), 10)

    def test_display_names_neutral(self) -> None:
        from factory_neutral_policy import find_policy_violations

        for item in all_presets_api():
            blob = f"{item['id']} {item['label']} {item['prompt']}"
            hits = find_policy_violations(blob)
            self.assertEqual(hits, [], msg=f"preset {item['id']}: {hits}")


if __name__ == "__main__":
    unittest.main()
