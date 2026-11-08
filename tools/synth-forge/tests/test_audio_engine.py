from synth_forge.audio_engine import measure_audio, write_preview_wav
from synth_forge.models import SynthParameters
from synth_forge.safety import MAX_PREVIEW_PEAK_DBFS
import numpy as np


def test_preview_respects_peak_limit(tmp_path, monkeypatch):
    import synth_forge.audio_engine as ae

    monkeypatch.setattr(ae, "PREVIEW_DIR", tmp_path)
    path, metrics = write_preview_wav(SynthParameters(master_volume=0.99), "peak-test")
    assert path.exists()
    assert metrics["peak_dbfs"] <= MAX_PREVIEW_PEAK_DBFS


def test_measure_audio_metrics():
    samples = np.sin(np.linspace(0, 1, 1000)).astype(np.float32) * 0.1
    m = measure_audio(samples)
    assert "spectral_centroid_hz" in m
    assert m["peak_dbfs"] < 0
