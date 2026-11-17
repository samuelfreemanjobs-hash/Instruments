"""Contracts for BLACK BOX 96 commercial package (FX / pitch / cassette banks)."""

from __future__ import annotations

import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from scripts.black_box.generate_era import generate_all, load_profile  # noqa: E402
from scripts.black_box.package_era import package  # noqa: E402

PROFILE = load_profile("bb96")


class BlackBox96PackageTests(unittest.TestCase):
    def test_package_has_process_banks_and_guide(self) -> None:
        with tempfile.TemporaryDirectory() as td:
            td_path = Path(td)
            masters = td_path / "masters"
            staging = td_path / "staging"
            generate_all(masters, PROFILE)
            manifest = package(staging, masters, PROFILE)
            pack = staging / "Black_Box_96"

            self.assertGreaterEqual(manifest["counts"]["core_oneshots"], 456)
            self.assertEqual(manifest["counts"]["loops"], 24)
            self.assertGreater(manifest["counts"]["bitcrushed_pitched"], 0)
            self.assertGreater(manifest["counts"]["cassette_tape"], 0)
            self.assertGreater(manifest["counts"]["parallel_banks"], 0)
            self.assertGreater(manifest["counts"]["matched_mini_kits"], 0)

            self.assertTrue((pack / "02_BITCRUSHED_AND_PITCHED" / "RateDropped_808s").is_dir())
            self.assertTrue((pack / "03_CASSETTE_AND_TAPE").is_dir())
            for lane in ("RAW", "TAPE", "LOFI", "CASSETTE"):
                self.assertTrue((pack / "04_PARALLEL_PROCESS_BANKS" / lane).is_dir())
            self.assertTrue((pack / "06_MATCHED_MINI_KITS").is_dir())
            guide = pack / "_DOCS" / "Hardware_Processing_Guide.md"
            self.assertTrue(guide.is_file())
            text = guide.read_text(encoding="utf-8")
            self.assertIn("CASSETTE", text)
            self.assertIn("pitch", text.lower())

            pitched = list((pack / "02_BITCRUSHED_AND_PITCHED" / "RateDropped_808s").glob("*_pitch-24st.wav"))
            self.assertGreaterEqual(len(pitched), 1)
            cass = list((pack / "03_CASSETTE_AND_TAPE").rglob("*_cassette.wav"))
            self.assertGreaterEqual(len(cass), 1)


if __name__ == "__main__":
    unittest.main()
