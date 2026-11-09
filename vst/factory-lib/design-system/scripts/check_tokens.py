#!/usr/bin/env python3
"""Validate tokens.json. Exit 0 only if every rule passes.

Usage:
    python3 vst/factory-lib/design-system/scripts/check_tokens.py [--check-preview]
"""
from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path

DS_ROOT = Path(__file__).resolve().parents[1]
TOKENS = DS_ROOT / "tokens.json"
PREVIEW = DS_ROOT / "preview.html"
HEX = re.compile(r"^#[0-9A-Fa-f]{6}$")
REF = re.compile(r"^\{([A-Za-z0-9_.]+)\}$")


def load_tokens(path: Path = TOKENS) -> dict:
    with path.open(encoding="utf-8") as f:
        return json.load(f)


def lookup(tokens: dict, dotted: str):
    node = tokens
    for part in dotted.split("."):
        if not isinstance(node, dict) or part not in node:
            raise KeyError(dotted)
        node = node[part]
    return node


def resolve(tokens: dict, value, seen: tuple[str, ...] = ()):
    """Follow {a.b.c} references to a literal value."""
    if isinstance(value, str):
        m = REF.match(value)
        if m:
            target = m.group(1)
            if target in seen:
                raise ValueError("reference cycle: " + " -> ".join(seen + (target,)))
            return resolve(tokens, lookup(tokens, target), seen + (target,))
    return value


def resolve_path(tokens: dict, dotted: str):
    return resolve(tokens, lookup(tokens, dotted), (dotted,))


def leaves(node, prefix: str = ""):
    if isinstance(node, dict):
        for k, v in node.items():
            yield from leaves(v, f"{prefix}.{k}" if prefix else k)
    else:
        yield prefix, node


def _lin(c: float) -> float:
    return c / 12.92 if c <= 0.03928 else ((c + 0.055) / 1.055) ** 2.4


def luminance(hex_color: str) -> float:
    r, g, b = (int(hex_color[i : i + 2], 16) / 255 for i in (1, 3, 5))
    return 0.2126 * _lin(r) + 0.7152 * _lin(g) + 0.0722 * _lin(b)


def contrast(fg: str, bg: str) -> float:
    a, b = luminance(fg), luminance(bg)
    hi, lo = max(a, b), min(a, b)
    return (hi + 0.05) / (lo + 0.05)


def check(tokens: dict) -> list[str]:
    errors: list[str] = []

    # 1. every colour leaf is hex or a resolvable reference to hex
    for section in ("color", "semantic"):
        for path, raw in leaves(tokens[section], section):
            try:
                val = resolve(tokens, raw, (path,))
            except (KeyError, ValueError) as exc:
                errors.append(f"{path}: unresolved reference {raw!r} ({exc})")
                continue
            if not (isinstance(val, str) and HEX.match(val)):
                errors.append(f"{path}: not a #RRGGBB colour ({val!r})")

    # 2. primitives restricted to certain semantic groups
    for rule in tokens.get("usageRestrictions", []):
        prim = rule["primitive"]
        allowed = tuple(rule["allowedUnder"])
        for path, raw in leaves(tokens["semantic"], "semantic"):
            m = REF.match(raw) if isinstance(raw, str) else None
            if m and (m.group(1) == prim or m.group(1).startswith(prim + ".")):
                if not any(path == a or path.startswith(a + ".") for a in allowed):
                    errors.append(f"{path} uses {prim} (allowed only under {', '.join(allowed)})")

    # 3. contrast pairs
    for pair in tokens.get("contrast", []):
        try:
            fg = resolve_path(tokens, pair["fg"])
            bg = resolve_path(tokens, pair["bg"])
        except (KeyError, ValueError) as exc:
            errors.append(f"contrast {pair['fg']} / {pair['bg']}: {exc}")
            continue
        if not all(isinstance(v, str) and HEX.match(v) for v in (fg, bg)):
            continue  # already reported by rule 1
        ratio = contrast(fg, bg)
        if ratio + 1e-9 < pair["min"]:
            errors.append(
                f"contrast {pair['fg']} {fg} on {pair['bg']} {bg} = {ratio:.2f} < {pair['min']} ({pair['use']})"
            )

    # 4. spacing on the 4 px grid
    for key, val in tokens["space"].items():
        if val % 4 != 0:
            errors.append(f"space.{key} = {val} is not a multiple of 4")

    # 5. typography floor
    typo = tokens["typography"]
    for name, style in typo["style"].items():
        if style["size"] < typo["minSize"]:
            errors.append(f"typography.style.{name} size {style['size']} < minSize {typo['minSize']}")
        if style["lineHeight"] < style["size"]:
            errors.append(f"typography.style.{name} lineHeight < size")
        if style["family"] not in typo["family"]:
            errors.append(f"typography.style.{name} family {style['family']!r} undefined")

    # 6. interactive sizes meet the hit-target minimum
    size = tokens["size"]
    floor = size["hitTargetMin"]
    for group in ("knob", "button", "toggle"):
        for key, val in size[group].items():
            if val < floor:
                errors.append(f"size.{group}.{key} = {val} < hitTargetMin {floor}")

    return errors


def contrast_report(tokens: dict) -> list[str]:
    rows = []
    for pair in tokens.get("contrast", []):
        try:
            ratio = contrast(resolve_path(tokens, pair["fg"]), resolve_path(tokens, pair["bg"]))
        except (KeyError, ValueError, TypeError):
            continue
        mark = "ok " if ratio + 1e-9 >= pair["min"] else "LOW"
        rows.append(f"  {mark} {ratio:5.2f} (min {pair['min']:>4}) {pair['fg']} on {pair['bg']}")
    return rows


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--check-preview", action="store_true", help="fail if preview.html is stale")
    ap.add_argument("--quiet", action="store_true")
    args = ap.parse_args()

    tokens = load_tokens()
    errors = check(tokens)

    if args.check_preview:
        sys.path.insert(0, str(Path(__file__).resolve().parent))
        from render_preview import render  # noqa: E402

        if not PREVIEW.exists() or PREVIEW.read_text(encoding="utf-8") != render(tokens):
            errors.append("preview.html is stale: run scripts/render_preview.py")

    if not args.quiet:
        print(f"{tokens['name']} v{tokens['version']}")
        print("\n".join(contrast_report(tokens)))
    if errors:
        print(f"\nFAIL ({len(errors)}):", file=sys.stderr)
        for e in errors:
            print(f"  - {e}", file=sys.stderr)
        return 1
    print("PASS tokens")
    return 0


if __name__ == "__main__":
    sys.exit(main())
