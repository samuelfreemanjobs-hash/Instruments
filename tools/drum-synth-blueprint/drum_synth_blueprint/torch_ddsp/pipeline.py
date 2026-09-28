"""Training loop and ONNX export for ddsp_808_encoder.onnx."""

from __future__ import annotations

from pathlib import Path

import torch
import torch.nn as nn

from drum_synth_blueprint.torch_ddsp.encoder import DDSP808Encoder, DDSP808TrainableSystem
from drum_synth_blueprint.torch_ddsp.spectral_loss import SpectralDistanceLoss
from drum_synth_blueprint.torch_ddsp.synth_torch import Differentiable808Synth


def _teacher_batch(batch_size: int, device: torch.device) -> torch.Tensor:
    """Synthetic acoustic targets from random known params (self-supervised smoke)."""
    synth = Differentiable808Synth().to(device)
    raw = torch.randn(batch_size, 3, device=device).abs()
    params = torch.stack(
        [
            raw[:, 0] * 0.08 + 0.03,
            raw[:, 1] * 0.8 + 0.3,
            raw[:, 2] * 1.2 + 0.9,
        ],
        dim=1,
    )
    with torch.no_grad():
        return synth(params)


def train_ddsp_808_smoke(
    *,
    steps: int = 40,
    batch_size: int = 4,
    lr: float = 3e-3,
    device: str | None = None,
) -> dict[str, float]:
    """
    Short training run: encoder → synth → multi-scale spectral loss vs teacher waveforms.

    Returns final loss scalars for logging [executed].
    """
    dev = torch.device(device or ("cuda" if torch.cuda.is_available() else "cpu"))
    num_samples = 8192
    system = DDSP808TrainableSystem(num_samples=num_samples).to(dev)
    criterion = SpectralDistanceLoss()
    opt = torch.optim.Adam(system.parameters(), lr=lr)

    last_loss = 0.0
    system.train()
    for _ in range(steps):
        target = _teacher_batch(batch_size, dev)[..., :num_samples]
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
    num_samples: int = 8192,
    opset: int = 17,
) -> Path:
    """
    Export encoder only as ``ddsp_808_encoder.onnx`` for JUCE ONNX Runtime.

    C++ loads params and runs deterministic synth (see juce_onnx_pipeline_guide.txt).
    """
    out = Path(path)
    out.parent.mkdir(parents=True, exist_ok=True)
    encoder = DDSP808Encoder(num_samples=num_samples)
    encoder.eval()
    dummy = torch.randn(1, num_samples)
    torch.onnx.export(
        encoder,
        dummy,
        str(out),
        input_names=["waveform"],
        output_names=["params"],
        dynamic_axes={"waveform": {0: "batch", 1: "samples"}, "params": {0: "batch"}},
        opset_version=opset,
    )
    return out


def run_training_pipeline(
    export_path: str | Path = "ddsp_808_encoder.onnx",
    *,
    steps: int = 60,
) -> dict[str, float | str]:
    metrics = train_ddsp_808_smoke(steps=steps)
    onnx_file = export_ddsp_808_encoder_onnx(export_path)
    metrics["onnx_path"] = str(onnx_file.resolve())
    return metrics
