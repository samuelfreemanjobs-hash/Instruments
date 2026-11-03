# Phonk Drum Studio

Train a lightweight 1D convolutional Audio VAE on your Memphis phonk drum folder and generate new kicks, snares, and percs locally.

## Quick start

```bash
cd tools/phonk-drum-studio
python -m venv venv
```

Activate the virtual environment:

```bash
# On Windows:
venv\Scripts\activate

# On macOS/Linux:
source venv/bin/activate
```

Then install and run:

```bash
pip install -r requirements.txt
python app.py
```

1. **Browse Folder** — point at a directory of `.wav` / `.aif` one-shots.
2. **Train Model** — saves `phonk_vae.pth` next to your samples when finished.
3. **Generate** or **Mutate** — audition and **Export to WAV**.

## Standalone build

```bash
python build_standalone.py
```

Produces `dist/PhonkDrumStudio` (Linux), `.exe` (Windows), or `.app` (macOS).

## Tests

```bash
PYTHONPATH=. pytest tests -v
```

See [ARCHITECTURE.md](ARCHITECTURE.md) for module layout and data flow.
