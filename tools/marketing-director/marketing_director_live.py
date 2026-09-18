#!/usr/bin/env python3
"""Marketing Director — local CLI entry (requires ANTHROPIC_API_KEY)."""

from __future__ import annotations

import json
import logging
import os
import sys

try:
    from anthropic import Anthropic
except ImportError:
    print("pip install anthropic", file=sys.stderr)
    raise SystemExit(1)

from marketing_director import MarketingDirector


def main() -> int:
    logging.basicConfig(level=logging.INFO)
    if len(sys.argv) < 2:
        print("Usage: marketing_director_live.py '<request>'", file=sys.stderr)
        return 1
    client = Anthropic(api_key=os.environ.get("ANTHROPIC_API_KEY"))
    director = MarketingDirector(client)
    result = director.handle_request(sys.argv[1])
    print(json.dumps(result, indent=2))
    return 0 if result.get("status") == "ok" else 1


if __name__ == "__main__":
    raise SystemExit(main())
