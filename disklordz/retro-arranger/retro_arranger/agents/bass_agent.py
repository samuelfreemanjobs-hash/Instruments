"""Monophonic synth / electric bass."""

from __future__ import annotations

from retro_arranger.agents.arranger_agent import PROGRESSIONS, section_bar_ranges
from retro_arranger.models import ArrangementSpec, MidiNote, StemTrack
from retro_arranger.timing import bar_tick, scale_pitch


def generate_bass_stem(spec: ArrangementSpec, tpq: int, *, mode: str = "synth") -> StemTrack:
    program = 38 if mode == "synth" else 33
    track = StemTrack("bass", "02_Bass.mid", program=program, channel=3)
    last_note: int | None = None
    for sec, start, end in section_bar_ranges(spec):
        prog = PROGRESSIONS[sec]
        bar = start
        pi = 0
        while bar < end:
            deg = prog[pi % len(prog)][0]
            root = scale_pitch(spec.key_root, deg, 2, spec.scale)
            pattern = [0, 0, 7, 0, 5, 0, 7, 0] if mode == "synth" else [0, 0, 12, 7, 0, 5, 0, 7]
            for i, offset in enumerate(pattern):
                beat = i * 0.5
                t0 = bar_tick(bar, beat, tpq)
                note = root + offset
                if last_note is not None and note != last_note:
                    # ensure monophonic: shorten previous implicitly via non-overlap durations
                    pass
                vel = 105 if i == 0 else (38 if offset == 0 and i % 2 else 92)
                dur = tpq // 2 - 4
                track.notes.append(MidiNote(t0, note, vel, dur, channel=3))
                if mode == "electric" and i == 2:
                    track.notes.append(MidiNote(t0, note + 12, 115, tpq // 4, channel=3))
                last_note = note
            bar += 1
            pi += 1
    return track
