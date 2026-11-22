"""Orchestrate session band agents."""

from __future__ import annotations

from retro_arranger.agents.bass_agent import generate_bass_stem
from retro_arranger.agents.drum_agent import generate_drum_stem
from retro_arranger.agents.guitar_agent import generate_guitar_stem
from retro_arranger.agents.keys_agent import generate_keys_stems
from retro_arranger.agents.lead_agent import generate_lead_stem
from retro_arranger.models import ArrangementResult, ArrangementSpec


def compose(spec: ArrangementSpec | None = None, *, bass_mode: str = "synth") -> ArrangementResult:
    spec = spec or ArrangementSpec()
    tpq = 480
    stems: list = []
    stems.append(generate_drum_stem(spec, tpq))
    stems.append(generate_bass_stem(spec, tpq, mode=bass_mode))
    stems.extend(generate_keys_stems(spec, tpq))
    stems.append(generate_guitar_stem(spec, tpq))
    stems.append(generate_lead_stem(spec, tpq))
    return ArrangementResult(spec=spec, stems=stems, ticks_per_beat=tpq)
