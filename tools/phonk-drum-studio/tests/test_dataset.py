"""Dataset loading contract tests."""

from pathlib import Path

import numpy as np
import soundfile as sf

from dataset import DrumSampleDataset, load_and_preprocess
from model import AUDIO_LENGTH, SAMPLE_RATE


def test_load_and_preprocess_wav(tmp_path: Path):
    t = np.linspace(0, 0.5, int(SAMPLE_RATE * 0.5), endpoint=False, dtype=np.float32)
    wave = (0.5 * np.sin(2 * np.pi * 120 * t)).astype(np.float32)
    path = tmp_path / "kick.wav"
    sf.write(path, wave, SAMPLE_RATE)

    tensor = load_and_preprocess(path)
    assert tensor is not None
    assert tensor.shape == (AUDIO_LENGTH,)
    assert float(tensor.abs().max()) <= 1.0 + 1e-5


def test_dataset_file_count(tmp_path: Path):
    for i in range(3):
        sf.write(
            tmp_path / f"s_{i}.wav",
            np.zeros(1000, dtype=np.float32),
            SAMPLE_RATE,
        )
    ds = DrumSampleDataset(tmp_path)
    assert len(ds) == 3
    item = ds[0]
    assert item.shape == (AUDIO_LENGTH,)
