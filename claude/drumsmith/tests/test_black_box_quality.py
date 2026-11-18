"""Contracts for Black Box quality pass: layered diversify, pack lanes, calibration."""

from __future__ import annotations

import json
import sys
import tempfile
import unittest
import wave
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from scripts.black_box.curate_hero import select_heroes  # noqa: E402
from scripts.black_box.generate_era import generate_all, load_profile  # noqa: E402
from scripts.black_box.layered import layered_808, stereo_widen  # noqa: E402
from scripts.black_box.package_era import package  # noqa: E402
from scripts.black_box.reference_calibrate import calibrate  # noqa: E402
from scripts.black_box.voices import RECIPE_SETS, recipes_for  # noqa: E402


class QualityPassTests(unittest.TestCase):
    def test_recipe_sets_differ_by_era(self) -> None:
        self.assertNotEqual(RECIPE_SETS["hollow"]["808"], RECIPE_SETS["signature"]["808"])
        self.assertNotEqual(RECIPE_SETS["trunk"]["SNR"], RECIPE_SETS["digital"]["SNR"])
        self.assertIn("Hollow", recipes_for("808", {"recipe_set": "hollow"}))
        self.assertIn("Boom", recipes_for("808", {"recipe_set": "trunk"}))

    def test_layered_808_longer_than_half_second(self) -> None:
        buf = layered_808(
            root_freq=43.65,
            duration=1.5,
            pitch_mod_depth=48,
            pitch_decay=0.018,
            amp_decay=0.7,
            click_amount=0.5,
            clip_drive_db=5.5,
            lofi_sr=26040,
            bit_depth=12,
            tape_lpf_hz=11000,
            seed=1,
        )
        self.assertGreaterEqual(len(buf) / 44100.0, 1.2)
        self.assertGreater(float(np.max(np.abs(buf[: int(0.008 * 44100)]))), 0.2)

    def test_stereo_widen_shape(self) -> None:
        mono = np.random.default_rng(0).uniform(-0.5, 0.5, 1000).astype(np.float32)
        st = stereo_widen(mono)
        self.assertEqual(st.shape[0], 2)
        self.assertEqual(st.shape[1], 1000)

    def test_bb93_diversify_recipes(self) -> None:
        profile = load_profile("bb93")
        self.assertTrue(profile["voice"]["diversify"])
        self.assertEqual(profile["voice"]["recipe_set"], "hollow")
        with tempfile.TemporaryDirectory() as td:
            rows = generate_all(Path(td), profile)
            eights = [r for r in rows if r["kind"] == "808"]
            tags = {r["name"].split("_")[2] for r in eights}
            self.assertTrue(tags & {"Hollow", "Haunt", "Fog", "Mud"})
            # MIDI next to at least one loop
            mids = list(Path(td).rglob("*.mid"))
            self.assertGreaterEqual(len(mids), 1)

    def test_package_quality_lanes(self) -> None:
        profile = load_profile("bb96")
        with tempfile.TemporaryDirectory() as td:
            masters = Path(td) / "masters"
            staging = Path(td) / "staging"
            generate_all(masters, profile)
            manifest = package(staging, masters, profile)
            pack = staging / profile["pack_dir"]
            self.assertTrue((pack / "07_DESK_PRESETS").is_dir())
            self.assertTrue((pack / "08_STUDIO_24BIT").is_dir())
            self.assertTrue((pack / "09_STEREO_TOPS").is_dir())
            self.assertTrue((pack / "10_VELOCITY_LAYERS").is_dir())
            self.assertTrue((pack / "11_CONSTRUCTION_KITS").is_dir())
            self.assertTrue((pack / "12_STARTER_HERO").is_dir())
            self.assertGreater(manifest["counts"]["starter_hero"], 10)
            # 24-bit studio file
            studio = next((pack / "08_STUDIO_24BIT").glob("*.wav"))
            with wave.open(str(studio), "rb") as w:
                self.assertEqual(w.getsampwidth(), 3)
            heroes = select_heroes(masters, per_kind=4, max_total=20)
            self.assertGreaterEqual(len(heroes), 8)

    def test_calibrate_runs(self) -> None:
        report = calibrate("bb96")
        self.assertIn(report["grade"], {"A", "B", "C", "N/A"})
        self.assertFalse(report["release_ready"])


if __name__ == "__main__":
    unittest.main()
