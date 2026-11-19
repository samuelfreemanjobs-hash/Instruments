# ZEN-Core Patch Forge — architecture

## Purpose

Dark-mode desktop UI for batch Roland ZENOLOGY `.svz` generation with genre profiles and a binary diff inspector.

## Build & run

```bash
pip install -e ../synth-forge && pip install -e .
zen-patch-forge
```

## Data flow

```text
Profile + mutation % → synth_forge.zen_forge_presets + ZenologySvzEngine
  → .svz or ZIP bank → user export folder
```

## Key modules

| Path | Role |
|------|------|
| `zen_patch_forge/main.py` | customtkinter UI (#121212 / #FF5500) |
| `../synth-forge/synth_forge/synths/zenology_svz.py` | Binary .svz engine |
| `../synth-forge/synth_forge/bin_diff.py` | Diff inspector |

## Related docs

- [tools/synth-forge/ARCHITECTURE.md](../synth-forge/ARCHITECTURE.md)
