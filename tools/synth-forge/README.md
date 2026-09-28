# SynthForge

Python preset workstation (see [ARCHITECTURE.md](ARCHITECTURE.md)).

Quick start:

```bash
cd tools/synth-forge
python3 -m venv .venv && source .venv/bin/activate
pip install -r requirements.txt
pip install -e .
pytest tests -v
uvicorn synth_forge.main:app --reload --port 8000
```
