#!/usr/bin/env python3
"""Contract tests for gen_juce_tokens.py (Phase 2 header generator).

Run: python3 -m unittest discover -s vst/factory-lib/design-system/scripts -p "test_*.py"
"""
from __future__ import annotations

import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

import gen_juce_tokens  # noqa: E402
from check_tokens import list_themes, load_contract, load_theme  # noqa: E402
from gen_juce_tokens import cpp_ident, generate_header, hex_to_argb  # noqa: E402


class GenJuceTokens(unittest.TestCase):
    def test_hex_to_argb(self):
        """Contract: #RRGGBB maps to opaque 0xAARRGGBB lowercase."""
        self.assertEqual(hex_to_argb("#CDFF00"), "0xffcdff00")
        self.assertEqual(hex_to_argb("#0A0A0C"), "0xff0a0a0c")

    def test_cpp_ident_escapes_keywords(self):
        """Contract: C++ keywords become valid identifiers (default → default_)."""
        self.assertEqual(cpp_ident("default"), "default_")
        self.assertEqual(cpp_ident("primary"), "primary")
        self.assertEqual(cpp_ident("2"), "n2")

    def test_header_contains_role_colours_and_meta(self):
        """Contract: generated header exposes theme meta + semantic colour roles."""
        tokens = load_theme("disklordz")
        text = generate_header(tokens, load_contract())
        self.assertIn('inline constexpr const char* themeId    = "disklordz";', text)
        self.assertIn("namespace fds {", text)
        self.assertIn("namespace color {", text)
        self.assertIn("inline constexpr std::uint32_t primary", text)
        self.assertIn("inline constexpr std::uint32_t default_", text)
        self.assertIn("namespace typography {", text)
        self.assertIn("namespace meter {", text)
        self.assertNotIn("#include <juce", text)
        self.assertNotIn("juce::", text)

    def test_generation_is_deterministic(self):
        """Contract: same tokens.json yields byte-identical header text."""
        tokens = load_theme("studio-light")
        contract = load_contract()
        a = generate_header(tokens, contract)
        b = generate_header(tokens, contract)
        self.assertEqual(a, b)

    def test_all_themes_generate_and_check_fresh(self):
        """Contract: every theme generates; --check passes against written files."""
        # Write into the real tree (idempotent) then check.
        for theme_id in list_themes():
            with self.subTest(theme=theme_id):
                self.assertEqual(gen_juce_tokens.write_theme(theme_id, check_only=False), 0)
                self.assertEqual(gen_juce_tokens.write_theme(theme_id, check_only=True), 0)

    def test_check_detects_staleness(self):
        """Contract: --check fails when DesignTokens.h does not match regeneration."""
        theme_id = "disklordz"
        path = gen_juce_tokens.output_path(theme_id)
        self.assertTrue(path.exists())
        original = path.read_text(encoding="utf-8")
        try:
            path.write_text(original + "\n// tampered\n", encoding="utf-8")
            self.assertEqual(gen_juce_tokens.write_theme(theme_id, check_only=True), 1)
        finally:
            path.write_text(original, encoding="utf-8")
            self.assertEqual(gen_juce_tokens.write_theme(theme_id, check_only=True), 0)

    def test_check_missing_fails(self):
        """Contract: --check fails when the generated header is absent."""
        with tempfile.TemporaryDirectory() as tmp:
            orig = gen_juce_tokens.GUI_GENERATED
            try:
                gen_juce_tokens.GUI_GENERATED = Path(tmp) / "generated"
                self.assertEqual(gen_juce_tokens.write_theme("disklordz", check_only=True), 1)
            finally:
                gen_juce_tokens.GUI_GENERATED = orig


if __name__ == "__main__":
    unittest.main()
