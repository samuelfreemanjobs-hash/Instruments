import pytest

torch = pytest.importorskip("torch")
pytest.importorskip("torchaudio")

from drum_synth_blueprint.torch_ddsp.encoder import DDSP808Encoder, DDSP808TrainableSystem
from drum_synth_blueprint.torch_ddsp.pipeline import export_ddsp_808_encoder_onnx, train_ddsp_808_smoke
from drum_synth_blueprint.torch_ddsp.spectral_loss import SpectralDistanceLoss
from drum_synth_blueprint.torch_ddsp.synth_torch import Differentiable808Synth


def test_spectral_loss_identical_near_zero():
    x = torch.randn(1, 8192)
    crit = SpectralDistanceLoss()
    assert crit(x, x).item() < 1e-3


def test_spectral_loss_multi_scale_positive():
    a = torch.randn(1, 8192)
    b = torch.randn(1, 8192)
    crit = SpectralDistanceLoss()
    assert crit(a, b).item() > 0.0


def test_differentiable_synth_backprop():
    params = torch.rand(1, 4, requires_grad=True)
    synth = Differentiable808Synth(duration_sec=0.25)
    out = synth(params).sum()
    out.backward()
    assert params.grad is not None
    assert torch.isfinite(params.grad).all()


def test_mel_encoder_output_bounds():
    enc = DDSP808Encoder()
    mel = torch.rand(2, 1, 128, 32)
    out = enc(mel)
    assert out.shape == (2, 4)
    assert out.min() >= 0.0 and out.max() <= 1.0


def test_train_smoke_and_onnx_export(tmp_path):
    metrics = train_ddsp_808_smoke(steps=4, batch_size=2, duration_sec=0.25)
    assert metrics["final_spectral_loss"] >= 0.0
    onnx = export_ddsp_808_encoder_onnx(tmp_path / "ddsp_808_encoder.onnx", mel_time_frames=32)
    assert onnx.is_file() and onnx.stat().st_size > 500


def test_end_to_end_grad_flow():
    system = DDSP808TrainableSystem(duration_sec=0.25)
    target = torch.randn(2, int(44100 * 0.25))
    synth, _ = system(target)
    loss = SpectralDistanceLoss()(synth, target)
    loss.backward()
    for p in system.encoder.parameters():
        assert p.grad is not None
