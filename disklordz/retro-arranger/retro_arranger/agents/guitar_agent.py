"""Nile Rodgers-style 16th funk comp."""

from __future__ import annotations

from retro_arranger.agents.arranger_agent import PROGRESSIONS, section_bar_ranges
from retro_arranger.models import ArrangementSpec, MidiNote, StemTrack
from retro_arranger.timing import apply_swing_16th, bar_tick


def generate_guitar_stem(spec: ArrangementSpec, tpq: int) -> StemTrack:
    track = StemTrack("guitar", "06_Rhythm_Guitar.mid", program=27, channel=5)
    for sec, start, end in section_bar_ranges(spec):
        if sec.value in ("intro", "outro"):
            continue
        prog = PROGRESSIONS[sec]
        bar = start
        pi = 0
        while bar < end:
            deg = prog[pi % len(prog)][0]
            from retro_arranger.timing import scale_pitch

            root = scale_pitch(spec.key_root, deg, 3, spec.scale)
            shape = [0, 3, 7, 3, 0, 3, 7, 10, 0, 3, 7, 3, 0, 5, 7, 3]
            for i, interval in enumerate(shape):
                sixteenth = i
                beat = sixteenth * 0.25
                tick = bar_tick(bar, beat, tpq)
                tick = apply_swing_16th(tick, tpq, spec.swing_pct, sixteenth % 2 == 1)
                vel = 72 if i % 4 == 0 else 58
                track.notes.append(
                    MidiNote(tick, root + interval, vel, tpq // 4 - 2, channel=5)
                )
            bar += 1
            pi += 1
    return track
