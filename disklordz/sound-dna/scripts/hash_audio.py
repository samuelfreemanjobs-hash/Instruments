#!/usr/bin/env python3
"""Compute SHA-256 of a WAV/file for Sound DNA registry and dedup."""

import hashlib
import sys
from pathlib import Path


def sha256_file(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1024 * 1024), b""):
            h.update(chunk)
    return h.hexdigest()


def main() -> None:
    if len(sys.argv) != 2:
        print("Usage: hash_audio.py <file>", file=sys.stderr)
        sys.exit(2)
    p = Path(sys.argv[1])
    if not p.is_file():
        print(f"Not found: {p}", file=sys.stderr)
        sys.exit(1)
    print(sha256_file(p))


if __name__ == "__main__":
    main()
