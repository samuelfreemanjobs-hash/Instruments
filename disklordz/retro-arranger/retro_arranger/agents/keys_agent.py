"""Keys: Rhodes, pads, DX7 stabs."""

from __future__ import annotations

from retro_arranger.agents.arranger_agent import PROGRESSIONS, chord_voicing_rootless, section_bar_ranges
from retro_arranger.models import ArrangementSpec, MidiControl, MidiNote, StemTrack
from retro_arranger.timing import bar_tick


def _rhodes_stem(spec: ArrangementSpec, tpq: int) -> StemTrack:
    track = StemTrack("electric_piano", "03_Electric_Piano.mid", program=5, channel=0)
    for sec, start, end in section_bar_ranges(spec):
        prog = PROGRESSIONS[sec]
        bar = start
        pi = 0
        while bar < end:
            qual = prog[pi % len(prog)][1]
            deg = prog[pi % len(prog)][0]
            voicing = chord_voicing_rootless(spec.key_root, spec.scale, deg, qual)
            t0 = bar_tick(bar, 0, tpq)
            vel = 72 if sec.value in ("chorus", "solo") else 64
            for n in voicing:
                track.notes.append(MidiNote(t0, n, vel, tpq * 2))
            bar += 1
            pi += 1
    return track


def _pad_stem(spec: ArrangementSpec, tpq: int) -> StemTrack:
    track = StemTrack("analog_pad", "04_Analog_Pad.mid", program=89, channel=1)
    for sec, start, end in section_bar_ranges(spec):
        if sec.value in ("intro", "outro", "bridge"):
            for bar in range(start, end):
                t0 = bar_tick(bar, 0, tpq)
                for n in [57, 60, 64, 67, 71]:
                    track.notes.append(MidiNote(t0, n + 12, 58, tpq * 4))
                track.controls.append(MidiControl(t0, 11, 90, channel=1))
                track.controls.append(MidiControl(t0 + tpq * 2, 1, 70, channel=1))
    return track


def _stabs_stem(spec: ArrangementSpec, tpq: int) -> StemTrack:
    track = StemTrack("polysynth_stabs", "05_Polysynth_Stabs.mid", program=80, channel=2)
    for sec, start, end in section_bar_ranges(spec):
        if sec.value not in ("chorus", "pre_chorus", "solo"):
            continue
        for bar in range(start, end):
            if bar % 2 == 1:
                t0 = bar_tick(bar, 2.5, tpq)
                for n in [62, 65, 69]:
                    track.notes.append(MidiNote(t0, n, 88, tpq // 2))
    return track


def generate_keys_stems(spec: ArrangementSpec, tpq: int) -> list[StemTrack]:
    return [_rhodes_stem(spec, tpq), _pad_stem(spec, tpq), _stabs_stem(spec, tpq)]
