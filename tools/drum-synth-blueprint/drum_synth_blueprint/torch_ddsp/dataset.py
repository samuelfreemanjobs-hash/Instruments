"""Flat-folder .wav dataset for DDSP 808 encoder training."""

from __future__ import annotations

import struct
import wave
from pathlib import Path

import torch
import torch.nn.functional as F
import torchaudio.transforms as T
from torch.utils.data import Dataset


def _load_wav(path: Path) -> tuple[torch.Tensor, int]:
    """Load mono/stereo PCM ``.wav`` without TorchCodec (stdlib only)."""
    with wave.open(str(path), "rb") as wf:
        sr = wf.getframerate()
        nch = wf.getnchannels()
        width = wf.getsampwidth()
        nframes = wf.getnframes()
        raw = wf.readframes(nframes)
    if width == 2:
        samples = torch.tensor(struct.unpack(f"<{nframes * nch}h", raw), dtype=torch.float32) / 32768.0
    elif width == 4:
        samples = torch.tensor(struct.unpack(f"<{nframes * nch}i", raw), dtype=torch.float32) / 2147483648.0
    else:
        raise ValueError(f"Unsupported WAV sample width: {width} bytes in {path}")
    if nch > 1:
        samples = samples.view(nframes, nch).mean(dim=1)
    else:
        samples = samples.view(nframes)
    return samples.unsqueeze(0), sr


class DrumSampleDataset(Dataset):
    """
    Reads every ``.wav`` in a flat folder; returns fixed-length mono waveforms.

    Resamples to ``target_sr``, zero-pads or trims to ``duration`` seconds,
    peak-normalizes to 1.0.
    """

    SUPPORTED_EXT = {".wav"}

    def __init__(self, folder_path: str | Path, target_sr: int = 44100, duration: float = 2.0) -> None:
        self.target_sr = target_sr
        self.num_samples = int(target_sr * duration)
        folder = Path(folder_path)
        if not folder.is_dir():
            raise FileNotFoundError(f"[DrumSampleDataset] Not a directory: {folder}")

        self.file_paths = sorted(
            p
            for p in folder.iterdir()
            if p.is_file() and p.suffix.lower() in self.SUPPORTED_EXT
        )
        if not self.file_paths:
            raise FileNotFoundError(
                f"[DrumSampleDataset] No .wav files found in: {folder}\n"
                "Use a flat folder (no nested sub-directories)."
            )

    def __len__(self) -> int:
        return len(self.file_paths)

    def _resample(self, waveform: torch.Tensor, sr: int) -> torch.Tensor:
        if sr == self.target_sr:
            return waveform
        if sr not in self._resamplers:
            self._resamplers[sr] = T.Resample(orig_freq=sr, new_freq=self.target_sr)
        return self._resamplers[sr](waveform)

    def __getitem__(self, idx: int) -> torch.Tensor:
        path = self.file_paths[idx]
        waveform, sr = _load_wav(path)
        if waveform.shape[0] > 1:
            waveform = waveform.mean(dim=0, keepdim=True)
        waveform = self._resample(waveform, sr)
        waveform = waveform.squeeze(0)

        current_len = waveform.shape[0]
        if current_len < self.num_samples:
            waveform = F.pad(waveform, (0, self.num_samples - current_len))
        elif current_len > self.num_samples:
            waveform = waveform[: self.num_samples]

        peak = waveform.abs().max()
        if peak > 0:
            waveform = waveform / peak
        return waveform
