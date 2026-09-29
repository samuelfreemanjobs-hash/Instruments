import numpy as np

from drum_synth_blueprint.encoder_stub import infer_params_numpy, synthesize_from_wav
from drum_synth_blueprint.mel_loss import feature_difference_loss, loss_for_synth_params
from drum_synth_blueprint.synth_808_generator import (
    Synth808Params,
    exponential_pitch_hz,
    normalize_peak,
    synthesize_808,
)


def test_pitch_starts_high_ends_low():
    p = Synth808Params(pitch_start_hz=350.0, pitch_end_hz=45.0, pitch_decay_sec=0.05)
    t = np.array([0.0, 0.05, 0.5])
    f = exponential_pitch_hz(t, p)
    assert f[0] > 300
    assert f[-1] < 80
    assert f[0] > f[-1]


def test_tanh_saturation_bounds():
    wav = synthesize_808(Synth808Params(drive=2.0))
    assert float(np.max(np.abs(wav))) <= 1.01


def test_mel_loss_self_near_zero():
    ref = normalize_peak(synthesize_808())
    loss = feature_difference_loss(ref, ref.copy())
    assert loss < 1e-6


def test_encoder_pipeline_runs():
    target = normalize_peak(synthesize_808())
    params, syn = synthesize_from_wav(target)
    assert params.shape == (3,)
    assert len(syn) > 1000
    loss = feature_difference_loss(target, syn)
    assert loss < 1.0


def test_loss_for_synth_params_finite():
    target = normalize_peak(synthesize_808())
    loss = loss_for_synth_params(target, 0.055, 0.85, 1.35)
    assert np.isfinite(loss)
