"""Stem / per-instrument loop export tests."""

from __future__ import annotations

import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

from phonk_loop_fx import LoopFxParams  # noqa: E402
from phonk_loop_render import render_phonk_loop  # noqa: E402
from phonk_pattern_render import render_pattern_stem  # noqa: E402


class PhonkStemTests(unittest.TestCase):
    def test_factory_kick_stem(self) -> None:
        full, _ = render_phonk_loop(
            prompt="juicy j memphis 86", bpm=86, bars=2, variation=3
        )
        kick, meta = render_phonk_loop(
            prompt="juicy j memphis 86",
            bpm=86,
            bars=2,
            variation=3,
            stem="kick",
        )
        self.assertEqual(meta["stem"], "kick")
        self.assertNotEqual(full[:4000], kick[:4000])

    def test_pattern_stem(self) -> None:
        pattern = {
            "bpm": 86,
            "tracks": [
                {
                    "name": "Kick",
                    "defaultNote": 36,
                    "steps": [{"on": True, "note": 36, "velocity": 110}]
                    + [{"on": False, "note": 36, "velocity": 100}] * 15,
                }
            ],
        }
        pcm, meta = render_pattern_stem(pattern, 0, fx=LoopFxParams(bitcrush=0.3))
        self.assertEqual(meta["stem"], "kick")
        self.assertGreater(len(pcm), 1000)


if __name__ == "__main__":
    unittest.main()
