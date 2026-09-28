import pytest

from synth_forge.models import SynthParameters
from synth_forge.safety import (
    MAX_DELAY_FEEDBACK,
    MAX_FILTER_RESONANCE,
    MAX_MASTER_VOLUME,
    MAX_PREVIEW_PEAK_DBFS,
    clamp_for_hardware,
    clamp_preview_peak_dbfs,
)


def test_clamp_resonance():
    p = SynthParameters(filter_resonance=0.99)
    out = clamp_for_hardware(p)
    assert out.filter_resonance == MAX_FILTER_RESONANCE


def test_clamp_delay_feedback():
    p = SynthParameters(delay_feedback=0.95)
    out = clamp_for_hardware(p)
    assert out.delay_feedback == MAX_DELAY_FEEDBACK


def test_clamp_master_volume():
    p = SynthParameters(master_volume=1.0)
    out = clamp_for_hardware(p)
    assert out.master_volume == MAX_MASTER_VOLUME


def test_preview_peak_cap():
    assert clamp_preview_peak_dbfs(0.0) == MAX_PREVIEW_PEAK_DBFS
    assert clamp_preview_peak_dbfs(-20.0) == -20.0


def test_clamp_does_not_mutate_input():
    p = SynthParameters(filter_resonance=0.99)
    clamp_for_hardware(p)
    assert p.filter_resonance == 0.99
