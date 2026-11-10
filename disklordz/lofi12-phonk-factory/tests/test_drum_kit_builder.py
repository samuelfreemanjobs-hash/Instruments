"""Kit builder smoke tests."""

from __future__ import annotations

import json
import tempfile
import unittest
from pathlib import Path

import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

from drum_kit_builder import build_kit  # noqa: E402


class DrumKitBuilderTests(unittest.TestCase):
    def test_demo_occult_kit(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            out = Path(tmp) / "kit"
            manifest = build_kit(mode_id="occult_tape", out_dir=out, kit_size="demo", seed=7)
            self.assertGreaterEqual(manifest["oneShotCount"], 20)
            self.assertGreaterEqual(manifest["loopCount"], 3)
            self.assertTrue((out / "manifest.json").is_file())
            loaded = json.loads((out / "manifest.json").read_text())
            self.assertEqual(loaded["factoryMode"], "occult_tape")
            tex = list((out / "Loops" / "textures").glob("*.wav"))
            self.assertGreaterEqual(len(tex), 1)


if __name__ == "__main__":
    unittest.main()
