import pytest

from synth_forge.models import SynthParameters
from synth_forge.synths.minifreak import MiniFreakAdapter
from synth_forge.synths.zenology import ZenologyAdapter


def test_stub_flags():
    assert MiniFreakAdapter().stub is True
    assert ZenologyAdapter().stub is True


def test_stub_container_headers():
    params = SynthParameters()
    mf = MiniFreakAdapter().pack(params)
    assert mf[:4] == b"MNFK"
    z = ZenologyAdapter().pack(params)
    assert z[:3] == b"SVE"
