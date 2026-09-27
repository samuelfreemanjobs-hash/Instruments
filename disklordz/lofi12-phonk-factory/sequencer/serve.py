#!/usr/bin/env python3
"""Serve the Lofi-12 step sequencer UI."""

from __future__ import annotations

import argparse
import http.server
import socketserver
from pathlib import Path

ROOT = Path(__file__).resolve().parent / "static"


class Handler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=str(ROOT), **kwargs)


def main() -> None:
    p = argparse.ArgumentParser()
    p.add_argument("--host", default="127.0.0.1")
    p.add_argument("--port", type=int, default=8765)
    args = p.parse_args()
    with socketserver.TCPServer((args.host, args.port), Handler) as httpd:
        print(f"Lofi-12 sequencer → http://{args.host}:{args.port}")
        print("Use Chrome/Edge; connect USB-MIDI to Lofi-12 MIDI IN.")
        httpd.serve_forever()


if __name__ == "__main__":
    main()
