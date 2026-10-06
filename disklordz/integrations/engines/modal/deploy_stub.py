#!/usr/bin/env python3
"""Modal GPU deploy stub (modal-labs/modal-client). Requires MODAL_TOKEN_ID + MODAL_TOKEN_SECRET."""

from __future__ import annotations

import json
import os
import sys


def main() -> None:
    if not os.environ.get("MODAL_TOKEN_ID"):
        print(
            json.dumps(
                {
                    "status": "stub",
                    "error": "modal_not_configured",
                    "hint": "pip install modal; set MODAL_TOKEN_ID / MODAL_TOKEN_SECRET",
                    "doc": "disklordz/integrations/engines/README.md",
                }
            )
        )
        sys.exit(0)
    try:
        import modal  # noqa: F401
    except ImportError:
        print(json.dumps({"error": "modal_not_installed", "hint": "pip install modal"}))
        sys.exit(1)
    print(json.dumps({"status": "ready", "message": "Extend deploy_stub.py with your Modal app"}))


if __name__ == "__main__":
    main()
