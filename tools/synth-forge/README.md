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

## Librarian seed fixtures (Minilogue XD + ZENOLOGY)

Template-based patch forging needs **your** hardware exports (not committed to git):

| Synth | Drop file at |
|-------|----------------|
| Minilogue XD | `fixtures/minilogue_xd/seed_init.mnlgxdprog` |
| ZENOLOGY Pro | `fixtures/zenology/seed_init.svz` |

1. Initialize a patch on the synth, export via official librarian / WRITE → Export.
2. Place the file at the path above (`fixtures/` is gitignored).
3. Use the **Binary diff** API (`POST /api/bin-diff`) with two exports where you moved **one** knob to discover new offsets; merge into `synth_forge/data/*/offsets.json`.

Batch exports:

- `POST /api/librarian/xd/batch` — ZIP of `.mnlgxdprog` (categories: `xd_bass`, `xd_pad`, `xd_pluck`, `xd_lead`, `xd_scifi_fx`).
- `POST /api/librarian/zen/batch` — `.svz` or ZIP bank (`bank: true` for multi-patch).

Desktop ZEN-Core UI: [`../zen-patch-forge/`](../zen-patch-forge/).
