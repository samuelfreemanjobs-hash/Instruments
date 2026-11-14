# ZEN-Core Patch Forge

Desktop companion for Roland ZENOLOGY `.svz` patch generation. Uses the shared engine in [`../synth-forge`](../synth-forge/).

```bash
cd tools/synth-forge && pip install -e .
cd ../zen-patch-forge
python3 -m venv .venv && source .venv/bin/activate
pip install -e .
zen-patch-forge
```

Place `seed_init.svz` from ZENOLOGY Pro at `../synth-forge/fixtures/zenology/seed_init.svz`.

**Experimental:** import generated patches in ZENOLOGY Pro (WRITE → Import) before using on hardware.
