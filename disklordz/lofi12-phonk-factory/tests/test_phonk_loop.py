"""Memphis phonk loop factory tests."""

from __future__ import annotations

import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

from phonk_groove import build_bar, max_hats_per_bar, validate_bpm  # noqa: E402
from phonk_loop_render import render_phonk_loop  # noqa: E402
from phonk_loop_factory import parse_bpm_from_prompt  # noqa: E402
from phonk_memphis_lanes import resolve_memphis_lane  # noqa: E402


class PhonkLoopTests(unittest.TestCase):
    def test_bpm_clamp(self) -> None:
        self.assertEqual(validate_bpm(40), 60.0)
        self.assertEqual(validate_bpm(220), 190.0)

    def test_high_bpm_sparse_hats(self) -> None:
        bar = build_bar(0, bpm=170, seed=12345, variation=0)
        hats = [h for h in bar.hits if h.instrument == "hat"]
        self.assertLessEqual(len(hats), max_hats_per_bar(170))

    def test_loop_duration_matches_bpm(self) -> None:
        bpm = 86.0
        bars = 2
        pcm, meta = render_phonk_loop(
            prompt="memphis phonk", bpm=bpm, bars=bars, variation=1
        )
        expected = (60.0 / bpm) * 4 * bars
        actual = len(pcm) / 44100
        self.assertAlmostEqual(actual, expected, delta=0.02)
        self.assertEqual(meta["bars"], bars)

    def test_batch_unique_seeds(self) -> None:
        a, _ = render_phonk_loop(prompt="memphis", bpm=90, bars=2, variation=0)
        b, _ = render_phonk_loop(prompt="memphis", bpm=90, bars=2, variation=1)
        self.assertNotEqual(a[:5000], b[:5000])

    def test_prompt_bpm_parse(self) -> None:
        self.assertEqual(parse_bpm_from_prompt("memphis 92 bpm"), 92.0)

    def test_artist_lane_detection(self) -> None:
        lane = resolve_memphis_lane("dirty 808 dj paul memphis")
        self.assertIsNotNone(lane)
        assert lane is not None
        self.assertEqual(lane.lane_id, "dj_paul")
        self.assertEqual(parse_bpm_from_prompt("dj paul style"), 84.0)

    def test_lane_in_loop_meta(self) -> None:
        _, meta = render_phonk_loop(
            prompt="juicy j phonk memphis", bpm=86, bars=2, variation=0
        )
        self.assertEqual(meta["memphisLane"], "juicy_j")


if __name__ == "__main__":
    unittest.main()
