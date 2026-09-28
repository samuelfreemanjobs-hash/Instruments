import numpy as np
import pytest

pytest.importorskip("librosa")
pytest.importorskip("soundfile")

from drum_synth_blueprint.phonk_kit.slicer import HitLabel, classify_hit, slice_loop_to_hits


def _synthetic_kick(sr: int = 44100, ms: int = 400) -> np.ndarray:
    n = int(sr * ms / 1000)
    t = np.arange(n) / sr
    freq = 45 + 280 * np.exp(-t * 18)
    phase = 2 * np.pi * np.cumsum(freq / sr)
    return np.sin(phase) * np.exp(-t * 6)


def test_classify_kick_like():
    y = _synthetic_kick()
    label, conf = classify_hit(y, 44100)
    assert label == HitLabel.KICK
    assert conf > 0.5


def test_slice_finds_multiple_onsets():
    sr = 44100
    gap = int(0.35 * sr)
    hit = _synthetic_kick(ms=120)
    mono = np.zeros(gap * 3 + len(hit), dtype=np.float64)
    for i in range(3):
        off = i * gap
        mono[off : off + len(hit)] = hit
    slices = slice_loop_to_hits(mono, sr, min_gap_ms=120)
    assert len(slices) >= 2
    assert any(spec.label == HitLabel.KICK for _, spec in slices)
