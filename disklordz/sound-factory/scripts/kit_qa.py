#!/usr/bin/env python3
"""Technical QA for a DiskLordz catalog kit. Human audition stays required.

    python disklordz/sound-factory/scripts/kit_qa.py disklordz/sound-factory/kits/memphis-vault-vol-1
"""

from __future__ import annotations

import hashlib
import json
import sys
import wave
from pathlib import Path


def read_pcm16(path: Path) -> tuple[list[int], int]:
    with wave.open(str(path), "rb") as handle:
        channels = handle.getnchannels()
        width = handle.getsampwidth()
        rate = handle.getframerate()
        frames = handle.getnframes()
        raw = handle.readframes(frames)
    problems: list[str] = []
    if channels != 1:
        problems.append(f"channels {channels}")
    if width != 2:
        problems.append(f"bit depth {width * 8}")
    if rate != 44100:
        problems.append(f"sample rate {rate}")
    if frames < 64:
        problems.append("too short")
    samples = [
        int.from_bytes(raw[i : i + 2], "little", signed=True) for i in range(0, len(raw), 2)
    ]
    return samples, problems


def fingerprint(samples: list[int]) -> tuple[int, ...]:
    """Envelope shape plus a pitch cue. Tuned 808s share an envelope and are not duplicates."""
    if not samples:
        return ()
    bins = 32
    size = max(1, len(samples) // bins)
    peaks: list[int] = []
    for b in range(bins):
        chunk = samples[b * size : (b + 1) * size] or samples[-1:]
        peaks.append(max(abs(s) for s in chunk))
    top = max(peaks) or 1
    crossings = 0
    for left, right in zip(samples, samples[1:]):
        if left < 0 <= right or right < 0 <= left:
            crossings += 1
    zcr_per_sec = int(crossings * 44100 / len(samples))
    return (zcr_per_sec // 15, *(min(15, (p * 16) // top) for p in peaks))


def check_file(path: Path) -> dict:
    samples, problems = read_pcm16(path)
    if not samples:
        return {"file": path.name, "problems": problems or ["empty"]}
    peak = max(abs(s) for s in samples)
    mean = sum(samples) / len(samples)
    rms = (sum(s * s for s in samples) / len(samples)) ** 0.5
    if peak >= 32767:
        problems.append("clipped")
    if rms < 400:
        problems.append("silent")
    if abs(mean) > 250:
        problems.append(f"dc offset {mean:.1f}")
    if abs(samples[0]) > 1200 or abs(samples[-1]) > 1200:
        problems.append("edge click")
    digest = hashlib.sha256(path.read_bytes()).hexdigest()
    return {
        "file": path.name,
        "sha256": digest,
        "peak": peak,
        "rms": round(rms, 1),
        "dc": round(mean, 2),
        "fingerprint": fingerprint(samples),
        "problems": problems,
    }


def audit(folder: Path) -> dict:
    wavs = sorted(folder.rglob("*.wav"))
    reports = [check_file(path) for path in wavs]
    by_hash: dict[str, list[str]] = {}
    by_print: dict[tuple[int, ...], list[str]] = {}
    for report in reports:
        if "sha256" not in report:
            continue
        by_hash.setdefault(report["sha256"], []).append(report["file"])
        by_print.setdefault(report["fingerprint"], []).append(report["file"])
    duplicates = [names for names in by_hash.values() if len(names) > 1]
    near = [names for names in by_print.values() if len(names) > 1]
    failures = [report for report in reports if report["problems"]]
    return {
        "format": "DISKLORDZ_KIT_QA",
        "folder": folder.name,
        "files": len(wavs),
        "failures": [{"file": item["file"], "problems": item["problems"]} for item in failures],
        "exactDuplicates": duplicates,
        "nearDuplicates": near,
        "humanAuditionRequired": True,
        "passed": not failures and not duplicates and not near,
    }


def main() -> None:
    if len(sys.argv) != 2:
        raise SystemExit("usage: kit_qa.py <kit-folder>")
    folder = Path(sys.argv[1])
    if not folder.is_dir():
        raise SystemExit(f"not a folder: {folder}")
    result = audit(folder)
    (folder / "qa.json").write_text(json.dumps(result, indent=2), encoding="utf-8")
    print(json.dumps({k: result[k] for k in ("files", "passed", "failures", "exactDuplicates", "nearDuplicates")}, indent=2))
    if not result["passed"]:
        raise SystemExit(1)


if __name__ == "__main__":
    main()
