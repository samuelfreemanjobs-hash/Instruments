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

Then install and run the **desktop app**:

```bash
pip install -r requirements.txt
pip install -e .
python -m phonk_drum_studio
```

Or:

```bash
chmod +x scripts/run-phonk-drum-studio.sh
./scripts/run-phonk-drum-studio.sh
```

1. **Browse Folder** — point at a directory of `.wav` / `.aif` one-shots.
2. **Train Model** — saves `phonk_vae.pth` next to your samples when finished.
3. **Generate** or **Mutate** — audition and **Export to WAV**.
4. **Build Factory Kit** — package WAVs into a Disklordz kit (`manifest.json` + slot WAVs). Name files `kick.wav`, `snare.wav`, etc., or use `--fill-stubs` to pad missing slots.

## Standalone desktop build

```bash
pip install pyinstaller
python build_standalone.py
```

Produces `dist/PhonkDrumStudio` (Linux), `PhonkDrumStudio.exe` (Windows), or `PhonkDrumStudio.app` (macOS). The bundle includes factory-kit support (`kit_from_samples.py`) and optional stub WAV fillers.

Linux menu entry template: `packaging/PhonkDrumStudio.desktop`.

### Desktop runtime notes

- **Linux:** `python3-tk`, **PortAudio** (`libportaudio2`) for audition playback.
- **Frozen app** writes optional data under `~/PhonkDrumStudio/`.

## Tests

```bash
PYTHONPATH=. pytest tests -v
```

See [ARCHITECTURE.md](ARCHITECTURE.md) for module layout and data flow.
