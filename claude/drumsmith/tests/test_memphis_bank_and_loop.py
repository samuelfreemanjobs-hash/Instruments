"""Contracts for Memphis kit bank batch + headless loop render."""

from __future__ import annotations

import json
import sys
import tempfile
import unittest
import wave
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from scripts.memphis_oneshots.batch_memphis_bank import render_bank  # noqa: E402
from scripts.memphis_oneshots.phonk_hat import generate_memphis_phonk_hat  # noqa: E402
from scripts.memphis_oneshots.render_memphis_loop import render_memphis_loop  # noqa: E402
from scripts.dsp_math import loop_samples  # noqa: E402


class PhonkHatTests(unittest.TestCase):
    def test_hat_peak_and_seed(self) -> None:
        a = generate_memphis_phonk_hat(seed=1)
        b = generate_memphis_phonk_hat(seed=1)
        self.assertTrue(np.allclose(a, b))
        self.assertGreaterEqual(float(np.max(np.abs(a))), 0.85)


class BankBatchTests(unittest.TestCase):
    def test_render_small_recipe(self) -> None:
        recipe = {
            "kicks": [{"key": "F1", "drive_db": 4.0}],
            "snares": [{"key": "A3", "seed": 1}],
            "hats": [{"center_hz": 9000, "seed": 2}],
        }
        with tempfile.TemporaryDirectory() as td:
            out = Path(td)
            rows = render_bank(out, recipe)
            self.assertEqual(len(rows), 3)
            for r in rows:
                wav = out / r["path"]
                self.assertTrue(wav.is_file())
                with wave.open(str(wav), "rb") as w:
                    self.assertEqual(w.getsampwidth(), 2)
                    self.assertEqual(w.getframerate(), 44100)
            manifest = json.loads((out / "manifest.json").read_text(encoding="utf-8"))
            self.assertFalse(manifest["release_ready"])


class MemphisLoopTests(unittest.TestCase):
    def test_loop_length_matches_blueprint(self) -> None:
        bp = {
            "bpm": 94.0,
            "bars": 2,
            "sample_rate": 44100,
            "grid_subdivisions_per_bar": 16,
            "samples_per_bar": loop_samples(1, 94.0, 44100),
            "lanes": {
                "KICK": [{"step": 0, "vel": 100}, {"step": 16, "vel": 90}],
                "SNARE": [{"step": 8, "vel": 110}, {"step": 24, "vel": 105}],
                "HAT": [{"step": i, "vel": 60} for i in range(0, 32, 4)],
            },
        }
        audio = render_memphis_loop(bp, seed=3)
        self.assertEqual(len(audio), loop_samples(2, 94.0, 44100))
        self.assertGreater(float(np.max(np.abs(audio))), 0.5)

    def test_clap_lane_audible_and_in_midi(self) -> None:
        from scripts.memphis_oneshots.render_memphis_loop import write_blueprint_midi

        bp = {
            "bpm": 94.0,
            "bars": 2,
            "sample_rate": 44100,
            "grid_subdivisions_per_bar": 16,
            "samples_per_bar": loop_samples(1, 94.0, 44100),
            "lanes": {
                "SNARE": [{"step": 8, "vel": 100}],
                "CLAP": [{"step": 8, "vel": 70}, {"step": 24, "vel": 65}],
            },
        }
        snare_only = render_memphis_loop(
            {**bp, "lanes": {"SNARE": bp["lanes"]["SNARE"]}}, seed=5
        )
        with_clap = render_memphis_loop(bp, seed=5)
        self.assertGreater(float(np.max(np.abs(with_clap))), float(np.max(np.abs(snare_only))) * 0.9)
        self.assertFalse(np.allclose(snare_only, with_clap))
        with tempfile.TemporaryDirectory() as td:
            mid = Path(td) / "clap.mid"
            write_blueprint_midi(bp, mid)
            raw = mid.read_bytes()
            # Note-on channel 10 (0x99) for GM clap note 39
            self.assertIn(bytes([0x99, 39]), raw)


if __name__ == "__main__":
    unittest.main()
