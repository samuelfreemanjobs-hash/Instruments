#!/usr/bin/env python3
"""Contracts for kit mastering, the WAV auditor, presets, and the preview stitcher."""

from __future__ import annotations

import json
import struct
import subprocess
import sys
import tempfile
import unittest
import wave
from pathlib import Path

ROOT = Path(__file__).resolve().parent
sys.path.insert(0, str(ROOT))

from pcmio import read_wav  # noqa: E402
from visualize_wav import audit  # noqa: E402


def write_pcm16(path: Path, samples: list[float], rate: int = 48000) -> None:
    frames = b"".join(struct.pack("<h", int(max(-1.0, min(1.0, sample)) * 32767)) for sample in samples)
    with wave.open(str(path), "wb") as handle:
        handle.setnchannels(1)
        handle.setsampwidth(2)
        handle.setframerate(rate)
        handle.writeframes(frames)


class WorkflowTests(unittest.TestCase):
    def test_process_kit_trims_normalizes_and_zips(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            folder = Path(tmp) / "raw"
            out = Path(tmp) / "kit"
            folder.mkdir()
            silence = [0.0] * 2000
            tone = [0.2 * ((index % 20) - 10) / 10 for index in range(8000)]
            write_pcm16(folder / "bounce_808_G1.wav", silence + tone)
            result = subprocess.run(
                [
                    sys.executable,
                    str(ROOT / "process_kit.py"),
                    str(folder),
                    "--out",
                    str(out),
                    "--zip",
                    "--peak-db",
                    "-0.3",
                ],
                check=False,
                capture_output=True,
                text=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            rendered = next(out.glob("DLZ_808Lab_808_01_*.wav"))
            samples, rate, _channels = read_wav(rendered)
            self.assertEqual(rate, 48000)
            self.assertLess(len(samples), 9000)
            self.assertGreater(max(abs(sample) for sample in samples[:200]), 0.05)
            peak = max(abs(sample) for sample in samples)
            self.assertAlmostEqual(peak, 10 ** (-0.3 / 20), delta=0.02)
            self.assertTrue(out.with_suffix(".zip").exists())
            with wave.open(str(rendered), "rb") as handle:
                self.assertEqual(handle.getsampwidth(), 3)

    def test_visualizer_flags_dc_and_accepts_centered_sine(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            clean = Path(tmp) / "clean.wav"
            dirty = Path(tmp) / "dirty.wav"
            sine = [0.4 * ((index % 48) - 24) / 24 for index in range(4800)]
            write_pcm16(clean, sine)
            write_pcm16(dirty, [sample + 0.05 for sample in sine])
            self.assertTrue(audit(clean)["passed"])
            self.assertIn("dc offset", audit(dirty)["problems"])

    def test_presets_stay_inside_ranges_and_sfz_maps_velocity(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            out = Path(tmp) / "presets"
            wavs = Path(tmp) / "wavs"
            wavs.mkdir()
            write_pcm16(wavs / "DLZ_808Lab_808_01_G1_v1.wav", [0.1, 0.0])
            write_pcm16(wavs / "DLZ_808Lab_808_01_G1_v2_rr1.wav", [0.2, 0.0])
            sfz = Path(tmp) / "kit.sfz"
            result = subprocess.run(
                [
                    sys.executable,
                    str(ROOT / "preset_factory.py"),
                    "--out",
                    str(out),
                    "--sfz-from",
                    str(wavs),
                    "--sfz-out",
                    str(sfz),
                ],
                check=False,
                capture_output=True,
                text=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            ranges = {
                item["id"]: (item["min"], item["max"])
                for item in json.loads((ROOT.parent / "params.json").read_text(encoding="utf-8"))["parameters"]
            }
            for path in out.glob("*.json"):
                values = json.loads(path.read_text(encoding="utf-8"))["parameters"]
                for key, value in values.items():
                    low, high = ranges[key]
                    self.assertGreaterEqual(value, low)
                    self.assertLessEqual(value, high)
            text = sfz.read_text(encoding="utf-8")
            self.assertIn("lovel=1 hivel=63", text)
            self.assertIn("lovel=64 hivel=127", text)
            self.assertIn("seq_position=1", text)

    def test_preview_length(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            folder = Path(tmp) / "shots"
            folder.mkdir()
            write_pcm16(folder / "kick.wav", [0.5] + [0.0] * 200)
            write_pcm16(folder / "hat.wav", [0.2] + [0.0] * 80)
            write_pcm16(folder / "snare.wav", [0.4] + [0.0] * 120)
            destination = Path(tmp) / "preview.wav"
            result = subprocess.run(
                [
                    sys.executable,
                    str(ROOT / "stitch_preview.py"),
                    str(folder),
                    "--out",
                    str(destination),
                    "--seconds",
                    "2",
                    "--bpm",
                    "140",
                ],
                check=False,
                capture_output=True,
                text=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            _samples, rate, _channels = read_wav(destination)
            with wave.open(str(destination), "rb") as handle:
                duration = handle.getnframes() / handle.getframerate()
            self.assertEqual(rate, 48000)
            self.assertAlmostEqual(duration, 2.0, delta=0.05)

    def test_filmstrip_writes_a_strip(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            destination = Path(tmp) / "knob.png"
            result = subprocess.run(
                [sys.executable, str(ROOT / "render_filmstrip.py"), "--out", str(destination), "--frames", "4", "--size", "8"],
                check=False,
                capture_output=True,
                text=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            produced = list(Path(tmp).glob("knob.*"))
            self.assertTrue(produced)


if __name__ == "__main__":
    unittest.main()
