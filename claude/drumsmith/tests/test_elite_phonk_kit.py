"""Contracts for elite phonk kit pack builder."""

from __future__ import annotations

import sys
import tempfile
import unittest
import wave
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from scripts.memphis_oneshots.build_elite_phonk_kit import build_kit  # noqa: E402


class ElitePhonkKitTests(unittest.TestCase):
    def test_parallel_lanes_and_bpm_loops(self) -> None:
        with tempfile.TemporaryDirectory() as td:
            out = Path(td) / "kit"
            manifest = build_kit(out)
            self.assertFalse(manifest["release_ready"])
            self.assertGreaterEqual(manifest["file_count"], 15)
            for lane in ("RAW", "TAPE", "LOFI"):
                kick = out / "01_PARALLEL_PROCESS_BANKS" / lane / "Kicks" / f"CNV_{lane}_KICK_01.wav"
                self.assertTrue(kick.is_file(), kick)
                with wave.open(str(kick), "rb") as w:
                    self.assertEqual(w.getsampwidth(), 2)
            bounce = list((out / "02_MATCHED_MINI_KITS" / "04_ClubBounce").glob("*_134bpm.wav"))
            self.assertEqual(len(bounce), 1)
            crawl = list((out / "02_MATCHED_MINI_KITS" / "01_ConverterCrawl").glob("*_94bpm.wav"))
            self.assertEqual(len(crawl), 1)


if __name__ == "__main__":
    unittest.main()
