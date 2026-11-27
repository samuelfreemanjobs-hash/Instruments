"""ATL trap DSP helpers — smoke tests."""

from __future__ import annotations

import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

from atl_trap_dsp import (  # noqa: E402
    integrate_exp_pitch,
    sampler_2005_character,
    svf_bandpass,
)


class AtlTrapDspTests(unittest.TestCase):
    def test_pitch_sweep_monotonic(self) -> None:
        freqs, phases = integrate_exp_pitch(2000, 120.0, 40.0, tau=0.05)
        self.assertEqual(len(freqs), 2000)
        self.assertGreater(freqs[0], freqs[-1])
        self.assertGreater(phases[-1], phases[0])

    def test_svf_and_sampler_finite(self) -> None:
        noise = [0.1 if i % 2 else -0.1 for i in range(4000)]
        bp = svf_bandpass(noise, 1500.0)
        out = sampler_2005_character(bp)
        self.assertEqual(len(out), len(bp))
        peak = max(abs(s) for s in out)
        self.assertLessEqual(peak, 1.5)


if __name__ == "__main__":
    unittest.main()
