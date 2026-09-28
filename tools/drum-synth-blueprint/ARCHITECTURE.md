# Drum synth blueprint (NumPy + DDSP reference)

## Purpose

Reference **vectorized** Trap kick and **phonk 808** curves plus **mel feature-loss** training targets for the **ddsp-ml-engineer** agent and future JUCE/ONNX ports.

## Build & run

```bash
cd tools/drum-synth-blueprint
pip install -r requirements.txt
pip install -e .
pytest tests -v
python -m drum_synth_blueprint.synth_808_generator
```

## Pure math core

| Module | Role |
|--------|------|
| `synth_808_generator.py` | Exp pitch **350→45 Hz**, integrated phase, **np.tanh** drive |
| `trap_kick_808.py` | Layered trap kick + glide 808 (legacy API) |
| `mel_loss.py` | Log-mel **feature difference loss** (phase-robust) |
| `encoder_stub.py` | Sample → `[pitch_decay, amp_decay, drive]` (numpy; optional torch MLP) |
| `torch_ddsp/` | **PyTorch** multi-scale `SpectralDistanceLoss`, `Differentiable808Synth`, train + **`ddsp_808_encoder.onnx`** export |

## PyTorch DDSP train + ONNX

```bash
cd tools/drum-synth-blueprint
pip install -r requirements.txt -r requirements-train.txt
pip install -e .
pytest tests/test_torch_ddsp.py -v
python scripts/train_ddsp_808.py --steps 60
python scripts/ddsp_training_loop.py --folder ./drums --epochs 20
# writes artifacts/ddsp_808_encoder.onnx (encoder only; synth stays in C++)
```

| Module | Role |
|--------|------|
| `torch_ddsp/spectral_loss.py` | torchaudio STFT **512 / 1024 / 2048**; normalized linear + log Frobenius |
| `torch_ddsp/synth_torch.py` | **4** bounded controls → exp pitch sweep + tanh (matches training blueprint) |
| `torch_ddsp/encoder.py` | Mel Conv2d `DDSP808Encoder`; ONNX **`mel_input` → `synth_parameters`** |
| `torch_ddsp/dataset.py` | ``DrumSampleDataset`` — flat ``.wav`` folder, resample/mono/pad/peak-norm |
| `torch_ddsp/mel_front_end.py` | Shared mel config (n_fft=2048, hop=1024, 128 mels) |
| `torch_ddsp/pipeline.py` | Smoke train, **folder train**, ONNX export |

## DDSP → ONNX → JUCE

See [`docs/juce_onnx_pipeline_guide.txt`](docs/juce_onnx_pipeline_guide.txt).

Agent prompts: [`docs/DDSP_TRAP_PHONK_AGENT_PROMPTS.md`](../../docs/DDSP_TRAP_PHONK_AGENT_PROMPTS.md).

## Data flow

```text
acoustic WAV → MelSpectrogram → DDSP808Encoder → 4× [0,1] params
            → Differentiable808Synth → SpectralDistanceLoss vs target
            → (export) ddsp_808_encoder.onnx + C++ synth in JUCE
```

## Related

- [tools/serum-forge/docs/SERUM_TEXT_TO_PRESET_PROMPTS.md](../serum-forge/docs/SERUM_TEXT_TO_PRESET_PROMPTS.md)
- [docs/JUCE_APC_AGENT_BOOTSTRAP.md](../../docs/JUCE_APC_AGENT_BOOTSTRAP.md)
