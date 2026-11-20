"""Tests for Lofi-12 phonk bank export constraints."""

from __future__ import annotations

import json
import tempfile
import unittest
import wave
from pathlib import Path

import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

from lofi12_prepare import LOFI_RATE_24K, MAX_SEC_24K  # noqa: E402
from phonk_factory import generate_bank  # noqa: E402


class PhonkFactoryTests(unittest.TestCase):
    def test_bank_writes_sixteen_wavs_under_duration(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            out = Path(tmp) / "bank"
            manifest = generate_bank(
                prompt="dirty memphis phonk 808",
                out_dir=out,
                variation=0,
                dst_rate=LOFI_RATE_24K,
            )
            self.assertEqual(len(manifest["slots"]), 16)
            self.assertTrue((out / "load_manifest.json").is_file())
            for slot in manifest["slots"]:
                wav = out / slot["filename"]
                self.assertTrue(wav.is_file(), slot["filename"])
                with wave.open(str(wav), "rb") as w:
                    self.assertEqual(w.getnchannels(), 1)
                    self.assertEqual(w.getframerate(), LOFI_RATE_24K)
                    self.assertLessEqual(
                        w.getnframes() / LOFI_RATE_24K, MAX_SEC_24K + 0.01
                    )
            loaded = json.loads((out / "load_manifest.json").read_text())
            self.assertEqual(loaded["format"], "LOFI12_PHONK_BANK")


if __name__ == "__main__":
    unittest.main()
