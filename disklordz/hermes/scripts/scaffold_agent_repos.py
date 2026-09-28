#!/usr/bin/env python3
"""Scaffold per-seat Hermes agent repos from _template."""

from __future__ import annotations

import shutil
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[3]
AGENT_ROOT = REPO_ROOT / "disklordz" / "hermes" / "agent-repos"
TEMPLATE = AGENT_ROOT / "_template"

SEATS: dict[str, tuple[str, str]] = {
    "hermes-lead": ("hermes-elite-lead", "lead.md"),
    "hermes-architect": ("hermes-elite-architect", "architect.md"),
    "hermes-dsp": ("hermes-elite-dsp", "dsp-engineer.md"),
    "hermes-gui": ("hermes-elite-gui", "gui-engineer.md"),
    "hermes-web": ("hermes-elite-web", "web-engineer.md"),
    "hermes-qa": ("hermes-elite-qa", "qa-engineer.md"),
    "hermes-research": ("hermes-elite-research", "research-engineer.md"),
    "hermes-devops": ("hermes-elite-devops", "devops-engineer.md"),
    "hermes-handoff": ("hermes-elite-handoff", "handoff-engineer.md"),
    "hermes-ops": ("hermes-elite-ops", "ops-engineer.md"),
    "hermes-gtm": ("hermes-elite-gtm", "gtm-engineer.md"),
    "hermes-presets": ("hermes-elite-presets", "presets-engineer.md"),
    "hermes-support": ("hermes-elite-support", "support-engineer.md"),
    "hermes-security": ("hermes-elite-security", "security-engineer.md"),
    "hermes-data": ("hermes-elite-data", "data-engineer.md"),
    "hermes-sop": ("hermes-elite-sop", "sop-procedures.md"),
}


def substitute(text: str, seat_id: str, skill_pkg: str, charter: str) -> str:
    skill_path = f".cursor/skills/{skill_pkg}/SKILL.md"
    charter_path = f".cursor/hermes/seats/{charter}"
    return (
        text.replace("{{SEAT_ID}}", seat_id)
        .replace("{{SKILL_PATH}}", skill_path)
        .replace("{{CHARTER_PATH}}", charter_path)
    )


def scaffold_one(seat_id: str, skill_pkg: str, charter: str, force: bool = False) -> Path:
    dest = AGENT_ROOT / seat_id
    if dest.exists() and not force:
        return dest
    if dest.exists() and force:
        shutil.rmtree(dest)
    shutil.copytree(TEMPLATE, dest)
    for name in ("README.md", "PLAYBOOK.local.md", "CHANGELOG.md"):
        p = dest / name
        p.write_text(substitute(p.read_text(encoding="utf-8"), seat_id, skill_pkg, charter), encoding="utf-8")
    return dest


def main() -> None:
    for seat_id, (skill_pkg, charter) in SEATS.items():
        path = scaffold_one(seat_id, skill_pkg, charter)
        print(f"ok {path.relative_to(REPO_ROOT)}")


if __name__ == "__main__":
    main()
