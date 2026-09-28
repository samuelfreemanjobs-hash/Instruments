"""Mandatory hardware safety clamps (all generation and export paths)."""

from __future__ import annotations

from synth_forge.models import SynthParameters

MAX_FILTER_RESONANCE = 0.85
MAX_DELAY_FEEDBACK = 0.75
MAX_MASTER_VOLUME = 0.90
MAX_PREVIEW_PEAK_DBFS = -12.0


def _clamp01(value: float, upper: float) -> float:
    return max(0.0, min(float(value), upper))


def clamp_for_hardware(params: SynthParameters) -> SynthParameters:
    """Return a copy with resonance, FX feedback, and master volume bounded."""
    data = params.model_dump()
    data["filter_resonance"] = _clamp01(data["filter_resonance"], MAX_FILTER_RESONANCE)
    data["delay_feedback"] = _clamp01(data["delay_feedback"], MAX_DELAY_FEEDBACK)
    data["master_volume"] = _clamp01(data["master_volume"], MAX_MASTER_VOLUME)
    return SynthParameters.model_validate(data)


def clamp_preview_peak_dbfs(peak_dbfs: float) -> float:
    """Limit reported / target preview peak to safe headroom."""
    return min(float(peak_dbfs), MAX_PREVIEW_PEAK_DBFS)
