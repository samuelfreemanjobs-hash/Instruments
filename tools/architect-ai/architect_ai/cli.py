#!/usr/bin/env python3
"""Run ArchitectAI locally (Gemini or OpenAI)."""

from __future__ import annotations

import argparse
import os
import sys
from pathlib import Path

# Allow `python architect_ai/cli.py` from tools/architect-ai/
_PKG_ROOT = Path(__file__).resolve().parent.parent
if str(_PKG_ROOT) not in sys.path:
    sys.path.insert(0, str(_PKG_ROOT))

from architect_ai.context import expand_paths, read_context_paths  # noqa: E402
from architect_ai.prompts import ARCHITECTAI_SYSTEM_PROMPT  # noqa: E402
from architect_ai.session import ArchitectSession  # noqa: E402


def build_model(provider: str):
    if provider == "gemini":
        from architect_ai.adapters.gemini import GeminiChatModel

        return GeminiChatModel()
    if provider == "openai":
        from architect_ai.adapters.openai_chat import OpenAIChatModel

        return OpenAIChatModel()
    raise ValueError(f"Unknown provider: {provider}")


def load_dotenv_if_present() -> None:
    env_path = _PKG_ROOT / ".env"
    if not env_path.is_file():
        return
    for line in env_path.read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if not line or line.startswith("#") or "=" not in line:
            continue
        key, _, val = line.partition("=")
        key = key.strip()
        val = val.strip().strip('"').strip("'")
        os.environ.setdefault(key, val)


def main(argv: list[str] | None = None) -> int:
    load_dotenv_if_present()
    parser = argparse.ArgumentParser(
        description="ArchitectAI — COSI-grounded architecture mentor (local CLI)"
    )
    parser.add_argument(
        "--provider",
        choices=("gemini", "openai"),
        default=os.environ.get("ARCHITECTAI_PROVIDER", "gemini"),
    )
    parser.add_argument(
        "--cosi",
        action="store_true",
        help="Append COSI section template to every user turn",
    )
    parser.add_argument(
        "--review",
        nargs="+",
        metavar="PATH",
        help="Attach file or directory text (skips secrets/binaries)",
    )
    parser.add_argument(
        "--once",
        metavar="PROMPT",
        help="Single-shot question then exit (non-interactive)",
    )
    parser.add_argument(
        "--print-system",
        action="store_true",
        help="Print system prompt and exit (no API call)",
    )
    args = parser.parse_args(argv)

    if args.print_system:
        print(ARCHITECTAI_SYSTEM_PROMPT)
        return 0

    try:
        model = build_model(args.provider)
    except RuntimeError as exc:
        print(f"Error: {exc}", file=sys.stderr)
        return 2

    session = ArchitectSession(model, force_cosi=args.cosi)
    context_block: str | None = None
    if args.review:
        paths = expand_paths(args.review)
        context_block = read_context_paths(paths)
        if not context_block.strip():
            print("Warning: no readable context from --review paths", file=sys.stderr)

    def run_turn(user_line: str) -> None:
        extra = context_block if context_block else None
        reply = session.ask(user_line, extra_system=extra)
        print(reply)
        print()

    if args.once:
        run_turn(args.once)
        return 0

    print("ArchitectAI (COSI mentor). Commands: /reset /quit")
    print(f"Provider: {args.provider}\n")
    while True:
        try:
            line = input("you> ").strip()
        except (EOFError, KeyboardInterrupt):
            print()
            break
        if not line:
            continue
        if line in ("/quit", "/exit", "quit", "exit"):
            break
        if line == "/reset":
            session.reset()
            print("(history cleared)\n")
            continue
        run_turn(line)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
