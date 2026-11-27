"""Machine engine resolver tests."""

from __future__ import annotations

import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

from phonk_machine_engine import ENGINE_IDS, render_machine_voice, resolve_engine_id  # noqa: E402
from phonk_synth import resolve_phonk_params  # noqa: E402


class MachineEngineTests(unittest.TestCase):
    def test_resolve_from_prompt_and_override(self) -> None:
        self.assertEqual(resolve_engine_id("dirty memphis hard dirt"), "hard_dirt_808")
        self.assertEqual(resolve_engine_id("traproom sp 1200"), "traproom_tape")
        self.assertEqual(resolve_engine_id("", override="tape_screw"), "tape_slowdown")
        self.assertEqual(resolve_engine_id("pure tr909 beat"), "tr909")
        self.assertEqual(resolve_engine_id("boss dr660"), "boss_dr660")
        self.assertEqual(resolve_engine_id("splice tape pack"), "mr_tape")

    def test_engine_ids_complete(self) -> None:
        self.assertIn("hard_dirt_808", ENGINE_IDS)
        self.assertIn("traproom_tape", ENGINE_IDS)

    def test_render_kick(self) -> None:
        params = resolve_phonk_params("memphis phonk", 1)
        wav = render_machine_voice("hard_dirt_808", "kick_808", params)
        self.assertGreater(len(wav), 1000)


if __name__ == "__main__":
    unittest.main()
