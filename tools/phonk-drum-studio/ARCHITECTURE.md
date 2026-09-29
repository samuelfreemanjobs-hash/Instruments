# Phonk Drum Studio — Architecture

## Purpose

Phonk Drum Studio is a standalone desktop app for Memphis phonk producers. It trains a lightweight 1D convolutional Audio VAE on a folder of drum one-shots (`.wav` / `.aif`) and generates new variations via latent sampling and mutation.

## Build & run

```bash
cd tools/phonk-drum-studio
python3 -m venv .venv && source .venv/bin/activate
pip install -r requirements.txt
python app.py
```

Standalone bundle:

```bash
python build_standalone.py
```

## Data flow

1. **Dataset** (`dataset.py`) scans a user folder, loads each file with torchaudio, downmixes to mono, resamples to 44.1 kHz, crops/pads to 22,050 samples (0.5 s), and peak-normalizes.
2. **Training** (`trainer.py`) runs on a background thread: Adam + multi-scale spectral reconstruction loss + weighted KL. Checkpoints save as `phonk_vae.pth` in the dataset folder.
3. **Generation** (`app.py` + `model.py`): random latent `z ~ N(0,I)` or encode → jitter → decode; output is peak-normalized and held in memory for preview/export.
4. **Export** writes 44.1 kHz WAV via soundfile (16-bit PCM or 32-bit float).

## Threading / realtime

- CustomTkinter main thread handles UI only.
- Training, load/generate/mutate, playback, and export run in `threading.Thread` workers.
- Cooperative training stop via `threading.Event`.

## Key modules

| Module | Responsibility |
|--------|----------------|
| `model.py` | `MultiScaleSpectralLoss`, `AudioVAE`, KL helper |
| `dataset.py` | File scan, preprocessing, `DataLoader` factory |
| `trainer.py` | Device selection, `TrainingWorker`, checkpoint I/O |
| `app.py` | CustomTkinter GUI, audition (sounddevice), waveform canvas |
| `build_standalone.py` | PyInstaller one-file/windowed bundle |

## Extension points

- Adjust `TrainConfig.epochs`, batch size, or channel schedule in `AudioVAE`.
- Add data augmentation in `load_and_preprocess`.
- Swap loss weights in `trainer.py` (`KL_WEIGHT`).

## Related docs

- Repository index: `/ARCHITECTURE.md`
