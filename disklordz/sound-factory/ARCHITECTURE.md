# Disklordz sound-factory

## Purpose

Offline **multisample instrument factory**: catalog-driven batches, clean-room trap synthesis, **SFZ** export, MPC Keygroup handoff, audio QA.

## Build & run

```bash
python3 disklordz/sound-factory/scripts/seed_jz400_catalog.py
python3 disklordz/sound-factory/scripts/run_product_batch.py \
  --catalog disklordz/sound-factory/catalog/jz400_catalog.json \
  --out disklordz/sound-factory/out --limit 5
python3 disklordz/sound-factory/tests/run_tests.sh
```

Outputs under `out/` (gitignored).

## Data flow

Catalog JSON → `run_product_batch.py` → WAV zones → `instrument-map.json` → `emit_sfz.py` → `project.json` / MPC handoff → QA (`factory_qa.py`).

## Key modules

| Path | Role |
|------|------|
| `scripts/trap_synth.py` | Rev2-inspired parametric multisample source |
| `scripts/DAWDreamer_renderer.py` | DawDreamer VST host entry (MPC-agent / multisample) |
| `scripts/renderers/dawdreamer_renderer.py` | `DawDreamerRenderer` implementation |
| `scripts/dawdreamer_render.py` | Zone batch via DawDreamer + `--vst-path` |
| `scripts/rev2trap_render.py` | Zone batch via `Rev2TrapOfflineRender` |
| `scripts/run_product_batch.py` | Chunked product renderer |
| `scripts/factory_qa.py` | QA gates |
| `scripts/emit_sfz.py` | SFZ writer |
| `catalog/` | JZ400 + REV2 catalogs |
| `schemas/instrument-map.schema.json` | Zone map contract |

## Threading / realtime

Offline batch only; no audio thread.

## Extension points

- **`--engine dawdreamer`** + `requirements-dawdreamer.txt` for third-party VSTi  
- Replace `trap_synth` with JUCE `OfflineRender` when Rev2Trap binary is enough  
- Submodule MPCTK for automated `.xpj`  
- YouTube ref tuning WO-SF-030  

## Related docs

- [docs/FACTORY_AGENTIC_WORKFLOW.md](../../docs/FACTORY_AGENTIC_WORKFLOW.md)  
- [docs/products/JZ400.md](../../docs/products/JZ400.md)  
- [docs/products/REV2_TRAP128.md](../../docs/products/REV2_TRAP128.md)
