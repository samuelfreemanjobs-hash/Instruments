---
name: logue-mkii-scaffold
description: Run scaffold-mkii.py for unit tree (/logue-mkii-scaffold).
---

# Scaffold (`/logue-mkii-scaffold [slug]`)

Requires `LOGUE_SDK` for `wasm.cc` generation when scaffolding from v1.

## Commands

```bash
# From v1 folder name under src/oscillators/
python3 tools/mkii/scaffold-mkii.py oscillators/<v1-name>

# Or with explicit mkII slug via build-mkii:
./tools/build-mkii.sh <slug> --scaffold-from oscillators/<v1-name>
```

Verify `src/mkii/oscillators/<slug>/` contains `Makefile`, `osc.h`, `unit.cc`, `PORTING.md`.

**Do not** overwrite `osc.h` files marked `PORT_COMPLETE`.

Update `status.json`: `current_phase`: `scaffold`.
