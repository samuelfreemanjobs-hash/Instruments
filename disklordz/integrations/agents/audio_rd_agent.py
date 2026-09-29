#!/usr/bin/env python3
"""Audio R&D agent — experiment log + partner agent checklist."""

from __future__ import annotations

import argparse
from pathlib import Path

REPO = Path(__file__).resolve().parents[3]
LOG = REPO / "docs" / "RD_EXPERIMENT_LOG.md"
PARTNERS = {
    "ddsp-ml-engineer": REPO / "disklordz" / "integrations" / "agents" / "ddsp_ml_engineer.py",
    "audio-plugin-coder": REPO / "disklordz" / "integrations" / "agents" / "audio_plugin_coder_agent.py",
}


def main() -> int:
    parser = argparse.ArgumentParser(description="Audio R&D fleet helper")
    parser.add_argument("--self-test", action="store_true")
    parser.add_argument("--list-partners", action="store_true")
    args = parser.parse_args()

    if args.self_test:
        if not LOG.is_file():
            raise SystemExit(f"missing {LOG}")
        for name, path in PARTNERS.items():
            if not path.is_file():
                raise SystemExit(f"missing partner CLI: {name} {path}")
        print("audio-rd: ok", LOG.name)
        return 0

    if args.list_partners:
        for name, path in PARTNERS.items():
            print(name, path.relative_to(REPO))
        return 0

    print("Audio R&D — read:", LOG)
    print("Partners: ddsp-ml-engineer (train/export), audio-plugin-coder (JUCE/APC)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
