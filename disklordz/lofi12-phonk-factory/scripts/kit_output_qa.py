"""Walk rendered kit folders: non-zero WAVs, peak ceiling, basic naming."""

from __future__ import annotations

import argparse
import math
import re
import struct
import sys
import wave
from dataclasses import dataclass
from pathlib import Path

# PR gate: no clipping past -0.3 dBFS (stricter than marketing -1.0 dBFS one-shot target).
MAX_PEAK_DBFS = -0.3

_WAV_NAME = re.compile(r"^[a-z0-9][a-z0-9_\-\.#]*\.wav$", re.IGNORECASE)
_FORBIDDEN_SOURCE_MARKERS = (
    "splice",
    "cymatics",
    "samplefocus",
    "youtube",
    "yt_rip",
)


@dataclass
class WavIssue:
    path: Path
    message: str


def _peak_dbfs_from_wav(path: Path) -> tuple[float, int]:
    with wave.open(str(path), "rb") as wf:
        nch = wf.getnchannels()
        width = wf.getsampwidth()
        nframes = wf.getnframes()
        if nframes == 0:
            return -math.inf, 0
        raw = wf.readframes(nframes)
    if width != 2:
        raise ValueError(f"unsupported sample width {width} in {path}")
    peak = 0
    step = width * nch
    for i in range(0, len(raw), step):
        sample = struct.unpack_from("<h", raw, i)[0]
        a = abs(sample)
        if a > peak:
            peak = a
    if peak == 0:
        return -math.inf, nframes
    linear = peak / 32768.0
    return 20.0 * math.log10(linear), nframes


def _name_ok(path: Path) -> bool:
    return bool(_WAV_NAME.match(path.name))


def scan_kit(kit_dir: Path) -> list[WavIssue]:
    issues: list[WavIssue] = []
    if not kit_dir.is_dir():
        return [WavIssue(kit_dir, "not a directory")]

    manifest = kit_dir / "manifest.json"
    if manifest.is_file():
        text = manifest.read_text(encoding="utf-8").lower()
        for marker in _FORBIDDEN_SOURCE_MARKERS:
            if marker in text:
                issues.append(WavIssue(manifest, f"forbidden third-party source marker: {marker}"))

    wavs = sorted(kit_dir.rglob("*.wav"))
    if not wavs:
        issues.append(WavIssue(kit_dir, "no WAV files under kit"))
        return issues

    for wav in wavs:
        if wav.stat().st_size == 0:
            issues.append(WavIssue(wav, "zero-byte file"))
            continue
        if not _name_ok(wav):
            issues.append(WavIssue(wav, f"unexpected filename (use lowercase slug): {wav.name}"))
        try:
            pk_db, frames = _peak_dbfs_from_wav(wav)
        except (wave.Error, ValueError) as exc:
            issues.append(WavIssue(wav, f"unreadable WAV: {exc}"))
            continue
        if frames == 0:
            issues.append(WavIssue(wav, "empty audio frames"))
        elif pk_db > MAX_PEAK_DBFS + 1e-6:
            issues.append(WavIssue(wav, f"peak {pk_db:.2f} dBFS exceeds {MAX_PEAK_DBFS} dBFS"))
        elif pk_db == -math.inf:
            issues.append(WavIssue(wav, "silent (all zeros)"))

    return issues


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="QA scan for DiskLordz kit output folders")
    parser.add_argument("--kit-dir", type=Path, help="Existing kit directory")
    parser.add_argument(
        "--generate-demo",
        action="store_true",
        help="Build occult_tape demo kit into a temp dir, then scan",
    )
    parser.add_argument("--mode", default="occult_tape", help="Factory mode when --generate-demo")
    args = parser.parse_args(argv)

    if args.generate_demo:
        import tempfile

        from drum_kit_builder import build_kit

        with tempfile.TemporaryDirectory(prefix="phonk-qa-") as tmp:
            out = Path(tmp) / "kit"
            build_kit(mode_id=args.mode, out_dir=out, kit_size="demo", seed=11)
            issues = scan_kit(out)
    elif args.kit_dir:
        issues = scan_kit(args.kit_dir.expanduser().resolve())
    else:
        parser.error("pass --kit-dir or --generate-demo")

    if issues:
        for item in issues:
            print(f"FAIL {item.path}: {item.message}", file=sys.stderr)
        return 1
    print("kit_output_qa: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())