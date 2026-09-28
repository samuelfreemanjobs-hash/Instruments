"""Export stems and multitrack MIDI via mido."""

from __future__ import annotations

import io
import zipfile
from pathlib import Path

from retro_arranger.models import ArrangementResult, StemTrack


def _events_for_stem(stem: StemTrack, tpq: int):
    from mido import Message

    events: list[tuple[int, object]] = []
    for tick, val in stem.pitch_bends:
        events.append((tick, Message("pitchwheel", pitch=val - 8192, channel=stem.channel)))
    for cc in stem.controls:
        events.append(
            (cc.tick, Message("control_change", control=cc.control, value=cc.value, channel=cc.channel))
        )
    for note in stem.notes:
        events.append(
            (
                note.tick,
                Message(
                    "note_on",
                    note=note.note,
                    velocity=max(1, min(127, note.velocity)),
                    channel=note.channel,
                ),
            )
        )
        events.append(
            (
                note.tick + max(1, note.duration_ticks),
                Message("note_off", note=note.note, velocity=0, channel=note.channel),
            )
        )
    events.sort(key=lambda x: x[0])
    return events


def _bpm_to_tempo(bpm: int) -> int:
    return int(60_000_000 / bpm)


def stem_to_bytes(stem: StemTrack, tpq: int, bpm: int) -> bytes:
    from mido import MidiFile, MidiTrack, MetaMessage, Message

    mid = MidiFile(ticks_per_beat=tpq)
    track = MidiTrack()
    mid.tracks.append(track)
    track.append(MetaMessage("track_name", name=stem.stem_id, time=0))
    track.append(MetaMessage("set_tempo", tempo=_bpm_to_tempo(bpm), time=0))
    if stem.channel != 9:
        track.append(Message("program_change", program=stem.program, channel=stem.channel, time=0))

    events = _events_for_stem(stem, tpq)
    last = 0
    for tick, msg in events:
        delta = max(0, tick - last)
        msg.time = delta
        track.append(msg)
        last = tick

    buf = io.BytesIO()
    mid.save(file=buf)
    return buf.getvalue()


def multitrack_to_bytes(result: ArrangementResult) -> bytes:
    from mido import MidiFile, MidiTrack, MetaMessage, Message

    tpq = result.ticks_per_beat
    bpm = result.spec.bpm
    mid = MidiFile(ticks_per_beat=tpq)
    mid.tracks.append(MidiTrack())
    mid.tracks[0].append(MetaMessage("set_tempo", tempo=_bpm_to_tempo(bpm), time=0))
    mid.tracks[0].append(MetaMessage("track_name", name="Conductor", time=0))

    for stem in result.stems:
        tr = MidiTrack()
        mid.tracks.append(tr)
        tr.append(MetaMessage("track_name", name=stem.stem_id, time=0))
        if stem.channel != 9:
            tr.append(Message("program_change", program=stem.program, channel=stem.channel, time=0))
        events = _events_for_stem(stem, tpq)
        last = 0
        for tick, msg in events:
            delta = max(0, tick - last)
            msg.time = delta
            tr.append(msg)
            last = tick

    buf = io.BytesIO()
    mid.save(file=buf)
    return buf.getvalue()


def export_stems_to_dir(result: ArrangementResult, out_dir: Path) -> dict[str, Path]:
    out_dir.mkdir(parents=True, exist_ok=True)
    paths: dict[str, Path] = {}
    for stem in result.stems:
        p = out_dir / stem.file_name
        p.write_bytes(stem_to_bytes(stem, result.ticks_per_beat, result.spec.bpm))
        paths[stem.file_name] = p
    full = out_dir / "Full_Arrangement_Multitrack.mid"
    full.write_bytes(multitrack_to_bytes(result))
    paths[full.name] = full
    return paths


def stems_zip_bytes(result: ArrangementResult) -> bytes:
    buf = io.BytesIO()
    with zipfile.ZipFile(buf, "w", zipfile.ZIP_DEFLATED) as zf:
        for stem in result.stems:
            zf.writestr(stem.file_name, stem_to_bytes(stem, result.ticks_per_beat, result.spec.bpm))
        zf.writestr("Full_Arrangement_Multitrack.mid", multitrack_to_bytes(result))
    return buf.getvalue()
