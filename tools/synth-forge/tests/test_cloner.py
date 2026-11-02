import io
import struct
import wave

import numpy as np

from synth_forge.cloner import analyze_wav, clone_from_wav
from synth_forge.models import CloneAudioRequest


def _make_sine_wav(freq: float = 440.0, duration: float = 0.5, rate: int = 44100) -> bytes:
    t = np.linspace(0, duration, int(rate * duration), endpoint=False)
    samples = (0.5 * np.sin(2 * np.pi * freq * t) * 32767).astype(np.int16)
    buf = io.BytesIO()
    with wave.open(buf, "wb") as wf:
        wf.setnchannels(1)
        wf.setsampwidth(2)
        wf.setframerate(rate)
        wf.writeframes(samples.tobytes())
    return buf.getvalue()


def test_analyze_wav_f0():
    data = _make_sine_wav(440.0)
    analysis = analyze_wav(data)
    assert 400 < analysis["f0_hz"] < 480
    assert "note" in analysis


def test_clone_batch():
    data = _make_sine_wav(220.0)
    req = CloneAudioRequest(synth_id="minilogue_xd", count=4)
    presets = clone_from_wav(data, req)
    assert len(presets) == 4
    assert presets[0].name.startswith("clone_")
