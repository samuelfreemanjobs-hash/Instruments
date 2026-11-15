#!/usr/bin/env python3
"""Instantiate APC / DPCS phase templates for a plugin.

Usage:
    python3 tools/apc_templates.py <Name> <phase> [--class effect] [--category fx.x] [--theme disklordz] [--force]
    python3 tools/apc_templates.py --list

Phases: dream, plan, design, verify, release, all
Never overwrites an existing file unless --force. See templates/apc/README.md.
"""
from __future__ import annotations

import argparse
import datetime as dt
import re
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[1]
TEMPLATES = REPO / "templates" / "apc"
NAME_RE = re.compile(r"^[A-Z][A-Za-z0-9]{1,39}$")

PHASES: dict[str, list[tuple[str, str]]] = {
    "dream": [
        ("constitution.template.md", "DPCS_PHASE0_CONSTITUTION.md"),
        ("creative-brief.template.md", ".ideas/creative-brief.md"),
        ("parameter-spec.template.md", ".ideas/parameter-spec.md"),
    ],
    "plan": [
        ("architecture.template.md", ".ideas/architecture.md"),
        ("apc-plan.template.md", ".ideas/apc-plan.md"),
    ],
    "design": [
        ("design-spec.template.md", ".ideas/design-spec.md"),
    ],
    "verify": [
        ("verify-phase1.template.sh", "scripts/verify-phase1.sh"),
        ("verify-phase4.template.sh", "scripts/verify-phase4.sh"),
    ],
    "release": [
        ("release-notes.template.md", "RELEASE_NOTES.md"),
    ],
}
PHASES["all"] = [item for key in ("dream", "plan", "design", "verify", "release") for item in PHASES[key]]


def render(text: str, values: dict[str, str]) -> str:
    for key, val in values.items():
        text = text.replace("{{" + key + "}}", val)
    return text


def instantiate(name: str, phase: str, *, plugin_class: str = "effect", category: str = "",
                theme: str = "disklordz", version: str = "1.0.0", force: bool = False,
                plugins_dir: Path | None = None) -> list[Path]:
    if not NAME_RE.match(name):
        raise ValueError(f"plugin name {name!r} must be PascalCase letters/digits (e.g. MeterTap)")
    if phase not in PHASES:
        raise ValueError(f"unknown phase {phase!r}; choose from {', '.join(PHASES)}")
    values = {
        "PLUGIN_NAME": name,
        "PLUGIN_CLASS": plugin_class,
        "CATEGORY": category or f"{plugin_class}.uncategorised",
        "THEME": theme,
        "VERSION": version,
        "DATE": dt.date.today().isoformat(),
    }
    root = (plugins_dir or REPO / "plugins") / name
    written: list[Path] = []
    for tpl, rel in PHASES[phase]:
        dst = root / rel
        if dst.exists() and not force:
            continue
        dst.parent.mkdir(parents=True, exist_ok=True)
        text = render((TEMPLATES / tpl).read_text(encoding="utf-8"), values)
        leftover = re.findall(r"\{\{[A-Z_]+\}\}", text)
        if leftover:
            raise RuntimeError(f"{tpl}: unfilled placeholders {sorted(set(leftover))}")
        dst.write_text(text, encoding="utf-8", newline="\n")
        if dst.suffix == ".sh":
            dst.chmod(0o755)
        written.append(dst)
    return written


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("name", nargs="?")
    ap.add_argument("phase", nargs="?")
    ap.add_argument("--class", dest="plugin_class", default="effect", choices=["effect", "synth", "midi", "sampler"])
    ap.add_argument("--category", default="")
    ap.add_argument("--theme", default="disklordz")
    ap.add_argument("--version", default="1.0.0")
    ap.add_argument("--force", action="store_true", help="overwrite existing files")
    ap.add_argument("--list", action="store_true", help="list phases and the files they create")
    args = ap.parse_args()

    if args.list or not (args.name and args.phase):
        for phase, items in PHASES.items():
            if phase == "all":
                continue
            print(f"{phase}:")
            for tpl, rel in items:
                print(f"  templates/apc/{tpl} -> plugins/<Name>/{rel}")
        return 0 if args.list else 1
    try:
        written = instantiate(args.name, args.phase, plugin_class=args.plugin_class, category=args.category,
                              theme=args.theme, version=args.version, force=args.force)
    except (ValueError, RuntimeError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1
    for p in written:
        print(f"wrote {p.relative_to(REPO)}")
    if not written:
        print("nothing written (files exist; use --force to overwrite)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
