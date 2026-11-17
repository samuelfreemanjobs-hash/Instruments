#!/usr/bin/env python3
"""Splice.com-oriented sound quality audit for Black Box kits.

Rubric sourced from Splice's public pack tips
(https://splice.com/blog/tips-for-creating-your-own-sample-pack/):
  - Release at 44.1 or 48 kHz; prefer 24-bit (we ship intentional 16-bit machine packs)
  - No MP3 / compressed sources
  - Clear one-shot vs loop folders + labeled names
  - Loops must loop cleanly (no edge clicks)
  - Pack scale: small 50–150, large 300–1000
  - Originality / variety (not 100 near-duplicates)

Plus commercial drum-one-shot hygiene: peak headroom, silence trim, DC, clip.
This is a technical screen — HITL listening still required (RELEASE_GATE.md).
"""

from __future__ import annotations

import argparse
import json
import math
import struct
import wave
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


def _measure(path: Path) -> dict:
    with wave.open(str(path), "rb") as w:
        ch, width, rate, frames, comptype, _ = (
            w.getnchannels(),
            w.getsampwidth(),
            w.getframerate(),
            w.getnframes(),
            w.getcomptype(),
            w.getcompname(),
        )
        raw = w.readframes(frames)
    if comptype != "NONE" or width != 2:
        return {"path": str(path), "error": "not_pcm16"}
    samples = [struct.unpack_from("<h", raw, i)[0] for i in range(0, len(raw), 2)]
    if not samples:
        return {"path": str(path), "error": "empty"}
    peak = max(abs(s) for s in samples) or 1
    peak_db = 20.0 * math.log10(peak / 32768.0)
    energy = sum((s / 32768.0) ** 2 for s in samples) / len(samples)
    rms_db = 20.0 * math.log10(math.sqrt(energy) + 1e-12)
    clipped = sum(1 for s in samples if s in (-32768, 32767)) / len(samples)
    # leading / trailing silence (threshold ~ -60 dBFS)
    thr = int(32768 * 10 ** (-60 / 20))
    lead = 0
    for s in samples:
        if abs(s) > thr:
            break
        lead += 1
    trail = 0
    for s in reversed(samples):
        if abs(s) > thr:
            break
        trail += 1
    # DC offset
    mean = sum(samples) / len(samples)
    dc = abs(mean) / 32768.0
    # edge click proxy: |first|/|last| vs peak
    edge = max(abs(samples[0]), abs(samples[-1])) / peak
    return {
        "path": str(path.relative_to(path.parents[3]) if len(path.parts) > 4 else path.name),
        "sample_rate": rate,
        "channels": ch,
        "bit_depth": width * 8,
        "duration_sec": round(frames / rate, 4),
        "peak_dbfs": round(peak_db, 3),
        "rms_dbfs": round(rms_db, 3),
        "clipped_fraction": round(clipped, 6),
        "lead_silence_ms": round(1000.0 * lead / rate, 2),
        "trail_silence_ms": round(1000.0 * trail / rate, 2),
        "dc_abs": round(dc, 6),
        "edge_vs_peak": round(edge, 4),
        "is_loop": "loop" in path.parent.name.lower() or "_LOOP_" in path.name.upper(),
    }


def _corr(a: list[int], b: list[int]) -> float:
    n = min(len(a), len(b), 2048)
    if n < 64:
        return 0.0
    aa = a[:n]
    bb = b[:n]
    ma = sum(aa) / n
    mb = sum(bb) / n
    num = sum((x - ma) * (y - mb) for x, y in zip(aa, bb))
    da = math.sqrt(sum((x - ma) ** 2 for x in aa)) or 1.0
    db = math.sqrt(sum((y - mb) ** 2 for y in bb)) or 1.0
    return num / (da * db)


def _read_pcm(path: Path) -> list[int]:
    with wave.open(str(path), "rb") as w:
        raw = w.readframes(w.getnframes())
    return [struct.unpack_from("<h", raw, i)[0] for i in range(0, len(raw), 2)]


def audit_kit(masters: Path, sku: str) -> dict:
    wavs = sorted(masters.rglob("*.wav"))
    findings: list[dict] = []
    warns: list[str] = []
    fails: list[str] = []
    metrics = []

    if not wavs:
        return {"sku": sku, "status": "FAIL", "errors": ["no wavs"], "splice_grade": "F"}

    # Pack scale (Splice: small 50–150, large 300–1000)
    n = len(wavs)
    if n < 50:
        warns.append(f"pack_size {n} below Splice small-pack band (50–150)")
    elif 150 < n < 300:
        warns.append(f"pack_size {n} between Splice small and large bands")
    elif n > 1000:
        warns.append(f"pack_size {n} above typical Splice large-pack ceiling (~1000)")

    by_role: dict[str, list[Path]] = defaultdict(list)
    for path in wavs:
        by_role[path.parent.name].append(path)
        m = _measure(path)
        metrics.append(m)
        if m.get("error"):
            fails.append(f"{path.name}: {m['error']}")
            continue
        if m["sample_rate"] not in (44100, 48000):
            fails.append(f"{path.name}: sample_rate {m['sample_rate']} (Splice wants 44.1/48k)")
        if m["channels"] != 1:
            warns.append(f"{path.name}: stereo — drum oneshots usually mono on Splice")
        if m["bit_depth"] < 16:
            fails.append(f"{path.name}: bit_depth {m['bit_depth']}")
        if m["bit_depth"] == 16 and path == wavs[0]:
            # Once per kit: Splice prefers 24-bit; we ship intentional 16-bit machine packs
            warns.append(
                "delivery is PCM16 (Splice blog prefers 24-bit @ 44.1/48k — intentional machine aesthetic)"
            )
        if m["peak_dbfs"] > -0.05:
            warns.append(f"{path.name}: peak {m['peak_dbfs']} dBFS near/at clip")
        if m["peak_dbfs"] < -12.0 and not m["is_loop"]:
            warns.append(f"{path.name}: quiet peak {m['peak_dbfs']} dBFS (Splice hits usually hotter)")
        if m["clipped_fraction"] > 0.002:
            fails.append(f"{path.name}: clipped_fraction {m['clipped_fraction']}")
        if m["lead_silence_ms"] > 25.0 and not m["is_loop"]:
            warns.append(f"{path.name}: lead silence {m['lead_silence_ms']} ms")
        if m["trail_silence_ms"] > 80.0 and not m["is_loop"]:
            warns.append(f"{path.name}: long trail {m['trail_silence_ms']} ms")
        if m["dc_abs"] > 0.02:
            warns.append(f"{path.name}: DC offset {m['dc_abs']}")
        if m["is_loop"] and m["edge_vs_peak"] > 0.35:
            warns.append(f"{path.name}: loop edge energy high ({m['edge_vs_peak']}) — check seamless loop")

    # Distinctiveness: consecutive pairs in each role
    near_dupes = 0
    checked = 0
    for role, paths in by_role.items():
        for a, b in zip(paths, paths[1:]):
            if checked >= 40:
                break
            try:
                c = _corr(_read_pcm(a), _read_pcm(b))
            except Exception:
                continue
            checked += 1
            if c > 0.985:
                near_dupes += 1
                findings.append({"type": "near_duplicate", "a": a.name, "b": b.name, "corr": round(c, 4)})
        if checked >= 40:
            break
    if near_dupes > 5:
        warns.append(f"{near_dupes} near-duplicate consecutive pairs (Splice originality bar)")

    # Naming / structure
    has_loops = any(m.get("is_loop") for m in metrics if not m.get("error"))
    has_oneshots = any(not m.get("is_loop") for m in metrics if not m.get("error"))
    if not has_oneshots:
        fails.append("no oneshots")
    if not has_loops:
        warns.append("no loops folder detected")

    peaks = [m["peak_dbfs"] for m in metrics if not m.get("error") and not m.get("is_loop")]
    durs = [m["duration_sec"] for m in metrics if not m.get("error") and not m.get("is_loop")]
    summary = {
        "wav_count": n,
        "roles": {k: len(v) for k, v in sorted(by_role.items())},
        "peak_dbfs_median": round(sorted(peaks)[len(peaks) // 2], 3) if peaks else None,
        "peak_dbfs_min": round(min(peaks), 3) if peaks else None,
        "peak_dbfs_max": round(max(peaks), 3) if peaks else None,
        "oneshot_duration_median_sec": round(sorted(durs)[len(durs) // 2], 4) if durs else None,
        "bit_depth_delivery": 16,
        "splice_prefers_bit_depth": 24,
        "sample_rate": 44100,
    }

    # Grade
    if fails:
        grade, status = "F", "FAIL"
    elif len(warns) > 25:
        grade, status = "C", "WARN"
    elif len(warns) > 8 or near_dupes > 3:
        grade, status = "B", "WARN"
    else:
        grade, status = "A-", "PASS_TECHNICAL"

    # Hard note: 16-bit intentional machine aesthetic vs Splice 24-bit preference
    splice_gap = [
        "Delivery is PCM16 mono @ 44.1k (machine / DR-era aesthetic).",
        "Splice blog guidance prefers 24-bit @ 44.1/48k for max quality.",
        "release_ready remains false until human listening (RELEASE_GATE.md).",
    ]

    return {
        "sku": sku,
        "status": status,
        "splice_grade": grade,
        "summary": summary,
        "errors": fails,
        "warnings": warns[:40],
        "warning_count": len(warns),
        "near_duplicate_findings": findings[:12],
        "splice_rubric": {
            "source": "https://splice.com/blog/tips-for-creating-your-own-sample-pack/",
            "checks": [
                "44.1/48 kHz",
                "prefer 24-bit (flag if 16)",
                "one-shots + loops labeled",
                "seamless loops",
                "pack size bands",
                "originality / low near-dupes",
                "peak/silence/DC hygiene",
            ],
            "gaps_vs_splice": splice_gap,
        },
        "release_ready": False,
    }


def main() -> None:
    import sys

    root = Path(__file__).resolve().parents[2]
    for p in (str(root), str(root / "scripts")):
        if p not in sys.path:
            sys.path.insert(0, p)

    parser = argparse.ArgumentParser(description="Splice-grade audit for Black Box masters")
    parser.add_argument(
        "--kits",
        nargs="*",
        default=["bb93", "bb96", "bb98", "bb2000"],
        help="era keys",
    )
    parser.add_argument(
        "--out",
        type=Path,
        default=Path("/opt/cursor/artifacts/Black_Box_Splice_Grade_Audit.json"),
    )
    args = parser.parse_args()
    from scripts.black_box.generate_era import kit_masters_dir, load_profile

    reports = []
    for key in args.kits:
        profile = load_profile(key)
        masters = kit_masters_dir(profile)
        if not masters.is_dir():
            reports.append({"sku": profile["sku"], "status": "FAIL", "errors": [f"missing {masters}"]})
            continue
        reports.append(audit_kit(masters, profile["sku"]))

    payload = {"kits": reports, "overall": "PASS_TECHNICAL" if all(r["status"] != "FAIL" for r in reports) else "FAIL"}
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    md = Path("/opt/cursor/artifacts/Black_Box_Splice_Grade_Audit.md")
    lines = ["# Black Box — Splice-grade sound quality audit", ""]
    lines.append("Rubric: [Splice pack tips](https://splice.com/blog/tips-for-creating-your-own-sample-pack/) + drum hygiene.")
    lines.append("")
    for r in reports:
        lines.append(f"## {r.get('sku')} — **{r.get('splice_grade')}** ({r.get('status')})")
        s = r.get("summary") or {}
        lines.append(f"- WAVs: {s.get('wav_count')} · peak median {s.get('peak_dbfs_median')} dBFS")
        lines.append(f"- Errors: {len(r.get('errors') or [])} · Warnings: {r.get('warning_count', 0)}")
        for g in (r.get("splice_rubric") or {}).get("gaps_vs_splice", []):
            lines.append(f"- Gap: {g}")
        for e in (r.get("errors") or [])[:5]:
            lines.append(f"- FAIL: {e}")
        for w in (r.get("warnings") or [])[:8]:
            lines.append(f"- WARN: {w}")
        lines.append("")
    lines.append("`release_ready: false` until HITL.")
    md.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(json.dumps(payload, indent=2))
    print(f"wrote {args.out}")
    print(f"wrote {md}")


if __name__ == "__main__":
    main()
