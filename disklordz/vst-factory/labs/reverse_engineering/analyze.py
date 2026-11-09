"""WAV character analysis — sonic guidance, not circuit clone claims."""

from __future__ import annotations

import struct
import wave
from pathlib import Path
from typing import Any


def analyze_character(path: Path) -> dict[str, Any]:
    with wave.open(str(path), "rb") as w:
        rate = w.getframerate()
        sampwidth = w.getsampwidth()
        raw = w.readframes(min(w.getnframes(), rate * 5))
    if sampwidth != 2:
        return {"ok": False, "error": "need 16-bit PCM"}
    vals = [struct.unpack("<h", raw[i : i + 2])[0] / 32768.0 for i in range(0, len(raw) - 1, 2)]
    if not vals:
        return {"ok": False, "error": "empty"}
    peak = max(abs(v) for v in vals)
    rms = (sum(v * v for v in vals) / len(vals)) ** 0.5
    zc = sum(1 for a, b in zip(vals, vals[1:]) if a * b < 0)
    brightness = "bright" if zc > len(vals) * 0.3 else "dark"
    return {
        "ok": True,
        "peak": peak,
        "rms": rms,
        "transient_density": zc / len(vals),
        "brightness_hint": brightness,
        "recommendations": [
            "Consider saturation module if high peak/low rms",
            "Consider filter.cutoff automation if brightness extreme",
        ],
        "legal": "Character inspiration only — do not claim proprietary circuit recreation",
    }
