"""PyTorch dataset for phonk drum one-shots."""

from __future__ import annotations

import logging
from pathlib import Path
from typing import List, Optional, Tuple

import numpy as np
import soundfile as sf
import torch
import torchaudio
from torch.utils.data import Dataset

from model import AUDIO_LENGTH, SAMPLE_RATE, peak_normalize

logger = logging.getLogger(__name__)

AUDIO_EXTENSIONS = {".wav", ".aif", ".aiff", ".WAV", ".AIF", ".AIFF"}


def list_audio_files(root: Path) -> List[Path]:
    if not root.is_dir():
        return []
    files: List[Path] = []
    for path in sorted(root.rglob("*")):
        if path.is_file() and path.suffix in AUDIO_EXTENSIONS:
            files.append(path)
    return files


def _load_waveform(path: Path) -> Optional[Tuple[torch.Tensor, int]]:
    """Load audio via soundfile; fall back to torchaudio when soundfile cannot read the file."""
    try:
        data, sr = sf.read(str(path), always_2d=True, dtype="float32")
        if data.size == 0:
            return None
        # soundfile: (frames, channels) -> torch (channels, frames)
        waveform = torch.from_numpy(np.ascontiguousarray(data.T))
        return waveform, int(sr)
    except (OSError, RuntimeError, ValueError) as sf_exc:
        logger.debug("soundfile failed for %s (%s); trying torchaudio", path, sf_exc)
    try:
        waveform, sr = torchaudio.load(str(path))
        return waveform, int(sr)
    except (OSError, RuntimeError, ValueError) as exc:
        logger.warning("Skipping unreadable file %s: %s", path, exc)
        return None


def load_and_preprocess(path: Path) -> Optional[torch.Tensor]:
    """Load mono 44.1 kHz waveform of length AUDIO_LENGTH, peak-normalized."""
    loaded = _load_waveform(path)
    if loaded is None:
        logger.warning("Skipping empty or unreadable file %s", path)
        return None
    waveform, sr = loaded

    if waveform.numel() == 0:
        logger.warning("Skipping empty file %s", path)
        return None

    if waveform.shape[0] > 1:
        waveform = waveform.mean(dim=0, keepdim=True)

    if sr != SAMPLE_RATE:
        resampler = torchaudio.transforms.Resample(orig_freq=sr, new_freq=SAMPLE_RATE)
        waveform = resampler(waveform)

    length = waveform.shape[-1]
    if length < AUDIO_LENGTH:
        waveform = torch.nn.functional.pad(waveform, (0, AUDIO_LENGTH - length))
    elif length > AUDIO_LENGTH:
        waveform = waveform[..., :AUDIO_LENGTH]

    waveform = peak_normalize(waveform)
    return waveform.squeeze(0)


class DrumSampleDataset(Dataset):
    def __init__(self, directory: str | Path) -> None:
        self.root = Path(directory)
        self.paths = list_audio_files(self.root)
        self._cache: dict[Path, torch.Tensor] = {}

    def __len__(self) -> int:
        return len(self.paths)

    @property
    def file_count(self) -> int:
        return len(self.paths)

    def __getitem__(self, index: int) -> torch.Tensor:
        path = self.paths[index]
        if path not in self._cache:
            tensor = load_and_preprocess(path)
            if tensor is None:
                tensor = torch.zeros(AUDIO_LENGTH)
            self._cache[path] = tensor
        return self._cache[path]


def collate_valid(batch: List[torch.Tensor]) -> torch.Tensor:
    return torch.stack(batch, dim=0)


def build_dataloader(
    directory: str | Path,
    batch_size: int,
    shuffle: bool = True,
    num_workers: int = 0,
) -> Tuple[DrumSampleDataset, torch.utils.data.DataLoader]:
    dataset = DrumSampleDataset(directory)
    loader = torch.utils.data.DataLoader(
        dataset,
        batch_size=batch_size,
        shuffle=shuffle and len(dataset) > 0,
        drop_last=len(dataset) >= batch_size,
        num_workers=num_workers,
        collate_fn=collate_valid,
    )
    return dataset, loader
