"""Contracts for full Black Box Collection era inventories + Dirty Circuits."""

from __future__ import annotations

import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from scripts.black_box.generate_era import generate_all, load_profile  # noqa: E402
from scripts.black_box.package_era import package  # noqa: E402
from scripts.black_box.package_dirty_circuits import build_bb96dc, build_dirty_circuits  # noqa: E402
from scripts.black_box.seeds import sku_seed  # noqa: E402


class CollectionEraTests(unittest.TestCase):
    def _assert_era(self, key: str) -> None:
        profile = load_profile(key)
        lo, hi = profile["target_samples"]
        with tempfile.TemporaryDirectory() as td:
            masters = Path(td) / "masters"
            staging = Path(td) / "staging"
            rows = generate_all(masters, profile)
            self.assertTrue(lo <= len(rows) <= hi, f"{key}: {len(rows)} not in {lo}-{hi}")
            manifest = package(staging, masters, profile)
            pack = staging / profile["pack_dir"]
            self.assertTrue((pack / "_DOCS" / "Hardware_Processing_Guide.md").is_file())
            self.assertTrue((pack / "02_BITCRUSHED_AND_PITCHED").is_dir())
            self.assertTrue((pack / "03_CASSETTE_AND_TAPE").is_dir())
            self.assertTrue((pack / "04_PARALLEL_PROCESS_BANKS" / "CASSETTE").is_dir())
            self.assertGreater(manifest["wav_count"], len(rows))

    def test_bb93(self) -> None:
        self._assert_era("bb93")

    def test_bb98(self) -> None:
        self._assert_era("bb98")

    def test_bb2000(self) -> None:
        self._assert_era("bb2000")

    def test_bb96_doubled_480(self) -> None:
        profile = load_profile("bb96")
        with tempfile.TemporaryDirectory() as td:
            rows = generate_all(Path(td), profile)
            self.assertEqual(len(rows), 480)

    def test_sku_seed_stable(self) -> None:
        a = sku_seed("BB96", 9300)
        b = sku_seed("BB96", 9300)
        self.assertEqual(a, b)
        self.assertNotEqual(sku_seed("BB96"), sku_seed("BB93"))

    def test_profiles_carry_voice(self) -> None:
        for key in ("bb93", "bb96", "bb98", "bb2000"):
            voice = load_profile(key).get("voice") or {}
            self.assertIn("808_tuning", voice)
            self.assertIn("snare_pitch_base", voice)

    def test_dirty_circuits_and_bb96dc(self) -> None:
        profile = load_profile("bb96")
        with tempfile.TemporaryDirectory() as td:
            masters = Path(td) / "masters"
            generate_all(masters, profile)
            oneshots = sum(
                1
                for p in masters.rglob("*.wav")
                if "loop" not in p.parent.name.lower()
            )
            m1 = build_dirty_circuits(Path(td) / "dc", masters)
            m2 = build_bb96dc(Path(td) / "bb96dc", masters)
            self.assertGreater(m1["wav_count"], 100)
            # CLEAN+DIRTY+RUINED+CASS (+ loops) for every oneshot
            self.assertGreaterEqual(m2["oneshot_sources"], oneshots)
            self.assertEqual(m2["banks"]["clean_dirty_pairs"], oneshots * 2)
            self.assertEqual(m2["banks"]["ruined_exclusives"], oneshots)
            self.assertEqual(m2["banks"]["cassette_desk"], oneshots)
            self.assertTrue((Path(td) / "dc" / "Dirty_Circuits" / "01_PROCESS_MODES" / "DIRTY").is_dir())
            self.assertTrue(
                (Path(td) / "bb96dc" / "Black_Box_96_Dirty_Circuits" / "01_CLEAN_TO_DIRTY_PAIRS").is_dir()
            )
            self.assertTrue(
                (Path(td) / "bb96dc" / "Black_Box_96_Dirty_Circuits" / "03_CASSETTE_DESK").is_dir()
            )


if __name__ == "__main__":
    unittest.main()
