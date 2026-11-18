"""Contracts for MEMPHIS-DR660 snare Engine B."""

from __future__ import annotations

import sys
import tempfile
import unittest
import wave
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from scripts.memphis_oneshots.phonk_snare import generate_memphis_phonk_snare  # noqa: E402
from scripts.memphis_oneshots.phonk_kick import write_wav_24bit_mono  # noqa: E402


class MemphisPhonkSnareTests(unittest.TestCase):
    def test_length_peak_and_seed_repro(self) -> None:
        a = generate_memphis_phonk_snare(duration=0.35, seed=660)
        b = generate_memphis_phonk_snare(duration=0.35, seed=660)
        self.assertEqual(a.shape, (15435,))
        self.assertTrue(np.allclose(a, b))
        peak = float(np.max(np.abs(a)))
        self.assertGreaterEqual(peak, 0.85)
        self.assertLessEqual(peak, 1.0)

    def test_fs3_differs_from_a3(self) -> None:
        a3 = generate_memphis_phonk_snare(membrane_root=220.0, seed=1)
        fs3 = generate_memphis_phonk_snare(membrane_root=185.0, seed=1)
        self.assertFalse(np.allclose(a3, fs3))

    def test_wav_24bit(self) -> None:
        with tempfile.TemporaryDirectory() as td:
            out = Path(td) / "snare.wav"
            buf = generate_memphis_phonk_snare(duration=0.12, seed=2)
            write_wav_24bit_mono(out, buf)
            with wave.open(str(out), "rb") as w:
                self.assertEqual(w.getnchannels(), 1)
                self.assertEqual(w.getsampwidth(), 3)
                self.assertEqual(w.getframerate(), 44100)


if __name__ == "__main__":
    unittest.main()
