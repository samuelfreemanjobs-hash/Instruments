import struct
import wave
from pathlib import Path

import pytest
import torch

torch = pytest.importorskip("torch")
pytest.importorskip("torchaudio")

from drum_synth_blueprint.torch_ddsp.dataset import DrumSampleDataset
from drum_synth_blueprint.torch_ddsp.pipeline import train_ddsp_808_from_folder


def _write_test_wav(path: Path, sr: int = 44100, seconds: float = 0.1) -> None:
    n = int(sr * seconds)
    t = torch.linspace(0, seconds, n)
    samples = (0.5 * torch.sin(2 * torch.pi * 80 * t)).tolist()
    pcm = b"".join(struct.pack("<h", max(-32768, min(32767, int(s * 32767)))) for s in samples)
    with wave.open(str(path), "wb") as wf:
        wf.setnchannels(1)
        wf.setsampwidth(2)
        wf.setframerate(sr)
        wf.writeframes(pcm)


def test_drum_dataset_loads_and_normalizes(tmp_path):
    for i in range(3):
        _write_test_wav(tmp_path / f"kick_{i}.wav")
    ds = DrumSampleDataset(tmp_path, duration=0.05, log=False)
    assert len(ds) == 3
    sample = ds[0]
    assert sample.dim() == 1
    assert sample.shape[0] == ds.num_samples
    assert sample.abs().max() <= 1.0 + 1e-6


def test_drum_dataset_empty_folder_raises(tmp_path):
    with pytest.raises(FileNotFoundError):
        DrumSampleDataset(tmp_path)


def test_dataloader_uses_partial_final_batch(tmp_path):
    """drop_last=False — e.g. 5 files with batch 4 still trains on the fifth."""
    for i in range(5):
        _write_test_wav(tmp_path / f"kick_{i}.wav", seconds=0.05)
    ds = DrumSampleDataset(tmp_path, duration=0.05, log=False)
    from torch.utils.data import DataLoader

    loader = DataLoader(ds, batch_size=4, shuffle=False, drop_last=False)
    sizes = [batch.shape[0] for batch in loader]
    assert sizes == [4, 1]


def test_train_from_folder_smoke(tmp_path):
    for i in range(2):
        _write_test_wav(tmp_path / f"hit_{i}.wav", seconds=0.08)
    metrics = train_ddsp_808_from_folder(
        tmp_path,
        num_epochs=1,
        batch_size=2,
        duration_sec=0.08,
        export_path=tmp_path / "enc.onnx",
        verbose=False,
    )
    assert metrics["num_samples"] == 2
    assert (tmp_path / "enc.onnx").is_file()
