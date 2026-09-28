"""LinnDrum / 808 / 707 inspired GM drum map."""

from __future__ import annotations

from retro_arranger.models import ArrangementSpec, MidiNote, StemTrack
from retro_arranger.timing import apply_swing_16th, bar_tick, humanize_ticks

# GM drums on channel 9
KICK, SNARE, CLAP = 36, 38, 39
CHH, PHH, OHH = 42, 44, 46
RIM, TAMBO, COW = 37, 54, 56


def generate_drum_stem(spec: ArrangementSpec, tpq: int) -> StemTrack:
    track = StemTrack("drums", "01_Drums.mid", program=0, channel=9)
    seed = hash(spec.title) & 0xFFFF
    for bar in range(spec.total_bars):
        for sixteenth in range(16):
            beat = sixteenth * 0.25
            tick = bar_tick(bar, beat, tpq)
            offbeat = sixteenth % 2 == 1
            tick = apply_swing_16th(tick, tpq, spec.swing_pct, offbeat)
            tick = humanize_ticks(tick, tpq, seed + bar * 16 + sixteenth, max_shift=8)

            if sixteenth == 0:
                track.notes.append(MidiNote(tick, KICK, 110, tpq // 4, channel=9))
            if sixteenth in (4, 12):
                track.notes.append(MidiNote(tick, SNARE, 102, tpq // 4, channel=9))
            if sixteenth in (3, 7, 11, 15):
                track.notes.append(MidiNote(tick, SNARE, 28, tpq // 8, channel=9))
            if sixteenth % 2 == 0:
                track.notes.append(MidiNote(tick, CHH, 78 if offbeat else 68, tpq // 8, channel=9))
            if bar % 4 == 3 and sixteenth == 14:
                track.notes.append(MidiNote(tick, OHH, 95, tpq // 4, channel=9))
            if bar % 8 == 4 and sixteenth == 8:
                track.notes.append(MidiNote(tick, CLAP, 90, tpq // 4, channel=9))
            if bar % 2 == 1 and sixteenth == 6:
                track.notes.append(MidiNote(tick, COW, 80, tpq // 6, channel=9))
    return track
