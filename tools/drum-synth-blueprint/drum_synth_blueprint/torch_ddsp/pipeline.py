"""Training loop and ONNX export for ddsp_808_encoder.onnx."""

from __future__ import annotations

from pathlib import Path

import torch
from torch.utils.data import DataLoader

from drum_synth_blueprint.torch_ddsp.dataset import DrumSampleDataset
from drum_synth_blueprint.torch_ddsp.encoder import DDSP808Encoder, DDSP808TrainableSystem
from drum_synth_blueprint.torch_ddsp.mel_front_end import MEL_N_MELS, make_mel_spectrogram
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


def train_ddsp_808_from_folder(
    drum_folder: str | Path,
    *,
    num_epochs: int = 20,
    batch_size: int = 4,
    learning_rate: float = 1e-3,
    sample_rate: int = 44100,
    duration_sec: float = 2.0,
    device: str | None = None,
    num_workers: int = 0,
    export_path: str | Path | None = None,
) -> dict[str, float | int | str]:
    """
    Train encoder on real ``.wav`` clips: mel → params → differentiable synth → spectral loss.

    Only ``DDSP808Encoder`` weights are optimized (synth is fixed math).
    """
    dev = torch.device(device or ("cuda" if torch.cuda.is_available() else "cpu"))
    dataset = DrumSampleDataset(drum_folder, target_sr=sample_rate, duration=duration_sec)
    loader = DataLoader(
        dataset,
        batch_size=batch_size,
        shuffle=True,
        num_workers=num_workers,
        drop_last=False,
        pin_memory=dev.type == "cuda",
    )

    encoder = DDSP808Encoder().to(dev)
    synth = Differentiable808Synth(sample_rate=float(sample_rate), duration_sec=duration_sec)
    criterion = SpectralDistanceLoss().to(dev)
    mel_transform = make_mel_spectrogram(sample_rate=sample_rate).to(dev)
    optimizer = torch.optim.Adam(encoder.parameters(), lr=learning_rate)

    mel_time_frames = 0
    last_avg = 0.0
    encoder.train()
    for _epoch in range(num_epochs):
        epoch_loss = 0.0
        batch_count = 0
        for target_audio in loader:
            target_audio = target_audio.to(dev)
            mel_features = mel_transform(target_audio).unsqueeze(1)
            if mel_time_frames == 0:
                mel_time_frames = mel_features.shape[-1]

            optimizer.zero_grad(set_to_none=True)
            predicted = encoder(mel_features)
            synthesized = synth(predicted)
            n = min(target_audio.shape[-1], synthesized.shape[-1])
            loss = criterion(synthesized[..., :n], target_audio[..., :n])
            loss.backward()
            optimizer.step()

            epoch_loss += float(loss.detach().cpu())
            batch_count += 1
        last_avg = epoch_loss / max(batch_count, 1)

    result: dict[str, float | int | str] = {
        "final_avg_spectral_loss": last_avg,
        "epochs": num_epochs,
        "num_samples": len(dataset),
        "mel_time_frames": mel_time_frames,
    }
    if export_path is not None:
        encoder.eval()
        onnx_file = export_ddsp_808_encoder_onnx(
            export_path,
            encoder=encoder,
            mel_time_frames=max(mel_time_frames, 1),
        )
        result["onnx_path"] = str(onnx_file.resolve())
    return result


def export_ddsp_808_encoder_onnx(
    path: str | Path,
    *,
    encoder: DDSP808Encoder | None = None,
    n_mels: int = MEL_N_MELS,
    mel_time_frames: int = 64,
    opset: int = 17,
) -> Path:
    """
    Export mel encoder as ``ddsp_808_encoder.onnx`` for JUCE ONNX Runtime.

    Pass a trained ``encoder`` to export learned weights; otherwise uses random init.
    """
    out = Path(path)
    out.parent.mkdir(parents=True, exist_ok=True)
    model = encoder if encoder is not None else DDSP808Encoder(n_mels=n_mels)
    model.eval()
    dummy = torch.randn(1, 1, n_mels, mel_time_frames)
    torch.onnx.export(
        model,
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
