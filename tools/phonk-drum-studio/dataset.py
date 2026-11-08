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

_RESAMPLE_CACHE: dict[int, torchaudio.transforms.Resample] = {}


def _get_resampler(orig_freq: int) -> torchaudio.transforms.Resample:
    if orig_freq not in _RESAMPLE_CACHE:
        _RESAMPLE_CACHE[orig_freq] = torchaudio.transforms.Resample(
            orig_freq=orig_freq, new_freq=SAMPLE_RATE
        )
    return _RESAMPLE_CACHE[orig_freq]


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
        waveform = torch.from_numpy(np.ascontiguousarray(data.T))
        return waveform, int(sr)
    except (OSError, RuntimeError, ValueError) as sf_exc:
        logger.debug("soundfile failed for %s (%s); trying torchaudio", path, sf_exc)
    try:
        waveform, sr = torchaudio.load(str(path))
        return waveform, int(sr)
    except (OSError, RuntimeError, ValueError, ImportError) as exc:
        logger.warning("Skipping unreadable file %s: %s", path, exc)
        return None


def load_and_preprocess(path: Path) -> Optional[torch.Tensor]:
    """Load mono 44.1 kHz waveform of length AUDIO_LENGTH, peak-normalized."""
    loaded = _load_waveform(path)
    if loaded is None:
        return None
    waveform, sr = loaded

    if waveform.numel() == 0:
        return None

    if waveform.shape[0] > 1:
        waveform = waveform.mean(dim=0, keepdim=True)

    if sr != SAMPLE_RATE:
        waveform = _get_resampler(sr)(waveform)

    length = waveform.shape[-1]
    if length < AUDIO_LENGTH:
        waveform = torch.nn.functional.pad(waveform, (0, AUDIO_LENGTH - length))
    elif length > AUDIO_LENGTH:
        waveform = waveform[..., :AUDIO_LENGTH]

    waveform = peak_normalize(waveform)
    return waveform.squeeze(0)


def scan_valid_audio_paths(directory: str | Path) -> Tuple[List[Path], int]:
    """Return paths that preprocess successfully; second value is corrupt/skip count."""
    root = Path(directory)
    candidates = list_audio_files(root)
    valid: List[Path] = []
    skipped = 0
    for path in candidates:
        if load_and_preprocess(path) is None:
            skipped += 1
            logger.warning("Excluding corrupt or unsupported audio: %s", path)
        else:
            valid.append(path)
    return valid, skipped


class DrumSampleDataset(Dataset):
    def __init__(self, directory: str | Path, *, paths: Optional[List[Path]] = None) -> None:
        self.root = Path(directory)
        if paths is not None:
            self.paths = list(paths)
        else:
            self.paths, _ = scan_valid_audio_paths(self.root)
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
                raise RuntimeError(f"Preprocess failed for indexed file: {path}")
            self._cache[path] = tensor
        return self._cache[path]


def collate_valid(batch: List[torch.Tensor]) -> torch.Tensor:
    return torch.stack(batch, dim=0)


def build_dataloader(
    directory: str | Path,
    batch_size: int,
    shuffle: bool = True,
    num_workers: int = 0,
    paths: Optional[List[Path]] = None,
) -> Tuple[DrumSampleDataset, torch.utils.data.DataLoader]:
    dataset = DrumSampleDataset(directory, paths=paths)
    loader = torch.utils.data.DataLoader(
        dataset,
        batch_size=batch_size,
        shuffle=shuffle and len(dataset) > 0,
        drop_last=len(dataset) >= batch_size,
        num_workers=num_workers,
        collate_fn=collate_valid,
    )
    return dataset, loader
