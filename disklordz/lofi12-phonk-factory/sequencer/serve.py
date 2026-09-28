#!/usr/bin/env python3
"""Serve sequencer UI + API (render loop, load pattern from groove)."""

from __future__ import annotations

import argparse
import io
import json
import struct
import sys
import tempfile
import wave
import zipfile
from http.server import BaseHTTPRequestHandler, HTTPServer
from pathlib import Path
from urllib.parse import urlparse

STATIC = Path(__file__).resolve().parent / "static"
SCRIPTS = Path(__file__).resolve().parent.parent / "scripts"
SEQ_SCRIPTS = Path(__file__).resolve().parent / "scripts"
sys.path.insert(0, str(SCRIPTS))
sys.path.insert(0, str(SEQ_SCRIPTS))


def _fx_from_payload(fx_raw: dict) -> "LoopFxParams":
    from phonk_loop_fx import LoopFxParams

    return LoopFxParams(
        filter_cutoff=float(fx_raw.get("filter", 0.65)),
        reverb_send=float(fx_raw.get("reverb", 0.25)),
        tape=float(fx_raw.get("tape", 0.2)),
        drive=float(fx_raw.get("drive", 0.15)),
        cassette=float(fx_raw.get("cassette", 0.0)),
        bitcrush=float(fx_raw.get("bitcrush", 0.0)),
    )


def _send_wav(handler: BaseHTTPRequestHandler, pcm: list[float], meta: dict) -> None:
    wav = _pcm_to_wav_bytes(pcm)
    handler.send_response(200)
    handler.send_header("Content-Type", "audio/wav")
    handler.send_header("X-Loop-Meta", json.dumps(meta))
    handler.send_header("Content-Length", str(len(wav)))
    handler.end_headers()
    handler.wfile.write(wav)


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
        if path == "/api/presets":
            from phonk_style_presets import all_presets_api

            data = json.dumps(all_presets_api()).encode("utf-8")
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.send_header("Content-Length", str(len(data)))
            self.end_headers()
            self.wfile.write(data)
            return
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
            from phonk_style_presets import merge_render_payload

            payload = merge_render_payload(payload)
            prompt = str(payload.get("prompt", "juicy j dj paul dirty memphis 86"))
            bpm = float(payload.get("bpm", 84))
            bars = int(payload.get("bars", 2))
            variation = int(payload.get("variation", 0))
            fx = _fx_from_payload(payload.get("fx") or {})
            pcm, meta = render_phonk_loop(
                prompt=prompt,
                bpm=bpm,
                bars=bars,
                variation=variation,
                lane_id=payload.get("lane"),
                engine_id=payload.get("engine"),
                fx=fx,
                stem=payload.get("stem"),
            )
            _send_wav(self, pcm, meta)
            return

        if path == "/api/render_pattern_stem":
            from phonk_pattern_render import render_pattern_stem

            pattern = payload.get("pattern")
            if not isinstance(pattern, dict):
                self.send_error(400, "pattern required")
                return
            track_index = int(payload.get("trackIndex", 0))
            prompt = str(payload.get("prompt", pattern.get("prompt", "memphis phonk")))
            fx = _fx_from_payload(payload.get("fx") or {})
            pcm, meta = render_pattern_stem(
                pattern,
                track_index,
                prompt=prompt,
                engine_id=payload.get("engine"),
                fx=fx,
                variation=int(payload.get("variation", 0)),
            )
            _send_wav(self, pcm, meta)
            return

        if path == "/api/generate_bank":
            from phonk_factory import generate_bank
            from phonk_style_presets import merge_render_payload

            payload = merge_render_payload(payload)
            prompt = str(payload.get("prompt", "juicy j dirty memphis phonk"))
            variation = int(payload.get("variation", 0))
            with tempfile.TemporaryDirectory() as tmp:
                out = Path(tmp) / "bank_a"
                generate_bank(
                    prompt=prompt,
                    out_dir=out,
                    variation=variation,
                    engine_id=payload.get("engine"),
                )
                zbuf = io.BytesIO()
                with zipfile.ZipFile(zbuf, "w", zipfile.ZIP_DEFLATED) as zf:
                    for f in out.rglob("*"):
                        if f.is_file():
                            zf.write(f, f.relative_to(out).as_posix())
                data = zbuf.getvalue()
            self.send_response(200)
            self.send_header("Content-Type", "application/zip")
            self.send_header(
                "Content-Disposition",
                f'attachment; filename="lofi12_phonk_bank_v{variation:02d}.zip"',
            )
            self.send_header("Content-Length", str(len(data)))
            self.end_headers()
            self.wfile.write(data)
            return

        if path == "/api/groove_pattern":
            from pattern_from_groove import groove_to_pattern
            from pattern_schema import Pattern
            from phonk_style_presets import get_preset, merge_render_payload

            payload = merge_render_payload(payload)
            prompt = str(payload.get("prompt", "juicy j dj paul dirty memphis 86"))
            bpm = float(payload.get("bpm", 84))
            pat: Pattern = groove_to_pattern(prompt, bpm, int(payload.get("variation", 0)))
            preset_id = payload.get("preset")
            if preset_id:
                ps = get_preset(str(preset_id))
                if ps:
                    pat.fx = {
                        "filter": ps.fx.filter_cutoff,
                        "reverb": ps.fx.reverb_send,
                        "tape": ps.fx.tape,
                        "drive": ps.fx.drive,
                        "cassette": ps.fx.cassette,
                        "bitcrush": ps.fx.bitcrush,
                    }
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
