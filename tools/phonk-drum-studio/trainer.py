"""Background training worker for the phonk drum VAE."""

from __future__ import annotations

import logging
import threading
from dataclasses import dataclass
from pathlib import Path
from typing import Callable, Optional

import torch
from torch.utils.data import DataLoader

from dataset import DrumSampleDataset, build_dataloader
from model import (
    LATENT_DIM,
    AudioVAE,
    MultiScaleSpectralLoss,
    kl_divergence,
)

logger = logging.getLogger(__name__)

ProgressCallback = Callable[[int, int, float], None]
CompleteCallback = Callable[[Path], None]
ErrorCallback = Callable[[str], None]

DEFAULT_CHECKPOINT = "phonk_vae.pth"
KL_WEIGHT = 1e-4


def select_device() -> torch.device:
    if torch.cuda.is_available():
        return torch.device("cuda")
    if getattr(torch.backends, "mps", None) and torch.backends.mps.is_available():
        return torch.device("mps")
    return torch.device("cpu")


def device_label(device: torch.device) -> str:
    if device.type == "cuda":
        name = torch.cuda.get_device_name(device)
        return f"CUDA ({name})"
    if device.type == "mps":
        return "Apple Silicon (MPS)"
    return "CPU"


@dataclass
class TrainConfig:
    data_dir: Path
    epochs: int = 100
    batch_size: int = 16
    learning_rate: float = 3e-4
    checkpoint_path: Path = Path(DEFAULT_CHECKPOINT)
    kl_weight: float = KL_WEIGHT


class TrainingWorker:
    """Runs VAE training on a background thread with cooperative cancellation."""

    def __init__(
        self,
        config: TrainConfig,
        progress_callback: Optional[ProgressCallback] = None,
        complete_callback: Optional[CompleteCallback] = None,
        error_callback: Optional[ErrorCallback] = None,
    ) -> None:
        self.config = config
        self.progress_callback = progress_callback
        self.complete_callback = complete_callback
        self.error_callback = error_callback
        self._stop_event = threading.Event()
        self._thread: Optional[threading.Thread] = None

    @property
    def is_running(self) -> bool:
        return self._thread is not None and self._thread.is_alive()

    def request_stop(self) -> None:
        self._stop_event.set()

    def start(self) -> None:
        if self.is_running:
            return
        self._stop_event.clear()
        self._thread = threading.Thread(target=self._run, name="PhonkVAETrainer", daemon=True)
        self._thread.start()

    def join(self, timeout: Optional[float] = None) -> None:
        if self._thread is not None:
            self._thread.join(timeout=timeout)

    def _emit_progress(self, epoch: int, total: int, loss: float) -> None:
        if self.progress_callback:
            self.progress_callback(epoch, total, loss)

    def _emit_error(self, message: str) -> None:
        logger.error(message)
        if self.error_callback:
            self.error_callback(message)

    def _run(self) -> None:
        try:
            self._train_loop()
        except Exception as exc:
            self._emit_error(str(exc))

    def _train_loop(self) -> None:
        data_dir = self.config.data_dir
        dataset, loader = build_dataloader(
            data_dir,
            batch_size=self._resolve_batch_size(),
            shuffle=True,
        )
        if len(dataset) == 0:
            self._emit_error("No valid audio files found in the selected folder.")
            return

        device = select_device()
        model = AudioVAE(latent_dim=LATENT_DIM).to(device)
        spectral = MultiScaleSpectralLoss().to(device)
        optimizer = torch.optim.Adam(model.parameters(), lr=self.config.learning_rate)

        model.train()
        total_epochs = self.config.epochs

        for epoch in range(1, total_epochs + 1):
            if self._stop_event.is_set():
                logger.info("Training stopped by user at epoch %s", epoch)
                break

            epoch_loss = 0.0
            batches = 0
            for batch in loader:
                if self._stop_event.is_set():
                    break
                wave = batch.to(device).unsqueeze(1)
                optimizer.zero_grad(set_to_none=True)
                recon, mu, logvar = model(wave)
                spec_loss = spectral(recon.squeeze(1), wave.squeeze(1))
                kld = kl_divergence(mu, logvar)
                loss = spec_loss + self.config.kl_weight * kld
                loss.backward()
                optimizer.step()
                epoch_loss += float(loss.item())
                batches += 1

            if batches == 0:
                self._emit_error("Training produced no batches (dataset too small for batch size).")
                return

            mean_loss = epoch_loss / batches
            self._emit_progress(epoch, total_epochs, mean_loss)

        if self._stop_event.is_set():
            return

        ckpt = self.config.checkpoint_path
        ckpt.parent.mkdir(parents=True, exist_ok=True)
        payload = {
            "model_state_dict": model.state_dict(),
            "latent_dim": LATENT_DIM,
            "config": {
                "epochs": self.config.epochs,
                "batch_size": self.config.batch_size,
                "learning_rate": self.config.learning_rate,
            },
        }
        torch.save(payload, ckpt)
        logger.info("Saved checkpoint to %s", ckpt)
        if self.complete_callback:
            self.complete_callback(ckpt)

    def _resolve_batch_size(self) -> int:
        requested = self.config.batch_size
        dataset = DrumSampleDataset(self.config.data_dir)
        if len(dataset) < requested:
            return max(1, min(requested, len(dataset)))
        device = select_device()
        if device.type == "cpu" and requested > 8:
            return 8
        return requested


def load_model_from_checkpoint(path: Path, device: Optional[torch.device] = None) -> AudioVAE:
    device = device or select_device()
    payload = torch.load(path, map_location=device, weights_only=False)
    latent_dim = int(payload.get("latent_dim", LATENT_DIM))
    model = AudioVAE(latent_dim=latent_dim)
    model.load_state_dict(payload["model_state_dict"])
    model.to(device)
    model.eval()
    return model
