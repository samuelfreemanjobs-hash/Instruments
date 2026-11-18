#!/usr/bin/env python3
"""Extract a groove card (JSON) from MIDI or a JSON note list. Stdlib only."""
from __future__ import annotations

import argparse
import json
import struct
import sys
from collections import defaultdict
from pathlib import Path

# GM-ish drum map → kit lane (override via --map-json)
DEFAULT_LANES = {
    35: "KICK", 36: "KICK",
    37: "PERC", 38: "SNARE", 39: "CLAP", 40: "SNARE",
    42: "HAT", 44: "HAT", 46: "HAT",
    49: "FX", 51: "PERC", 54: "PERC", 56: "COWBELL",
}


def _read_vlq(data: bytes, i: int) -> tuple[int, int]:
    value = 0
    while i < len(data):
        b = data[i]
        i += 1
        value = (value << 7) | (b & 0x7F)
        if not (b & 0x80):
            break
    return value, i


def parse_midi(path: Path) -> dict:
    raw = path.read_bytes()
    if raw[:4] != b"MThd":
        raise ValueError(f"not a MIDI file: {path}")
    header_len = struct.unpack(">I", raw[4:8])[0]
    fmt, ntrks, division = struct.unpack(">HHH", raw[8:14])
    if division & 0x8000:
        raise ValueError("SMPTE time division not supported")
    ppq = division
    i = 8 + header_len
    notes = []
    tempo_us = 500000  # 120 BPM default
    for _ in range(ntrks):
        if raw[i : i + 4] != b"MTrk":
            raise ValueError("bad track magic")
        length = struct.unpack(">I", raw[i + 4 : i + 8])[0]
        track = raw[i + 8 : i + 8 + length]
        i += 8 + length
        t = 0
        running = None
        j = 0
        while j < len(track):
            delta, j = _read_vlq(track, j)
            t += delta
            if j >= len(track):
                break
            status = track[j]
            if status & 0x80:
                running = status
                j += 1
            else:
                if running is None:
                    raise ValueError("running status without prior status")
                status = running
            if status == 0xFF:
                meta = track[j]
                j += 1
                mlen, j = _read_vlq(track, j)
                payload = track[j : j + mlen]
                j += mlen
                if meta == 0x51 and mlen == 3:
                    tempo_us = (payload[0] << 16) | (payload[1] << 8) | payload[2]
            elif status in (0xF0, 0xF7):
                slen, j = _read_vlq(track, j)
                j += slen
            else:
                kind = status & 0xF0
                if kind in (0xC0, 0xD0):
                    j += 1
                else:
                    d1 = track[j]
                    d2 = track[j + 1] if j + 1 < len(track) else 0
                    j += 2
                    if kind == 0x90 and d2 > 0:
                        notes.append({"tick": t, "pitch": d1, "velocity": d2, "channel": status & 0x0F})
                    # note-off / note-on vel0 ignored for groove onset map
    bpm = round(60_000_000 / tempo_us, 3)
    return {"ppq": ppq, "format": fmt, "bpm": bpm, "notes": notes, "source": str(path)}


def parse_json_notes(path: Path) -> dict:
    data = json.loads(path.read_text(encoding="utf-8"))
    notes = data.get("notes") or data
    if not isinstance(notes, list):
        raise ValueError("JSON must be a list of notes or {notes:[...]}")
    return {
        "ppq": int(data.get("ppq", 480)),
        "bpm": float(data.get("bpm", 120)),
        "notes": notes,
        "source": str(path),
    }


def analyze(payload: dict, grid: int, lane_map: dict[int, str]) -> dict:
    ppq = int(payload["ppq"])
    bpm = float(payload["bpm"])
    # grid = subdivisions per bar in 4/4 (16 => 16ths)
    grid_ticks = max(1, (ppq * 4) // grid)
    rows = []
    odd_delays = []
    vel_by_lane: dict[str, list[int]] = defaultdict(list)
    for n in payload["notes"]:
        tick = int(n["tick"])
        pitch = int(n["pitch"])
        vel = int(n["velocity"])
        lane = lane_map.get(pitch, f"MIDI{pitch}")
        nearest = round(tick / grid_ticks) * grid_ticks
        delta = tick - nearest
        step = nearest // grid_ticks
        if step % 2 == 1:
            odd_delays.append(delta)
        vel_by_lane[lane].append(vel)
        rows.append({
            "tick": tick,
            "pitch": pitch,
            "lane": lane,
            "velocity": vel,
            "delta_ticks": delta,
            "tag": "C",
        })
    swing_ticks = None
    swing_pct = None
    if odd_delays:
        swing_ticks = round(sum(odd_delays) / len(odd_delays), 2)
        # 0 = straight; ~grid_ticks/3 ≈ classic swing feel approximation
        swing_pct = round(100.0 * swing_ticks / grid_ticks, 2) if grid_ticks else None
    return {
        "schema": "drumsmith-groove-card-v1",
        "source": payload.get("source"),
        "bpm": bpm,
        "ppq": ppq,
        "grid_subdivisions_per_bar": grid,
        "grid_ticks": grid_ticks,
        "swing_mean_odd_delta_ticks": swing_ticks,
        "swing_pct_of_grid": swing_pct,
        "swing_tag": "C",
        "note_count": len(rows),
        "velocity_by_lane": {k: {"min": min(v), "max": max(v), "mean": round(sum(v) / len(v), 2)} for k, v in vel_by_lane.items()},
        "hits": rows,
        "rights": "analysis-only-unless-cleared",
    }


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description="Learn MIDI groove → JSON card")
    ap.add_argument("--midi", type=Path, help="Standard MIDI file")
    ap.add_argument("--json-notes", type=Path, help="JSON note list {ppq,bpm,notes:[{tick,pitch,velocity}]}")
    ap.add_argument("--grid", type=int, default=16, help="Subdivisions per bar (default 16)")
    ap.add_argument("--map-json", type=Path, help="Optional pitch→lane map JSON object")
    ap.add_argument("--out", type=Path, help="Write groove card JSON (default stdout)")
    args = ap.parse_args(argv)
    if bool(args.midi) == bool(args.json_notes):
        ap.error("provide exactly one of --midi or --json-notes")
    lane_map = dict(DEFAULT_LANES)
    if args.map_json:
        lane_map.update({int(k): v for k, v in json.loads(args.map_json.read_text()).items()})
    payload = parse_midi(args.midi) if args.midi else parse_json_notes(args.json_notes)
    card = analyze(payload, args.grid, lane_map)
    text = json.dumps(card, indent=2)
    if args.out:
        args.out.parent.mkdir(parents=True, exist_ok=True)
        args.out.write_text(text + "\n", encoding="utf-8")
    else:
        print(text)
    return 0


if __name__ == "__main__":
    sys.exit(main())
