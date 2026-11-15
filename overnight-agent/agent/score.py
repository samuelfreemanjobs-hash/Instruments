"""Score a generated kit for format validity, energy, and diversity."""
from __future__ import annotations

import json
import wave
from pathlib import Path
from typing import Any, Dict, List


def _read_wav(path: Path) -> Dict[str, Any]:
    problems: List[str] = []
    with wave.open(str(path), "rb") as w:
        ch, width, rate, frames = w.getnchannels(), w.getsampwidth(), w.getframerate(), w.getnframes()
        raw = w.readframes(frames)
    if ch != 1:
        problems.append(f"channels={ch}")
    if width != 2:
        problems.append(f"width={width}")
    if rate != 44100:
        problems.append(f"rate={rate}")
    if frames < 64:
        problems.append("too_short")
    import array

    samples = array.array("h")
    samples.frombytes(raw)
    if not samples:
        return {"file": path.name, "problems": problems or ["empty"], "peak": 0, "rms": 0.0}
    peak = max(abs(s) for s in samples)
    rms = (sum(s * s for s in samples) / len(samples)) ** 0.5
    if peak >= 32767:
        problems.append("clipped")
    if rms < 200:
        problems.append("silent")
    return {
        "file": path.name,
        "problems": problems,
        "peak": peak,
        "rms": round(rms, 1),
        "frames": frames,
    }


def score_kit(kit_dir: Path, build_ok: bool | None = None) -> Dict[str, Any]:
    manifest_path = kit_dir / "manifest.json"
    report: Dict[str, Any] = {
        "kit_dir": str(kit_dir),
        "ok": True,
        "score": 0.0,
        "files": [],
        "build_ok": build_ok,
    }
    if not manifest_path.is_file():
        report["ok"] = False
        report["error"] = "missing manifest"
        return report

    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    peaks: List[int] = []
    problem_count = 0
    for voice in manifest.get("voices", []):
        path = kit_dir / voice["file"]
        if not path.is_file():
            report["files"].append({"file": voice["file"], "problems": ["missing"]})
            problem_count += 1
            continue
        info = _read_wav(path)
        report["files"].append(info)
        peaks.append(int(info["peak"]))
        problem_count += len(info["problems"])

    n = max(1, len(report["files"]))
    avg_peak = sum(peaks) / max(1, len(peaks)) if peaks else 0.0
    diversity = 0.0
    if len(peaks) >= 2:
        diversity = (max(peaks) - min(peaks)) / 32767.0

    score = 40.0
    score += min(35.0, avg_peak / 32767.0 * 35.0)
    score += min(15.0, diversity * 40.0)
    score -= problem_count * 6.0
    if build_ok is True:
        score += 10.0
    elif build_ok is False:
        score -= 20.0

    report["score"] = round(max(0.0, score), 2)
    report["ok"] = problem_count == 0 and len(peaks) >= 6
    report["avg_peak"] = round(avg_peak, 1)
    report["diversity"] = round(diversity, 3)
    report["problem_count"] = problem_count
    return report
