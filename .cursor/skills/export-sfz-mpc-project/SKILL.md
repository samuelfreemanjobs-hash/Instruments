---
name: export-sfz-mpc-project
description: "Every factory project exports SFZ + instrument-map + MPC handoff bundle. Use at end of multisample render pipelines."
---

# Export SFZ + MPC project bundle

## Required exports (every instrument)

1. `samples/*.wav`  
2. `instrument-map.json` (`DISKLORDZ_INSTRUMENT_MAP`)  
3. `{instrumentId}.sfz` via `emit_sfz.py` or batch runner  
4. `project.json` + `HANDOFF_MPC.md` for **cursor-mpc-agent**  

## Commands

```bash
python3 disklordz/sound-factory/scripts/generate_multisample_instrument.py \
  --instrument-id DEMO_001 --lane jeezy --out /tmp/inst --emit-sfz

python3 disklordz/sound-factory/scripts/emit_sfz.py --map /tmp/inst/instrument-map.json --out /tmp/inst
```

Batch path auto-emits SFZ: `run_product_batch.py`.

## Pipeline JSON

```json
{ "step": "export.sfz", "required": true },
{ "step": "export.mpc_keygroup", "delegate": "cursor-mpc-agent", "required": true }
```

Skill `mpc-keygroup-program` runs after SFZ exists.
