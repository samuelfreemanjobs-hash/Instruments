"""FM percussion voice tests."""

from __future__ import annotations

import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

from phonk_fm import fm_cowbell_808, fm_cowbell_memphis  # noqa: E402
from phonk_machine_engine import render_machine_voice  # noqa: E402
from phonk_synth import resolve_phonk_params  # noqa: E402


class PhonkFmTests(unittest.TestCase):
    def test_fm_cowbells_non_empty(self) -> None:
        a = fm_cowbell_808(540, seed=1)
        b = fm_cowbell_memphis(540, seed=1)
        self.assertGreater(len(a), 1000)
        self.assertNotEqual(a[:400], b[:400])

    def test_juicy_engine_uses_fm_cowbell(self) -> None:
        params = resolve_phonk_params("juicy j memphis", 0)
        pcm = render_machine_voice("juicy_j", "cowbell_low", params)
        self.assertGreater(len(pcm), 500)


if __name__ == "__main__":
    unittest.main()
