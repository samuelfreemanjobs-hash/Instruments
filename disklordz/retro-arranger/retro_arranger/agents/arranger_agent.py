"""Bandleader: form, harmony, global groove."""

from __future__ import annotations

from retro_arranger.models import ArrangementSpec, SectionId

# Jazz-R&B extensions per section (scale degrees as chord roots)
PROGRESSIONS: dict[SectionId, list[tuple[int, str]]] = {
    SectionId.intro: [(0, "m9"), (5, "9"), (3, "m7"), (4, "13")],
    SectionId.verse: [(0, "m9"), (5, "9"), (3, "m7"), (4, "13")] * 2,
    SectionId.pre_chorus: [(1, "m7"), (4, "7alt"), (0, "m9"), (5, "9")],
    SectionId.chorus: [(0, "m9"), (5, "9"), (3, "m7"), (4, "13")] * 2,
    SectionId.bridge: [(2, "m7"), (5, "9"), (1, "m7b5"), (4, "7alt")],
    SectionId.solo: [(0, "m9"), (5, "9"), (3, "m7"), (4, "13")] * 2,
    SectionId.outro: [(0, "m9"), (5, "9"), (3, "m7"), (0, "m9")],
}


def chord_voicing_rootless(root: str, scale: str, degree: int, quality: str) -> list[int]:
    """Return MIDI note numbers for rootless voicing (extensions emphasized)."""
    from retro_arranger.timing import scale_pitch

    r = scale_pitch(root, degree, 3, scale)
    ext = {
        "m9": [3, 7, 10, 14],
        "9": [4, 7, 10, 14],
        "m7": [3, 7, 10],
        "13": [4, 7, 10, 14, 21],
        "7alt": [4, 8, 10, 13],
        "m7b5": [3, 6, 10],
    }.get(quality, [4, 7, 10])
    return [r + x for x in ext]


def section_bar_ranges(spec: ArrangementSpec) -> list[tuple[SectionId, int, int]]:
    """Return (section, start_bar, end_bar) exclusive end."""
    out: list[tuple[SectionId, int, int]] = []
    cursor = 0
    for sec, bars in spec.sections:
        out.append((sec, cursor, cursor + bars))
        cursor += bars
    return out
