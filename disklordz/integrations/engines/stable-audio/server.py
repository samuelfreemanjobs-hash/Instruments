#!/usr/bin/env python3
"""Placeholder worker for Stability-AI/stable-audio-tools."""

from __future__ import annotations

import json
import sys
from http.server import BaseHTTPRequestHandler, HTTPServer


class Handler(BaseHTTPRequestHandler):
    def do_POST(self) -> None:
        self.send_response(503)
        self.send_header("Content-Type", "application/json")
        self.end_headers()
        self.wfile.write(
            json.dumps(
                {
                    "error": "stable_audio_not_wired",
                    "hint": "Install stable-audio-tools and call generate API here.",
                    "repo": "https://github.com/Stability-AI/stable-audio-tools",
                }
            ).encode()
        )


def main() -> None:
    port = int(sys.argv[1]) if len(sys.argv) > 1 else 8766
    HTTPServer(("0.0.0.0", port), Handler).serve_forever()


if __name__ == "__main__":
    main()
