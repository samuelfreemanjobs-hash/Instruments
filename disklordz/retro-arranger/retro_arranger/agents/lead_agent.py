"""Lead / solo call-and-response."""

from __future__ import annotations

from retro_arranger.agents.arranger_agent import section_bar_ranges
from retro_arranger.models import ArrangementSpec, MidiControl, MidiNote, StemTrack
from retro_arranger.timing import bar_tick, scale_pitch


def generate_lead_stem(spec: ArrangementSpec, tpq: int) -> StemTrack:
    track = StemTrack("lead", "07_Lead_Solo.mid", program=65, channel=4)
    degrees = [0, 2, 4, 2, 7, 4, 2, 0]
    for sec, start, end in section_bar_ranges(spec):
        if sec.value not in ("solo", "chorus", "bridge"):
            continue
        for bar in range(start, end):
            for i, deg in enumerate(degrees[:4]):
                t0 = bar_tick(bar, 1 + i * 0.5, tpq)
                pitch = scale_pitch(spec.key_root, deg, 4, spec.scale) + 12
                track.notes.append(MidiNote(t0, pitch, 96, tpq // 2))
                track.controls.append(MidiControl(t0, 1, 80 + i * 5, channel=4))
    return track
