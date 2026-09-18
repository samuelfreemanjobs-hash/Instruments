"""Load prompts and build LLM request bundles."""

from __future__ import annotations

import json
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[3]
PROMPTS = REPO_ROOT / "docs" / "business-agents" / "prompts"


def read_prompt(name: str) -> str:
    path = PROMPTS / name
    if not path.is_file():
        raise FileError(f"Missing prompt: {path}")
    return path.read_text(encoding="utf-8")


def build_messages(system_file: str, user_content: str) -> list[dict[str, str]]:
    return [
        {"role": "system", "content": read_prompt(system_file)},
        {"role": "user", "content": user_content},
    ]


def format_prompt_bundle(agent: str, system_file: str, user_content: str) -> str:
    """Human/CLI path: paste into Cursor or Claude Code."""
    lines = [
        f"# Business agent: {agent}",
        "",
        "## System",
        read_prompt(system_file),
        "",
        "## User",
        user_content,
        "",
        "---",
        "Save model JSON output under business-agents/ then run:",
        f"  python3 scripts/business-agents/business_agent.py {agent} validate --file <path>",
    ]
    return "\n".join(lines)


def parse_intake_text(path: Path) -> dict:
    text = path.read_text(encoding="utf-8")
    if path.suffix.lower() == ".json":
        return json.loads(text)
    # Minimal YAML-ish: key: value and multiline |
    data: dict = {}
    current_key = None
    buf: list[str] = []
    for line in text.splitlines():
        if line.strip().startswith("#") or not line.strip():
            continue
        if ":" in line and not line.startswith(" ") and not line.startswith("\t"):
            if current_key and buf:
                data[current_key] = "\n".join(buf).strip()
                buf = []
            key, _, rest = line.partition(":")
            current_key = key.strip()
            rest = rest.strip()
            if rest == "|":
                continue
            if rest:
                data[current_key] = rest
                current_key = None
        elif current_key:
            buf.append(line.rstrip())
    if current_key and buf:
        data[current_key] = "\n".join(buf).strip()
    return data
