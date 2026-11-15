#!/usr/bin/env python3
"""Validate design-system themes against the role contract. Exit 0 only if every rule passes.

Usage:
    python3 vst/factory-lib/design-system/scripts/check_tokens.py              # all themes
    python3 vst/factory-lib/design-system/scripts/check_tokens.py --theme disklordz
    python3 vst/factory-lib/design-system/scripts/check_tokens.py --check-preview
    python3 vst/factory-lib/design-system/scripts/check_tokens.py --check-generated
"""
from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path

DS_ROOT = Path(__file__).resolve().parents[1]
CONTRACT = DS_ROOT / "contract" / "roles.json"
THEMES = DS_ROOT / "themes"
PREVIEWS = DS_ROOT / "previews"
HEX = re.compile(r"^#[0-9A-Fa-f]{6}$")
REF = re.compile(r"^\{([A-Za-z0-9_.]+)\}$")
THEME_ID = re.compile(r"^[a-z_][a-z0-9-]*$")
HEADER_FIELDS = ("id", "name", "brandName", "version", "contract")


def load_json(path: Path) -> dict:
    with path.open(encoding="utf-8") as f:
        return json.load(f)


def load_contract() -> dict:
    return load_json(CONTRACT)


def list_themes() -> list[str]:
    return sorted(p.parent.name for p in THEMES.glob("*/tokens.json"))


def load_theme(theme_id: str) -> dict:
    return load_json(THEMES / theme_id / "tokens.json")


def preview_path(theme_id: str) -> Path:
    return PREVIEWS / f"{theme_id}.html"


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


def effective_pairs(tokens: dict, contract: dict) -> list[dict]:
    """Contract baseline pairs, tightened (never loosened) by the theme's own pairs."""
    pairs = {(p["fg"], p["bg"]): dict(p) for p in contract["baselineContrast"]}
    for p in tokens.get("contrast", []):
        key = (p["fg"], p["bg"])
        if key in pairs:
            pairs[key]["min"] = max(pairs[key]["min"], p["min"])
        else:
            pairs[key] = dict(p)
    return list(pairs.values())


def check(tokens: dict, contract: dict | None = None) -> list[str]:
    contract = contract or load_contract()
    floors = contract["floors"]
    errors: list[str] = []

    # 0. theme header and contract version
    for field in HEADER_FIELDS:
        if field not in tokens:
            errors.append(f"theme header missing '{field}'")
    if "id" in tokens and not THEME_ID.match(str(tokens["id"])):
        errors.append(f"theme id {tokens['id']!r} must be lowercase-kebab")
    if "contract" in tokens:
        if str(tokens["contract"]).split(".")[0] != contract["contractVersion"].split(".")[0]:
            errors.append(f"theme targets contract {tokens['contract']}, contract is {contract['contractVersion']}")

    # 1. every role the components bind to is defined
    missing = []
    for path in contract["requiredPaths"]:
        try:
            lookup(tokens, path)
        except KeyError:
            missing.append(path)
    errors += [f"missing required role {p}" for p in missing]
    if missing:
        return errors  # later rules would cascade

    # 2. every colour leaf is hex or a resolvable reference to hex
    for section in ("color", "semantic"):
        for path, raw in leaves(tokens.get(section, {}), section):
            try:
                val = resolve(tokens, raw, (path,))
            except (KeyError, ValueError) as exc:
                errors.append(f"{path}: unresolved reference {raw!r} ({exc})")
                continue
            if not (isinstance(val, str) and HEX.match(val)):
                errors.append(f"{path}: not a #RRGGBB colour ({val!r})")

    # 3. primitives restricted to certain semantic groups (theme policy)
    for rule in tokens.get("usageRestrictions", []):
        prim = rule["primitive"]
        allowed = tuple(rule["allowedUnder"])
        for path, raw in leaves(tokens["semantic"], "semantic"):
            m = REF.match(raw) if isinstance(raw, str) else None
            if m and (m.group(1) == prim or m.group(1).startswith(prim + ".")):
                if not any(path == a or path.startswith(a + ".") for a in allowed):
                    errors.append(f"{path} uses {prim} (allowed only under {', '.join(allowed)})")

    # 4. theme pairs may tighten but never loosen the contract
    baseline = {(p["fg"], p["bg"]): p["min"] for p in contract["baselineContrast"]}
    for p in tokens.get("contrast", []):
        floor = baseline.get((p["fg"], p["bg"]))
        if floor is not None and p["min"] < floor:
            errors.append(f"theme loosens contrast {p['fg']} / {p['bg']} to {p['min']} (contract {floor})")

    # 5. contrast pairs
    for pair in effective_pairs(tokens, contract):
        try:
            fg = resolve_path(tokens, pair["fg"])
            bg = resolve_path(tokens, pair["bg"])
        except (KeyError, ValueError) as exc:
            errors.append(f"contrast {pair['fg']} / {pair['bg']}: {exc}")
            continue
        if not all(isinstance(v, str) and HEX.match(v) for v in (fg, bg)):
            continue  # already reported by rule 2
        ratio = contrast(fg, bg)
        if ratio + 1e-9 < pair["min"]:
            errors.append(
                f"contrast {pair['fg']} {fg} on {pair['bg']} {bg} = {ratio:.2f} < {pair['min']} ({pair['use']})"
            )

    # 6. spacing grid
    grid = floors["spaceGrid"]
    for key, val in tokens["space"].items():
        if val % grid != 0:
            errors.append(f"space.{key} = {val} is not a multiple of {grid}")

    # 7. typography floor
    typo = tokens["typography"]
    if typo["minSize"] < floors["minTextSize"]:
        errors.append(f"typography.minSize {typo['minSize']} < contract floor {floors['minTextSize']}")
    for name, style in typo["style"].items():
        if style["size"] < max(typo["minSize"], floors["minTextSize"]):
            errors.append(f"typography.style.{name} size {style['size']} < minSize {typo['minSize']}")
        if style["lineHeight"] < style["size"]:
            errors.append(f"typography.style.{name} lineHeight < size")
        if style["family"] not in typo["family"]:
            errors.append(f"typography.style.{name} family {style['family']!r} undefined")

    # 8. interactive sizes meet the hit-target floor
    size = tokens["size"]
    if size["hitTargetMin"] < floors["hitTargetMin"]:
        errors.append(f"size.hitTargetMin {size['hitTargetMin']} < contract floor {floors['hitTargetMin']}")
    target = max(size["hitTargetMin"], floors["hitTargetMin"])
    for group in ("knob", "button", "toggle"):
        for key, val in size[group].items():
            if val < target:
                errors.append(f"size.{group}.{key} = {val} < hitTargetMin {target}")

    return errors


def contrast_report(tokens: dict, contract: dict) -> list[str]:
    rows = []
    for pair in effective_pairs(tokens, contract):
        try:
            ratio = contrast(resolve_path(tokens, pair["fg"]), resolve_path(tokens, pair["bg"]))
        except (KeyError, ValueError, TypeError):
            continue
        mark = "ok " if ratio + 1e-9 >= pair["min"] else "LOW"
        rows.append(f"  {mark} {ratio:5.2f} (min {pair['min']:>4}) {pair['fg']} on {pair['bg']}")
    return rows


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--theme", action="append", help="theme id (repeatable); default: all themes")
    ap.add_argument("--check-preview", action="store_true", help="fail if previews/<theme>.html is stale")
    ap.add_argument(
        "--check-generated",
        action="store_true",
        help="fail if gui/generated/<theme>/DesignTokens.h is stale (runs gen_juce_tokens --check)",
    )
    ap.add_argument("--quiet", action="store_true")
    args = ap.parse_args()

    contract = load_contract()
    themes = args.theme or list_themes()
    failed = 0
    for theme_id in themes:
        tokens = load_theme(theme_id)
        errors = check(tokens, contract)
        if tokens.get("id") != theme_id:
            errors.append(f"theme id {tokens.get('id')!r} does not match folder {theme_id!r}")
        if args.check_preview and not errors:
            sys.path.insert(0, str(Path(__file__).resolve().parent))
            from render_preview import render  # noqa: E402

            out = preview_path(theme_id)
            if not out.exists() or out.read_text(encoding="utf-8") != render(tokens, contract):
                errors.append(f"previews/{theme_id}.html is stale: run scripts/render_preview.py")
        if not args.quiet:
            print(f"[{theme_id}] {tokens.get('name')} v{tokens.get('version')} (contract {contract['contractVersion']})")
            if not any(e.startswith("missing required role") for e in errors):
                print("\n".join(contrast_report(tokens, contract)))
        if errors:
            failed += 1
            print(f"FAIL [{theme_id}] ({len(errors)}):", file=sys.stderr)
            for e in errors:
                print(f"  - {e}", file=sys.stderr)
        else:
            print(f"PASS [{theme_id}]")

    if args.check_generated and failed == 0:
        sys.path.insert(0, str(Path(__file__).resolve().parent))
        import gen_juce_tokens  # noqa: E402

        for theme_id in themes:
            if gen_juce_tokens.write_theme(theme_id, check_only=True) != 0:
                failed = 1
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
