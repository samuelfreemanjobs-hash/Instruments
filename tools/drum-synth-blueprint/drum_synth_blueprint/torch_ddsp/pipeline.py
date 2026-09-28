"""Training loop and ONNX export for ddsp_808_encoder.onnx."""

from __future__ import annotations

from pathlib import Path

import torch

from drum_synth_blueprint.torch_ddsp.encoder import DDSP808Encoder, DDSP808TrainableSystem
from drum_synth_blueprint.torch_ddsp.spectral_loss import SpectralDistanceLoss
from drum_synth_blueprint.torch_ddsp.synth_torch import Differentiable808Synth


def _teacher_batch(
    batch_size: int,
    device: torch.device,
    *,
    duration_sec: float,
) -> torch.Tensor:
    """Self-supervised targets from random normalized synth parameters."""
    synth = Differentiable808Synth(duration_sec=duration_sec).to(device)
    params = torch.rand(batch_size, 4, device=device)
    with torch.no_grad():
        return synth(params)


def train_ddsp_808_smoke(
    *,
    steps: int = 40,
    batch_size: int = 4,
    lr: float = 1e-3,
    device: str | None = None,
    duration_sec: float = 0.5,
) -> dict[str, float]:
    """
    Encoder (mel) → synth → multi-scale spectral loss vs teacher waveforms.

    Returns final loss scalars for logging [executed].
    """
    dev = torch.device(device or ("cuda" if torch.cuda.is_available() else "cpu"))
    system = DDSP808TrainableSystem(duration_sec=duration_sec).to(dev)
    criterion = SpectralDistanceLoss().to(dev)
    opt = torch.optim.Adam(system.parameters(), lr=lr)

    last_loss = 0.0
    system.train()
    for _ in range(steps):
        target = _teacher_batch(batch_size, dev, duration_sec=duration_sec)
        opt.zero_grad(set_to_none=True)
        synth_wav, _params = system(target)
        loss = criterion(synth_wav, target)
        loss.backward()
        opt.step()
        last_loss = float(loss.detach().cpu())

    return {"final_spectral_loss": last_loss, "steps": float(steps)}


def export_ddsp_808_encoder_onnx(
    path: str | Path,
    *,
    n_mels: int = 128,
    mel_time_frames: int = 64,
    opset: int = 17,
) -> Path:
    """
    Export mel encoder as ``ddsp_808_encoder.onnx`` for JUCE ONNX Runtime.

    C++ computes ``mel_input`` offline; runs deterministic synth from ``synth_parameters``.
    """
    out = Path(path)
    out.parent.mkdir(parents=True, exist_ok=True)
    encoder = DDSP808Encoder(n_mels=n_mels)
    encoder.eval()
    dummy = torch.randn(1, 1, n_mels, mel_time_frames)
    torch.onnx.export(
        encoder,
        dummy,
        str(out),
        input_names=["mel_input"],
        output_names=["synth_parameters"],
        dynamic_axes={"mel_input": {0: "batch", 3: "time_frames"}, "synth_parameters": {0: "batch"}},
        opset_version=opset,
    )
    return out


def run_training_pipeline(
    export_path: str | Path = "ddsp_808_encoder.onnx",
    *,
    steps: int = 60,
    duration_sec: float = 2.0,
) -> dict[str, float | str]:
    metrics = train_ddsp_808_smoke(steps=steps, duration_sec=duration_sec)
    onnx_file = export_ddsp_808_encoder_onnx(export_path)
    metrics["onnx_path"] = str(onnx_file.resolve())
    return metrics
