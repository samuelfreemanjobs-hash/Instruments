#!/usr/bin/env python3
"""Minimal HTTP shim for facebookresearch/audiocraft (install separately)."""

from __future__ import annotations

import json
import sys
from http.server import BaseHTTPRequestHandler, HTTPServer


class Handler(BaseHTTPRequestHandler):
    def do_POST(self) -> None:
        length = int(self.headers.get("Content-Length", "0"))
        body = json.loads(self.rfile.read(length) or b"{}")
        try:
            import numpy as np  # noqa: F401 — optional when audiocraft installed
            from audiocraft.models import AudioGen  # type: ignore
        except ImportError:
            self.send_response(503)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            self.wfile.write(
                json.dumps(
                    {
                        "error": "audiocraft_not_installed",
                        "hint": "pip install audiocraft (see github.com/facebookresearch/audiocraft)",
                    }
                ).encode()
            )
            return

        prompt = body.get("prompt", "drum kick")
        model = AudioGen.get_pretrained("facebook/audiogen-medium")
        wav = model.generate([prompt], progress=False)[0].cpu().numpy().flatten()
        pcm = wav.tolist()
        out = json.dumps({"pcm": pcm, "sampleRate": 16000}).encode()
        self.send_response(200)
        self.send_header("Content-Type", "application/json")
        self.end_headers()
        self.wfile.write(out)

    def log_message(self, fmt: str, *args: object) -> None:
        sys.stderr.write(fmt % args + "\n")


def main() -> None:
    port = int(sys.argv[1]) if len(sys.argv) > 1 else 8765
    HTTPServer(("0.0.0.0", port), Handler).serve_forever()


if __name__ == "__main__":
    main()
