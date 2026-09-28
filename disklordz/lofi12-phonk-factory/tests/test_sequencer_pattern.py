"""Sequencer pattern schema + groove import."""

from __future__ import annotations

import json
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "sequencer" / "scripts"))
sys.path.insert(0, str(ROOT / "scripts"))

from pattern_from_groove import groove_to_pattern  # noqa: E402
from pattern_schema import Pattern, load_pattern, save_pattern  # noqa: E402


class SequencerPatternTests(unittest.TestCase):
    def test_roundtrip_json(self) -> None:
        pat = Pattern(bpm=86.0)
        pat.tracks[0].steps[0].on = True
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "p.json"
            save_pattern(path, pat)
            loaded = load_pattern(path)
            self.assertEqual(loaded.bpm, 86.0)
            self.assertTrue(loaded.tracks[0].steps[0].on)

    def test_groove_import_has_hits(self) -> None:
        pat = groove_to_pattern("dj paul memphis phonk", 84.0, 0)
        self.assertEqual(len(pat.tracks), 6)
        kicks = sum(1 for s in pat.tracks[0].steps if s.on)
        snares = sum(1 for s in pat.tracks[1].steps if s.on)
        self.assertGreater(kicks, 0)
        self.assertGreaterEqual(snares, 2)
        exported = pat.to_dict()
        self.assertEqual(exported["version"], 2)


if __name__ == "__main__":
    unittest.main()
