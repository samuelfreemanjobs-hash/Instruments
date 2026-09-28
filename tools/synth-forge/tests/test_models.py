import pytest

from synth_forge.models import SynthParameters


def test_timbre_vector_length():
    v = SynthParameters().to_timbre_vector()
    assert len(v) == 14


def test_cosine_similarity_identical():
    a = SynthParameters().to_timbre_vector()
    sim = SynthParameters.cosine_similarity(a, a)
    assert sim == pytest.approx(1.0)


def test_cosine_similarity_orthogonal():
    a = [1.0, 0.0]
    b = [0.0, 1.0]
    assert SynthParameters.cosine_similarity(a, b) == pytest.approx(0.0)
