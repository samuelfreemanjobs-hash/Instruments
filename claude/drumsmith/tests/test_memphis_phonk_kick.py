"""Contracts for MEMPHIS-660 Engine B kick renderer (stdlib unittest)."""

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

from scripts.memphis_oneshots.phonk_kick import (  # noqa: E402
    generate_memphis_phonk_kick,
    write_wav_24bit_mono,
)


class MemphisPhonkKickTests(unittest.TestCase):
    def test_kick_length_and_peak(self) -> None:
        buf = generate_memphis_phonk_kick(root_freq=43.65, duration=0.25)
        self.assertEqual(buf.shape, (11025,))
        peak = float(np.max(np.abs(buf)))
        self.assertGreaterEqual(peak, 0.85)
        self.assertLessEqual(peak, 1.0)

    def test_d1_differs_from_f1(self) -> None:
        f1 = generate_memphis_phonk_kick(root_freq=43.65)
        d1 = generate_memphis_phonk_kick(root_freq=36.71)
        self.assertFalse(np.allclose(f1, d1))

    def test_writes_24bit_mono_wav(self) -> None:
        with tempfile.TemporaryDirectory() as td:
            out = Path(td) / "kick.wav"
            buf = generate_memphis_phonk_kick(duration=0.1)
            write_wav_24bit_mono(out, buf)
            with wave.open(str(out), "rb") as w:
                self.assertEqual(w.getnchannels(), 1)
                self.assertEqual(w.getsampwidth(), 3)
                self.assertEqual(w.getframerate(), 44100)
                self.assertEqual(w.getnframes(), len(buf))


if __name__ == "__main__":
    unittest.main()
