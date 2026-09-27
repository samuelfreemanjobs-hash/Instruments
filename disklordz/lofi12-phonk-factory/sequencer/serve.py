#!/usr/bin/env python3
"""Serve sequencer UI + API (render loop, load pattern from groove)."""

from __future__ import annotations

import argparse
import json
import struct
import sys
import wave
import io
from http.server import BaseHTTPRequestHandler, HTTPServer
from pathlib import Path
from urllib.parse import urlparse

STATIC = Path(__file__).resolve().parent / "static"
SCRIPTS = Path(__file__).resolve().parent.parent / "scripts"
SEQ_SCRIPTS = Path(__file__).resolve().parent / "scripts"
sys.path.insert(0, str(SCRIPTS))
sys.path.insert(0, str(SEQ_SCRIPTS))


def _pcm_to_wav_bytes(pcm: list[float], rate: int = 44100) -> bytes:
    buf = io.BytesIO()
    with wave.open(buf, "wb") as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(rate)
        frames = b"".join(
            struct.pack("<h", int(max(-1.0, min(1.0, s)) * 32767)) for s in pcm
        )
        w.writeframes(frames)
    return buf.getvalue()


class Handler(BaseHTTPRequestHandler):
    def log_message(self, fmt: str, *args) -> None:
        pass

    def do_GET(self) -> None:
        path = urlparse(self.path).path
        if path in ("", "/"):
            path = "/index.html"
        file_path = STATIC / path.lstrip("/")
        if not file_path.is_file() or not str(file_path.resolve()).startswith(str(STATIC.resolve())):
            self.send_error(404)
            return
        data = file_path.read_bytes()
        ctype = "text/html" if file_path.suffix == ".html" else "application/javascript"
        if file_path.suffix == ".css":
            ctype = "text/css"
        self.send_response(200)
        self.send_header("Content-Type", ctype)
        self.send_header("Content-Length", str(len(data)))
        self.end_headers()
        self.wfile.write(data)

    def do_POST(self) -> None:
        path = urlparse(self.path).path
        length = int(self.headers.get("Content-Length", 0))
        body = self.rfile.read(length) if length else b"{}"
        try:
            payload = json.loads(body.decode("utf-8"))
        except json.JSONDecodeError:
            self.send_error(400, "Invalid JSON")
            return

        if path == "/api/render_loop":
            from phonk_loop_fx import LoopFxParams
            from phonk_loop_render import render_phonk_loop

            prompt = str(payload.get("prompt", "juicy j memphis phonk 84"))
            bpm = float(payload.get("bpm", 84))
            bars = int(payload.get("bars", 2))
            variation = int(payload.get("variation", 0))
            fx_raw = payload.get("fx") or {}
            fx = LoopFxParams(
                filter_cutoff=float(fx_raw.get("filter", 0.65)),
                reverb_send=float(fx_raw.get("reverb", 0.25)),
                tape=float(fx_raw.get("tape", 0.2)),
                drive=float(fx_raw.get("drive", 0.15)),
            )
            pcm, meta = render_phonk_loop(
                prompt=prompt,
                bpm=bpm,
                bars=bars,
                variation=variation,
                engine_id=payload.get("engine"),
                fx=fx,
            )
            wav = _pcm_to_wav_bytes(pcm)
            self.send_response(200)
            self.send_header("Content-Type", "audio/wav")
            self.send_header("X-Loop-Meta", json.dumps(meta))
            self.send_header("Content-Length", str(len(wav)))
            self.end_headers()
            self.wfile.write(wav)
            return

        if path == "/api/groove_pattern":
            from pattern_from_groove import groove_to_pattern
            from pattern_schema import Pattern

            prompt = str(payload.get("prompt", "dj paul memphis 84"))
            bpm = float(payload.get("bpm", 84))
            pat: Pattern = groove_to_pattern(prompt, bpm, int(payload.get("variation", 0)))
            data = json.dumps(pat.to_dict()).encode("utf-8")
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.send_header("Content-Length", str(len(data)))
            self.end_headers()
            self.wfile.write(data)
            return

        self.send_error(404)


def main() -> None:
    p = argparse.ArgumentParser()
    p.add_argument("--host", default="127.0.0.1")
    p.add_argument("--port", type=int, default=8765)
    args = p.parse_args()
    httpd = HTTPServer((args.host, args.port), Handler)
    print(f"Sequencer + API → http://{args.host}:{args.port}")
    httpd.serve_forever()


if __name__ == "__main__":
    main()
