#!/usr/bin/env python3
"""Convert arbitrary files to markdown for the RAG corpus (requires markitdown)."""

from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path

RAG_ROOT = Path(__file__).resolve().parents[1]
CORPUS_OUT = RAG_ROOT / "corpus" / "imports"


def main() -> None:
    parser = argparse.ArgumentParser(description="Ingest files via microsoft/markitdown CLI")
    parser.add_argument("inputs", nargs="+", help="PDF, DOCX, etc.")
    args = parser.parse_args()

    CORPUS_OUT.mkdir(parents=True, exist_ok=True)
    for raw in args.inputs:
        src = Path(raw)
        if not src.is_file():
            print(f"skip missing: {src}", file=sys.stderr)
            continue
        out = CORPUS_OUT / f"{src.stem}.md"
        try:
            subprocess.run(
                ["markitdown", str(src), "-o", str(out)],
                check=True,
                capture_output=True,
                text=True,
            )
            print(f"Wrote {out}")
        except FileNotFoundError:
            print(
                "Install markitdown: pip install 'markitdown[all]' "
                "(https://github.com/microsoft/markitdown)",
                file=sys.stderr,
            )
            sys.exit(1)
        except subprocess.CalledProcessError as exc:
            print(exc.stderr or exc.stdout, file=sys.stderr)
            sys.exit(exc.returncode)


if __name__ == "__main__":
    main()
