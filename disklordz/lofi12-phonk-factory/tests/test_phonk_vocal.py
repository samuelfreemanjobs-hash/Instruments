"""Synthetic phonk vocal factory tests."""

from __future__ import annotations

import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

from phonk_vocal_synth import render_vocal_chop_pack, render_vocal_loop  # noqa: E402


class PhonkVocalTests(unittest.TestCase):
    def test_vocal_loop_length(self) -> None:
        bpm = 84.0
        bars = 2
        pcm, meta = render_vocal_loop(
            prompt="dj paul memphis screw vocal", bpm=bpm, bars=bars, variation=0
        )
        expected = (60.0 / bpm) * 4 * bars
        self.assertAlmostEqual(len(pcm) / 44100, expected, delta=0.05)
        self.assertEqual(meta["type"], "synthetic_vocal_texture")

    def test_chop_pack_keys(self) -> None:
        pack = render_vocal_chop_pack("phonk dark", 0, None)
        self.assertIn("chop_uh", pack)
        self.assertTrue(all(len(v) > 100 for v in pack.values()))


if __name__ == "__main__":
    unittest.main()
