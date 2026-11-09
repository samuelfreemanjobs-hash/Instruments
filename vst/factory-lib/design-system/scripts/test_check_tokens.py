#!/usr/bin/env python3
"""Contract tests for check_tokens.py: each test breaks one rule and expects a failure.

Run: python3 -m unittest discover -s vst/factory-lib/design-system/scripts -p "test_*.py"
"""
from __future__ import annotations

import copy
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from check_tokens import check, contrast, load_tokens  # noqa: E402


class CheckTokensContracts(unittest.TestCase):
    def setUp(self) -> None:
        self.tokens = load_tokens()

    def errors_after(self, mutate) -> list[str]:
        t = copy.deepcopy(self.tokens)
        mutate(t)
        return check(t)

    def test_shipped_tokens_pass(self):
        """Contract: the committed tokens satisfy every rule."""
        self.assertEqual(check(self.tokens), [])

    def test_cyan_outside_meter_fails(self):
        """Contract: steel cyan is reserved for semantic.meter."""
        errs = self.errors_after(lambda t: t["semantic"]["brand"].__setitem__("stroke", "{color.cyan.400}"))
        self.assertTrue(any("uses color.cyan" in e for e in errs), errs)

    def test_gold_outside_premium_fails(self):
        """Contract: gold is reserved for premium badges."""
        errs = self.errors_after(lambda t: t["semantic"]["active"].__setitem__("default", "{color.gold.500}"))
        self.assertTrue(any("uses color.gold" in e for e in errs), errs)

    def test_low_contrast_fails(self):
        """Contract: declared contrast minimums are enforced."""
        errs = self.errors_after(lambda t: t["color"]["neutral"].__setitem__("100", "#2A2A2A"))
        self.assertTrue(any(e.startswith("contrast semantic.text.primary") for e in errs), errs)

    def test_unresolved_reference_fails(self):
        """Contract: every reference resolves."""
        errs = self.errors_after(lambda t: t["semantic"]["bg"].__setitem__("panel", "{color.neutral.123}"))
        self.assertTrue(any("unresolved reference" in e for e in errs), errs)

    def test_reference_cycle_fails(self):
        """Contract: reference cycles are rejected, not looped forever."""
        def mutate(t):
            t["semantic"]["bg"]["panel"] = "{semantic.bg.section}"
            t["semantic"]["bg"]["section"] = "{semantic.bg.panel}"
        errs = self.errors_after(mutate)
        self.assertTrue(any("cycle" in e for e in errs), errs)

    def test_bad_hex_fails(self):
        """Contract: colours are #RRGGBB."""
        errs = self.errors_after(lambda t: t["color"]["lime"].__setitem__("500", "lime"))
        self.assertTrue(any("not a #RRGGBB" in e for e in errs), errs)

    def test_off_grid_spacing_fails(self):
        """Contract: spacing is a multiple of 4."""
        errs = self.errors_after(lambda t: t["space"].__setitem__("2", 6))
        self.assertTrue(any("space.2" in e for e in errs), errs)

    def test_tiny_text_fails(self):
        """Contract: no text style below typography.minSize."""
        errs = self.errors_after(lambda t: t["typography"]["style"]["scale"].__setitem__("size", 9))
        self.assertTrue(any("typography.style.scale" in e for e in errs), errs)

    def test_small_hit_target_fails(self):
        """Contract: interactive controls meet hitTargetMin."""
        errs = self.errors_after(lambda t: t["size"]["button"].__setitem__("sm", 20))
        self.assertTrue(any("size.button.sm" in e for e in errs), errs)

    def test_contrast_formula_reference_values(self):
        """Contract: WCAG 2.x formula (black/white = 21, identical = 1)."""
        self.assertAlmostEqual(contrast("#000000", "#FFFFFF"), 21.0, places=2)
        self.assertAlmostEqual(contrast("#7B2CBF", "#7B2CBF"), 1.0, places=6)


if __name__ == "__main__":
    unittest.main()
