#!/usr/bin/env python3
"""verify_kit.py — delivery checks for rendered drum samples.

Usage:  python3 scripts/verify_kit.py <dir_or_wav> [--profile splice] [--json]
Exit:   0 = no FAIL   1 = at least one FAIL   2 = no WAV files found

stdlib only. Statuses: PASS / FAIL / WARN. Only FAIL changes the exit code.
Profile "splice" encodes: 24-bit WAV, >= 44.1 kHz and uniform across the pack,
sample peak at -1.0 dBFS or below, no clipping, micro-fades, no DC, loops on bar lines.
Pitch accuracy and by-ear judgement are NOT checked here (see CALIPER).
"""
import json
import math
import os
import re
import struct
import sys
import wave

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import dsp_math as dm  # noqa: E402

PROFILES = {
    "splice": {
        "bits": 24,
        "min_sr": 44100,
        "peak_ceiling_dbfs": -1.0,
        "peak_tolerance_db": 0.01,
        "normalized_floor_dbfs": -1.5,   # quieter than this -> WARN (not normalized)
        "true_peak_warn_dbtp": -0.5,     # above this -> WARN (approximate meter)
        "edge_max_linear": 0.01,         # first/last sample, about -40 dBFS
        "dc_fail": 0.005,
        "dc_warn": 0.001,
        "clip_level": 0.999,
        "clip_run": 3,
        "silence_floor": 1e-4,
    }
}

LOOP_RE = re.compile(r"(\d+(?:\.\d+)?)\s*bpm.*?(\d+)\s*bars?", re.IGNORECASE)


def read_wav(path):
    """Return (channels_of_floats, sr, bits). Supports 16- and 24-bit PCM."""
    with wave.open(path, "rb") as w:
        ch, sw, sr, n = w.getnchannels(), w.getsampwidth(), w.getframerate(), w.getnframes()
        raw = w.readframes(n)
    if sw == 2:
        vals = [v / 32768.0 for v in struct.unpack("<%dh" % (n * ch), raw)]
    elif sw == 3:
        vals = []
        for i in range(0, len(raw), 3):
            v = raw[i] | (raw[i + 1] << 8) | (raw[i + 2] << 16)
            if v & 0x800000:
                v -= 0x1000000
            vals.append(v / 8388608.0)
    else:
        raise ValueError("unsupported sample width: %d bytes" % sw)
    chans = [vals[c::ch] for c in range(ch)]
    return chans, sr, sw * 8


def _max_run(chan, level):
    run = best = 0
    for s in chan:
        if abs(s) >= level:
            run += 1
            best = max(best, run)
        else:
            run = 0
    return best


def check_file(path, profile):
    p = PROFILES[profile]
    out = {}
    chans, sr, bits = read_wav(path)
    frames = len(chans[0])
    flat_peak = max((max(abs(s) for s in c) if c else 0.0) for c in chans)

    out["bit_depth"] = ("PASS" if bits == p["bits"] else "FAIL", "%d-bit (need %d)" % (bits, p["bits"]))
    out["sample_rate"] = ("PASS" if sr >= p["min_sr"] else "FAIL", "%d Hz (min %d)" % (sr, p["min_sr"]))

    if flat_peak < p["silence_floor"] or frames == 0:
        out["not_silent"] = ("FAIL", "file is silent or empty")
        return out, sr, bits
    out["not_silent"] = ("PASS", "peak %.1f dBFS" % dm.dbfs(flat_peak))

    pk = dm.dbfs(flat_peak)
    ceil = p["peak_ceiling_dbfs"] + p["peak_tolerance_db"]
    out["peak_ceiling"] = ("PASS" if pk <= ceil else "FAIL", "sample peak %.3f dBFS (ceiling %.1f)" % (pk, p["peak_ceiling_dbfs"]))
    out["normalized"] = ("PASS" if pk >= p["normalized_floor_dbfs"] else "WARN", "peak %.3f dBFS (target %.1f)" % (pk, p["peak_ceiling_dbfs"]))

    tp = max(dm.true_peak(c) for c in chans)
    tpd = dm.dbfs(tp)
    out["true_peak"] = ("PASS" if tpd <= p["true_peak_warn_dbtp"] else "WARN", "approx true peak %.2f dBTP (approximate meter)" % tpd)

    run = max(_max_run(c, p["clip_level"]) for c in chans)
    out["no_clipping"] = ("PASS" if run < p["clip_run"] else "FAIL", "longest run at full scale: %d samples" % run)

    edge = max(max(abs(c[0]), abs(c[-1])) for c in chans)
    out["edges_faded"] = ("PASS" if edge <= p["edge_max_linear"] else "FAIL", "max edge sample %.4f (limit %.2f)" % (edge, p["edge_max_linear"]))

    dc = max(abs(sum(c) / len(c)) for c in chans)
    st = "PASS" if dc <= p["dc_warn"] else ("WARN" if dc <= p["dc_fail"] else "FAIL")
    out["dc_offset"] = (st, "mean %.5f" % dc)

    m = LOOP_RE.search(os.path.basename(path))
    if m:
        bpm, bars = float(m.group(1)), int(m.group(2))
        want = dm.loop_samples(bars, bpm, sr)
        out["loop_length"] = ("PASS" if abs(frames - want) <= 1 else "FAIL", "%d frames (want %d for %d bar @ %s bpm)" % (frames, want, bars, m.group(1)))
    return out, sr, bits


def verify(target, profile="splice"):
    files = []
    if os.path.isdir(target):
        for root, _, names in os.walk(target):
            files += [os.path.join(root, n) for n in sorted(names) if n.lower().endswith(".wav")]
    elif target.lower().endswith(".wav"):
        files = [target]
    report = {"profile": profile, "files": {}, "pack": {}, "summary": {"files": len(files), "fail": 0, "warn": 0}}
    rates, depths = set(), set()
    for f in sorted(files):
        try:
            checks, sr, bits = check_file(f, profile)
            rates.add(sr)
            depths.add(bits)
        except Exception as e:  # unreadable file is a FAIL, never a skip
            checks = {"readable": ("FAIL", str(e))}
        report["files"][f] = checks
        for status, _ in checks.values():
            if status == "FAIL":
                report["summary"]["fail"] += 1
            elif status == "WARN":
                report["summary"]["warn"] += 1
    if len(files) > 1:
        ok = len(rates) <= 1 and len(depths) <= 1
        report["pack"]["uniform_format"] = ("PASS" if ok else "FAIL", "rates=%s bits=%s" % (sorted(rates), sorted(depths)))
        if not ok:
            report["summary"]["fail"] += 1
    return report


def main(argv):
    if not argv:
        print(__doc__)
        return 2
    target = argv[0]
    profile = argv[argv.index("--profile") + 1] if "--profile" in argv else "splice"
    rep = verify(target, profile)
    if rep["summary"]["files"] == 0:
        print("no WAV files found at %s" % target)
        return 2
    if "--json" in argv:
        print(json.dumps(rep, indent=2))
    else:
        for f, checks in rep["files"].items():
            print(f)
            for k, (s, d) in checks.items():
                print("  %-14s %-4s %s" % (k, s, d))
        for k, (s, d) in rep["pack"].items():
            print("PACK %-14s %-4s %s" % (k, s, d))
        print("SUMMARY files=%(files)d fail=%(fail)d warn=%(warn)d" % rep["summary"])
    return 1 if rep["summary"]["fail"] else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
