#!/usr/bin/env python3
"""Unit tests for tools/visual_diff.py (synthetic images, no JUCE)."""
from __future__ import annotations

import json
import shutil
import tempfile
import unittest
from pathlib import Path

from PIL import Image

import visual_diff as vd


def _write_png(path: Path, color: tuple[int, int, int, int], size=(32, 16)) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    Image.new("RGBA", size, color).save(path)


def _write_sidecar(path: Path, text: str, width=32, height=16) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    body = {
        "component": "Synthetic",
        "state": "default",
        "scale": 1.0,
        "theme": "test",
        "width": width,
        "height": height,
        "texts": [
            {"string": text, "bounds": {"x": 1.0, "y": 1.0, "w": 10.0, "h": 8.0}},
        ],
    }
    path.write_text(json.dumps(body), encoding="utf-8")


def _manifest(tmp: Path) -> dict:
    return {
        "suite": "factory-gui",
        "juce_tag": "8.0.15",
        "juce_version": "JUCE v8.0.15",
        "os": "Linux",
        "fonts": {},
        "themes": ["test"],
    }


class VisualDiffTests(unittest.TestCase):
    def setUp(self) -> None:
        self.tmp = Path(tempfile.mkdtemp(prefix="visual_diff_"))
        self.golden = self.tmp / "golden"
        self.actual = self.tmp / "actual"
        self.out = self.tmp / "out"
        theme_g = self.golden / "test"
        theme_a = self.actual / "test"
        theme_g.mkdir(parents=True)
        theme_a.mkdir(parents=True)
        (self.golden / "MANIFEST.json").write_text(
            json.dumps(_manifest(self.tmp), indent=2), encoding="utf-8"
        )
        (self.actual / "BUILD_INFO.json").write_text(
            json.dumps(
                {
                    "juce_version": "JUCE v8.0.15",
                    "os": "Linux",
                }
            ),
            encoding="utf-8",
        )
        self.theme_g = theme_g
        self.theme_a = theme_a

    def tearDown(self) -> None:
        shutil.rmtree(self.tmp, ignore_errors=True)

    def test_identical_passes(self) -> None:
        _write_png(self.theme_g / "Widget-default@1x.png", (10, 20, 30, 255))
        _write_png(self.theme_a / "Widget-default@1x.png", (10, 20, 30, 255))
        _write_sidecar(self.theme_g / "Widget-default@1x.json", "HELLO")
        _write_sidecar(self.theme_a / "Widget-default@1x.json", "HELLO")
        report = vd.run_compare(
            self.actual,
            self.golden,
            self.out,
            channel_tol=8,
            max_diff_ratio=0.002,
            expected_juce="8.0.15",
            update=False,
        )
        self.assertTrue(report.ok, vd.write_summary_md(report))

    def test_one_pixel_over_tolerance_fails(self) -> None:
        _write_png(self.theme_g / "Widget-default@1x.png", (10, 20, 30, 255), size=(10, 10))
        # One pixel differs by 20 (> tol 8); 1/100 = 1% > 0.2%
        img = Image.new("RGBA", (10, 10), (10, 20, 30, 255))
        img.putpixel((0, 0), (30, 20, 30, 255))
        img.save(self.theme_a / "Widget-default@1x.png")
        _write_sidecar(self.theme_g / "Widget-default@1x.json", "HELLO")
        _write_sidecar(self.theme_a / "Widget-default@1x.json", "HELLO")
        report = vd.run_compare(
            self.actual,
            self.golden,
            self.out,
            channel_tol=8,
            max_diff_ratio=0.002,
            expected_juce="8.0.15",
            update=False,
        )
        self.assertFalse(report.ok)
        fails = [r for r in report.image_results if not r.ok]
        self.assertEqual(len(fails), 1)
        self.assertIn("pixels differ", fails[0].reason)
        self.assertTrue((self.out / "diffs" / "test" / "Widget-default@1x.diff.png").is_file())

    def test_size_mismatch_fails(self) -> None:
        _write_png(self.theme_g / "Widget-default@1x.png", (10, 20, 30, 255), size=(32, 16))
        _write_png(self.theme_a / "Widget-default@1x.png", (10, 20, 30, 255), size=(16, 16))
        _write_sidecar(self.theme_g / "Widget-default@1x.json", "HELLO")
        _write_sidecar(self.theme_a / "Widget-default@1x.json", "HELLO")
        report = vd.run_compare(
            self.actual,
            self.golden,
            self.out,
            channel_tol=8,
            max_diff_ratio=0.002,
            expected_juce="8.0.15",
            update=False,
        )
        self.assertFalse(report.ok)
        fails = [r for r in report.image_results if not r.ok]
        self.assertTrue(any("size mismatch" in r.reason for r in fails))

    def test_text_mismatch_fails(self) -> None:
        _write_png(self.theme_g / "Widget-default@1x.png", (10, 20, 30, 255))
        _write_png(self.theme_a / "Widget-default@1x.png", (10, 20, 30, 255))
        _write_sidecar(self.theme_g / "Widget-default@1x.json", "METERS · INPUT")
        _write_sidecar(self.theme_a / "Widget-default@1x.json", "METERS Â· INPUT")
        report = vd.run_compare(
            self.actual,
            self.golden,
            self.out,
            channel_tol=8,
            max_diff_ratio=0.002,
            expected_juce="8.0.15",
            update=False,
        )
        self.assertFalse(report.ok)
        fails = [r for r in report.text_results if not r.ok]
        self.assertEqual(len(fails), 1)
        self.assertIn("string mismatch", fails[0].reason)

    def test_ellipsis_detected_fails(self) -> None:
        _write_png(self.theme_g / "Widget-default@1x.png", (10, 20, 30, 255))
        _write_png(self.theme_a / "Widget-default@1x.png", (10, 20, 30, 255))
        _write_sidecar(self.theme_g / "Widget-default@1x.json", "-12.0 dB")
        _write_sidecar(self.theme_a / "Widget-default@1x.json", "...")
        report = vd.run_compare(
            self.actual,
            self.golden,
            self.out,
            channel_tol=8,
            max_diff_ratio=0.002,
            expected_juce="8.0.15",
            update=False,
        )
        self.assertFalse(report.ok)
        fails = [r for r in report.text_results if not r.ok]
        self.assertEqual(len(fails), 1)
        self.assertIn("ellipsis", fails[0].reason.lower())

    def test_missing_golden_fails_without_update(self) -> None:
        _write_png(self.theme_a / "Widget-default@1x.png", (10, 20, 30, 255))
        _write_sidecar(self.theme_a / "Widget-default@1x.json", "HELLO")
        report = vd.run_compare(
            self.actual,
            self.golden,
            self.out,
            channel_tol=8,
            max_diff_ratio=0.002,
            expected_juce="8.0.15",
            update=False,
        )
        self.assertFalse(report.ok)
        self.assertTrue(any("missing golden" in r.reason for r in report.image_results))

    def test_bounds_outside_fails(self) -> None:
        _write_png(self.theme_g / "Widget-default@1x.png", (10, 20, 30, 255))
        _write_png(self.theme_a / "Widget-default@1x.png", (10, 20, 30, 255))
        _write_sidecar(self.theme_g / "Widget-default@1x.json", "OK")
        body = {
            "component": "Synthetic",
            "state": "default",
            "scale": 1.0,
            "theme": "test",
            "width": 32,
            "height": 16,
            "texts": [
                {"string": "OK", "bounds": {"x": 0.0, "y": 0.0, "w": 100.0, "h": 8.0}},
            ],
        }
        (self.theme_a / "Widget-default@1x.json").write_text(json.dumps(body), encoding="utf-8")
        report = vd.run_compare(
            self.actual,
            self.golden,
            self.out,
            channel_tol=8,
            max_diff_ratio=0.002,
            expected_juce="8.0.15",
            update=False,
        )
        self.assertFalse(report.ok)
        self.assertTrue(any("bounds outside" in r.reason for r in report.text_results))


if __name__ == "__main__":
    unittest.main()
