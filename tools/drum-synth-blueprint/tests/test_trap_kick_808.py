import numpy as np

from drum_synth_blueprint.trap_kick_808 import normalize_peak, render_trap_808, render_trap_kick


def test_kick_has_onset_energy():
    k = render_trap_kick()
    peak_early = float(np.max(np.abs(k[:500])))
    assert peak_early > 0.05


def test_808_glide_changes_tail():
    short = render_trap_808(glide_ms=0, glide_semitones=0)
    glide = render_trap_808(glide_ms=120, glide_semitones=7)
    assert len(glide) > 44100
    assert not np.allclose(short, glide)


def test_normalize_bounded():
    x = normalize_peak(render_trap_kick() * 10)
    assert float(np.max(np.abs(x))) <= 0.951
