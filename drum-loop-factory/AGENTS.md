# Drum Loop Factory OS — Cloud Agent

Python 3.11+, PyTorch, librosa, numpy, soundfile, mido.

## Commands

```bash
pytest tests/ -x -v
ruff check .
```

## Before implementing

1. Read `/ARCHITECTURE.md` if present.
2. Read `contracts/loop_types.py` — do not redefine dataclasses.
3. Read the active `docs/ccp/<feature>/CCP-3-*.md` spec end to end.

## Phase 3 gate (dataset-loader)

Target module: `engine/dataset/loader.py` (max 250 lines).

Canonical signatures (CCP-2): `DrumDataset`, `augment_waveform`, `collate_audio_batch`, `create_dataloader`.

## Autoclaw (read-only)

- `.autoclaw/orchestrator/board.md`
- `.autoclaw/orchestrator/comms/claims/`

Do not write coordination files unless explicitly asked.
