#!/usr/bin/env python3
"""Fail if any relative markdown link in the given docs points at a missing file.

Usage: python3 tools/check_doc_links.py [paths...]   (default: SOP-VF docs, templates, design system)
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[1]
DEFAULT = [
    "docs/disklordz/sops/SOP-VF-*.md",
    "docs/disklordz/README.md",
    "docs/disklordz/templates/TEMPLATE-factory-cloud-agent-prompt.md",
    "templates/apc/README.md",
    "vst/factory-lib/README.md",
    "vst/factory-lib/design-system/*.md",
    "vst/factory-lib/design-system/themes/*/THEME.md",
]
LINK = re.compile(r"\[[^\]]*\]\(([^)\s]+)\)")


def broken_links(md: Path) -> list[str]:
    out = []
    text = md.read_text(encoding="utf-8")
    text = re.sub(r"```.*?```", "", text, flags=re.S)
    for target in LINK.findall(text):
        if re.match(r"^[a-z]+:", target) or target.startswith("#"):
            continue
        path = target.split("#", 1)[0]
        if not path:
            continue
        if not (md.parent / path).resolve().exists():
            out.append(f"{md.relative_to(REPO)}: {target}")
    return out


def main(argv: list[str]) -> int:
    patterns = argv or DEFAULT
    files = sorted({p for pat in patterns for p in REPO.glob(pat)})
    errors = [e for f in files for e in broken_links(f)]
    print(f"checked {len(files)} files")
    for e in errors:
        print(f"BROKEN {e}", file=sys.stderr)
    return 1 if errors else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
