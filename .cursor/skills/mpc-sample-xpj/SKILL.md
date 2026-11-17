---
name: mpc-sample-xpj
description: Generate and validate Akai MPC Sample .xpj projects (gzip JSON + _[ProjectData]) using MPCTK patterns. Use for stock MPC Sample or Melodee .xpj import — not Live .xpm keygroups.
---

# MPC Sample — `.xpj` (MPCTK lane)

## Authority

- Upstream: [MPC-Sample-Toolkit](https://github.com/tarikcampos/MPC-Sample-Toolkit) (MIT)
- Spec header: magic `ACVS`, `SerialisableProjectData`, JSON body
- Ecosystem: `docs/MPC_SAMPLE_ECOSYSTEM.md`

## Output layout

```text
MyProgram.xpj
MyProgram_[ProjectData]/
  sample.wav
```

## Host workflow (MPCTK)

```bash
pip install -e ".[dev]"   # in MPCTK clone
mpctk generate /path/to/sample.wav \
  --template /path/to/template.xpj \
  --source-root C --target-root D \
  --layout chromatic-keyboard \
  --pads 16 --bank B --start-pad 1 \
  --name "My Program" --output /path/to/out
```

**Limitation [inspected]:** generation still needs a structural **template `.xpj`** until MPCTK template-independence ships.

## Tuning limits (hardware-verified in MPCTK)

- Coarse tune: −24 … +24 semitones
- Fine tune: −90 … +90 cents

## In-repo touchpoints

- Validate routing: `mpc_sample_coder.py --route "xpj generate"`
- Melodee can open stock `.xpj` read-only with `_[ProjectData]` (see Melodee `MELODEE.md`)

## Do not

- Emit `KeygroupProgram` or `MPCVObject` XML — that is **`mpc-sample-xpm-keygroup`**
- Assume MPCTK runs in Cloud CI without vendoring or cloning the toolkit

## Verify

- MPCTK pytest in toolkit repo
- On device or Melodee: load project after SD/USB transfer