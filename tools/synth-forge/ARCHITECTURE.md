# SynthForge — architecture

## Purpose

**SynthForge** is an autonomous hardware preset engineering workstation: batch preset generation, genetic mutation/morphing, SQLite memory and lineage, browser preview metrics, natural-language prompt-to-patch, WAV cloning, and librarian export staging. It lives in the monorepo under `tools/synth-forge/` for agents working alongside JD Upgraded offline tools.

## Build & run

```bash
cd tools/synth-forge
python3 -m venv .venv && source .venv/bin/activate
pip install -r requirements.txt
pip install -e .
pytest tests -v
uvicorn synth_forge.main:app --reload --port 8000
```

Open `http://127.0.0.1:8000/` for the static workstation UI.

Env names: see [`.env.example`](.env.example) (`SYNTH_FORGE_DB_PATH`, `SYNTH_FORGE_HOST`, `SYNTH_FORGE_PORT`).

## Data flow

```text
Prompt / batch request → semantic_engine / generator → safety.clamp_for_hardware
  → synth adapter pack (.prog / SysEx / stub .mnfk / .sve)
  → db (SQLite WAL) + optional audio_engine preview WAV + metrics
  → REST export / static UI librarian staging
```

Clone path: WAV upload → `cloner.analyze_wav` (FFT, ADSR, f0) → seed parameters → `generate_batch` variations.

## Threading / realtime

FastAPI serves HTTP; audio preview synthesis is offline NumPy (no realtime audio thread). Optional `sounddevice` capture is not required for CI.

## Key modules

| Path | Role |
|------|------|
| `synth_forge/models.py` | Pydantic presets, batch requests, timbre vectors |
| `synth_forge/safety.py` | Shared resonance/feedback/master/preview clamps |
| `synth_forge/synths/` | Hardware adapters + MiniFreak/Zenology stubs |
| `synth_forge/generator.py` | Mutation, morph, archetypes |
| `synth_forge/semantic_engine.py` | Prompt → category → parameters |
| `synth_forge/cloner.py` | WAV analysis → clone batches |
| `synth_forge/audio_engine.py` | Preview WAV + spectral metrics |
| `synth_forge/db.py` | `synth_memory.db` persistence |
| `synth_forge/main.py` | FastAPI routes + static mount |
| `synth_forge/static/` | Workstation UI |

## Hardware safety

All generation and adapter pack paths call `clamp_for_hardware` (resonance ≤ 0.85, delay feedback ≤ 0.75, master ≤ 0.90). Preview peaks capped at −12 dBFS in `audio_engine`.

## Extension points

- Add a synth: implement `SynthAdapter` in `synth_forge/synths/`, register in `synths/__init__.py`, add round-trip test in `tests/test_adapters.py`.
- New genre archetype: extend `ARCHETYPES` / `STYLE_PATTERNS` in generator + semantic_engine.
- Production capture: optional `sounddevice` extra in `pyproject.toml`.

## Fleet agent

| ID | Entry |
|----|--------|
| `hardware-preset-designer` | [disklordz/agents/profit/hardware-preset-designer/agent.md](../../disklordz/agents/profit/hardware-preset-designer/agent.md) |
| CLI | `python3 disklordz/integrations/agents/hardware_preset_designer.py` |
| Copilot skill | `.github/skills/disklordz-hardware-preset-designer/SKILL.md` |

## Related docs

- [tools/ARCHITECTURE.md](../ARCHITECTURE.md) — C++ offline tools index  
- Root [ARCHITECTURE.md](../../ARCHITECTURE.md) — monorepo index
