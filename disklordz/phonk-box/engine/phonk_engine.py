#!/usr/bin/env python3
"""
DiskLordz Phonk Box — Phase 0 PC engine.
USB MIDI in → 8-pad sample playback + 16-step sequencer (mono mix).
SparkLE pad law file names: kick, knock optional merged in kick, snare, clap, hat_closed, hat_open, rim, cowbell_fx
"""
from __future__ import annotations

import argparse
import json
import sys
import threading
import time
from dataclasses import dataclass, field
from pathlib import Path

import numpy as np
import sounddevice as sd
import soundfile as sf

try:
    import rtmidi
except ImportError:
    rtmidi = None  # type: ignore

SAMPLE_RATE = 44100
PAD_NOTES = [36, 37, 38, 39, 42, 46, 49, 51]
STEP_NOTES = list(range(48, 64))
PLAY_NOTE = 120
STOP_NOTE = 121

PAD_FILES = [
    "kick.wav",
    "knock.wav",
    "snare.wav",
    "clap.wav",
    "hat_closed.wav",
    "hat_open.wav",
    "rim.wav",
    "cowbell_fx.wav",
]


@dataclass
class EngineState:
    bpm: float = 140.0
    playing: bool = False
    selected_pad: int = 0
    steps: list[list[bool]] = field(default_factory=lambda: [[False] * 16 for _ in range(8)])
    pads: list[np.ndarray] = field(default_factory=list)
    pending_hits: list[tuple[int, float]] = field(default_factory=list)
    lock: threading.Lock = field(default_factory=threading.Lock)
    step_index: int = 0
    last_step_time: float = 0.0


def load_wav_normalized(path: Path) -> np.ndarray:
    data, sr = sf.read(path, dtype="float32", always_2d=False)
    if data.ndim > 1:
        data = data.mean(axis=1)
    if sr != SAMPLE_RATE:
        ratio = SAMPLE_RATE / sr
        x = np.arange(int(len(data) * ratio))
        xp = np.linspace(0, len(data) - 1, len(data))
        data = np.interp(x, xp, data).astype(np.float32)
    peak = np.max(np.abs(data)) or 1.0
    return (data / peak * 0.85).astype(np.float32)


def silent_pad_placeholder() -> np.ndarray:
    return np.zeros(int(SAMPLE_RATE * 0.05), dtype=np.float32)


def load_kit(kit_dir: Path) -> list[np.ndarray]:
    pads: list[np.ndarray] = []
    for name in PAD_FILES:
        path = kit_dir / name
        if not path.is_file():
            alt = kit_dir / name.replace("_", "")
            path = alt if alt.is_file() else path
        if not path.is_file():
            pads.append(silent_pad_placeholder())
            continue
        pads.append(load_wav_normalized(path))
    return pads


def load_kit_from_manifest(manifest_path: Path) -> tuple[list[np.ndarray], float, str]:
    """Load pads 1–8 from an 808 Lab spark-le-manifest.json (paths relative to manifest dir)."""
    manifest_path = manifest_path.resolve()
    base = manifest_path.parent
    with manifest_path.open(encoding="utf-8") as f:
        doc = json.load(f)
    bpm = float(doc.get("bpm") or 140.0)
    preset_id = str(doc.get("preset_id") or "")
    pad_map = (doc.get("spark_le") or {}).get("pads") or {}
    pads: list[np.ndarray] = []
    for i in range(1, 9):
        entry = pad_map.get(str(i))
        if not entry or not entry.get("file"):
            pads.append(silent_pad_placeholder())
            continue
        wav_path = (base / str(entry["file"])).resolve()
        if not wav_path.is_file():
            pads.append(silent_pad_placeholder())
            continue
        pads.append(load_wav_normalized(wav_path))
    return pads, bpm, preset_id


def resolve_kit_load(
    kit_dir: Path | None, manifest_path: Path | None, default_bpm: float
) -> tuple[list[np.ndarray], float, str | None]:
    if manifest_path is not None:
        pads, bpm, preset = load_kit_from_manifest(manifest_path)
        return pads, bpm, preset
    if kit_dir is not None:
        auto = kit_dir / "spark-le-manifest.json"
        if auto.is_file():
            pads, bpm, preset = load_kit_from_manifest(auto)
            return pads, bpm, preset
        return load_kit(kit_dir), default_bpm, None
    return synthesize_beep(), default_bpm, None


def synthesize_beep() -> list[np.ndarray]:
    t = np.linspace(0, 0.08, int(SAMPLE_RATE * 0.08), False)
    pads = []
    freqs = [55, 80, 120, 200, 400, 600, 900, 1200]
    for f in freqs:
        w = (np.sin(2 * np.pi * f * t) * np.exp(-t * 20)).astype(np.float32)
        pads.append(w)
    return pads


def mix_hit(out: np.ndarray, offset: int, sample: np.ndarray, gain: float) -> None:
    end = min(len(out), offset + len(sample))
    n = end - offset
    if n <= 0:
        return
    out[offset:end] += sample[:n] * gain


class Sequencer:
    def __init__(self, state: EngineState) -> None:
        self.state = state

    def advance(self) -> None:
        if not self.state.playing:
            return
        now = time.perf_counter()
        step_sec = (60.0 / self.state.bpm) / 4.0  # 16th notes
        if now - self.state.last_step_time < step_sec:
            return
        self.state.last_step_time = now
        with self.state.lock:
            si = self.state.step_index
            for pad_i in range(8):
                if self.state.steps[pad_i][si] and len(self.state.pads) > pad_i:
                    self.state.pending_hits.append((pad_i, 0.9))
            self.state.step_index = (si + 1) % 16


def audio_callback(state: EngineState, outdata, frames, _time, _status) -> None:
    buf = np.zeros(frames, dtype=np.float32)
    with state.lock:
        hits = state.pending_hits[:]
        state.pending_hits.clear()
    for pad_i, gain in hits:
        if pad_i < len(state.pads):
            mix_hit(buf, 0, state.pads[pad_i], gain)
    peak = np.max(np.abs(buf)) or 1.0
    if peak > 0.99:
        buf *= 0.99 / peak
    outdata[:] = buf.reshape(-1, 1)


def handle_midi(msg, state: EngineState) -> None:
    if len(msg) < 1:
        return
    status = msg[0]
    if status & 0xF0 == 0x90 and len(msg) >= 3 and msg[2] > 0:
        note = msg[1]
        vel = msg[2] / 127.0
        if note in PAD_NOTES:
            pad_i = PAD_NOTES.index(note)
            with state.lock:
                if pad_i < len(state.pads):
                    state.pending_hits.append((pad_i, vel))
        elif note in STEP_NOTES:
            step = note - 48
            with state.lock:
                row = state.selected_pad
                state.steps[row][step] = not state.steps[row][step]
        elif note == PLAY_NOTE:
            state.playing = True
            state.last_step_time = 0.0
        elif note == STOP_NOTE:
            state.playing = False
    elif status & 0xF0 == 0xB0 and len(msg) >= 3:
        cc, val = msg[1], msg[2]
        if cc == 1:
            state.bpm = 60 + (val / 127) * 140
        elif cc == 64:
            state.selected_pad = min(7, val % 8)


def run_midi_loop(port_name: str | None, state: EngineState) -> None:
    if rtmidi is None:
        print("python-rtmidi not installed; MIDI loop disabled", file=sys.stderr)
        return
    midi_in = rtmidi.MidiIn()
    ports = midi_in.get_ports()
    if not ports:
        print("No MIDI input ports. Install loopMIDI or connect Phonk Box.", file=sys.stderr)
        return
    idx = 0
    if port_name:
        for i, p in enumerate(ports):
            if port_name.lower() in p.lower():
                idx = i
                break
    midi_in.open_port(idx)
    print(f"MIDI in: {ports[idx]}")
    seq = Sequencer(state)
    while True:
        for msg, _ in midi_in.get_message() or []:
            handle_midi(msg, state)
        seq.advance()
        time.sleep(0.001)


def self_test(state: EngineState, dry_run: bool = False) -> None:
    if len(state.pads) != 8:
        state.pads = synthesize_beep()
    assert len(state.pads) == 8
    for i, pad in enumerate(state.pads):
        assert len(pad) > 0, f"pad {i} empty"
    if dry_run:
        print("phonk_engine: dry-run self-test OK (8 pads, no audio device)")
        return
    for i in range(8):
        state.pending_hits.append((i, 0.8))
    try:
        with sd.OutputStream(
            samplerate=SAMPLE_RATE,
            channels=1,
            callback=lambda out, frames, t, s: audio_callback(state, out, frames, t, s),
            blocksize=512,
        ):
            time.sleep(0.8)
    except (OSError, sd.PortAudioError) as e:
        print(f"phonk_engine: no audio output ({e}); pad synthesis OK", file=sys.stderr)
        return
    print("phonk_engine: self-test OK (8 beeps)")


def main() -> None:
    ap = argparse.ArgumentParser(description="DiskLordz Phonk Box engine")
    ap.add_argument("--kit", type=Path, help="Folder with 8 pad WAVs or sprint kit root (auto manifest)")
    ap.add_argument(
        "--manifest",
        type=Path,
        help="808 Lab spark-le-manifest.json (pad files + BPM)",
    )
    ap.add_argument("--midi", type=str, default="", help="Substring match MIDI port name")
    ap.add_argument("--self-test", action="store_true")
    ap.add_argument("--dry-run", action="store_true", help="Self-test without audio device")
    ap.add_argument("--bpm", type=float, default=None, help="Override manifest BPM")
    args = ap.parse_args()

    default_bpm = 140.0 if args.bpm is None else args.bpm
    pads, manifest_bpm, preset_id = resolve_kit_load(args.kit, args.manifest, default_bpm)
    state = EngineState(bpm=args.bpm if args.bpm is not None else manifest_bpm)
    state.pads = pads
    if preset_id and (args.manifest or args.kit):
        print(f"Kit: {preset_id} @ {state.bpm} BPM ({len(state.pads)} pads)")

    if args.self_test:
        self_test(state, dry_run=args.dry_run)
        return

    stream = sd.OutputStream(
        samplerate=SAMPLE_RATE,
        channels=1,
        callback=lambda out, frames, t, s: audio_callback(state, out, frames, t, s),
        blocksize=512,
    )
    with stream:
        try:
            run_midi_loop(args.midi or None, state)
        except KeyboardInterrupt:
            print("\nStopped.")


if __name__ == "__main__":
    main()
