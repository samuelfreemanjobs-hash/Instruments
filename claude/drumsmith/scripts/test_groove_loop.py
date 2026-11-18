#!/usr/bin/env python3
"""Unit tests for learn_midi_groove + trap_loop_architect (stdlib)."""
from __future__ import annotations

import json
import tempfile
import unittest
from pathlib import Path

from dsp_math import loop_samples
from learn_midi_groove import analyze, parse_json_notes, parse_midi
from trap_loop_architect import build_blueprint, write_type0_midi, blueprint_to_midi_events


class TestLoopSamples(unittest.TestCase):
    def test_tls_bar(self):
        # kit-spec: 74 BPM @ 44.1k → 143027 samples/bar
        self.assertEqual(loop_samples(1, 74, 44100), 143027)


class TestGroove(unittest.TestCase):
    def test_json_notes_swingish(self):
        with tempfile.TemporaryDirectory() as d:
            p = Path(d) / "notes.json"
            ppq = 480
            grid = 16
            gt = (ppq * 4) // grid
            notes = [
                {"tick": 0, "pitch": 36, "velocity": 100},
                {"tick": gt + 40, "pitch": 42, "velocity": 80},  # delayed odd 16th
                {"tick": 2 * gt, "pitch": 38, "velocity": 110},
            ]
            p.write_text(json.dumps({"ppq": ppq, "bpm": 140, "notes": notes}), encoding="utf-8")
            card = analyze(parse_json_notes(p), grid, {36: "KICK", 38: "SNARE", 42: "HAT"})
            self.assertEqual(card["note_count"], 3)
            self.assertIsNotNone(card["swing_mean_odd_delta_ticks"])
            self.assertEqual(card["hits"][1]["lane"], "HAT")

    def test_midi_roundtrip_parse(self):
        with tempfile.TemporaryDirectory() as d:
            mid = Path(d) / "g.mid"
            write_type0_midi(mid, 140, 480, [(0, 36, 100), (240, 42, 80), (480, 38, 110)])
            payload = parse_midi(mid)
            self.assertGreaterEqual(len(payload["notes"]), 3)
            self.assertAlmostEqual(payload["bpm"], 140, places=0)


class TestArchitect(unittest.TestCase):
    def test_blueprint(self):
        bp = build_blueprint("t", "drift-phonk", 74, 2, 44100, 16)
        self.assertEqual(bp["samples_per_bar"], 143027)
        self.assertIn("KICK", bp["lanes"])
        self.assertEqual(bp["status"], "agent-demo")
        ev = blueprint_to_midi_events(bp, 480)
        self.assertTrue(ev)


if __name__ == "__main__":
    unittest.main()
