---
name: mpc-keygroup-program
description: "Build Akai MPC Sample .xpj projects via MPC-Sample-Toolkit (MPCTK). Use for chromatic/keygroup programs from factory WAVs — on local VS Code or Antigravity, not Cloud."
---

# MPC keygroup / chromatic programs (MPCTK)

## Primary tool

**[MPC-Sample-Toolkit (MPCTK)](https://github.com/samuelfreemanjobs-hash/MPC-Sample-Toolkit)** — not a separate `mpc-agent` repo.

- Reads/writes gzip `.xpj`, chromatic pad banks, scale layouts, WAV injection
- CLI: `mpctk` · GUI: `mpctk-gui` (macOS)
- Validated on physical MPC Sample hardware

**Runtime:** local Cursor (VS Code) or Antigravity. Cloud Agent prepares WAV + manifest; human or local agent runs MPCTK.

## Workflow

1. Ingest factory output: WAV paths from `DISKLORDZ_*_MANIFEST` or `InstrumentMapSpec`.
2. Choose structural XPJ template (MPCTK still requires template until that milestone ships).
3. Set source root / target root for transposition.
4. Run generation (CLI or GUI); output `.xpj` + `_[ProjectData]`.
5. Optional: hardware smoke test on MPC Sample.

## In-repo interchange

**InstrumentMapSpec / KeygroupSpec JSON** (future schema under `disklordz/sound-factory/schemas/`) lists zones before export so SFZ and MPC share one map.

## Install (local)

```bash
git clone https://github.com/samuelfreemanjobs-hash/MPC-Sample-Toolkit.git
cd MPC-Sample-Toolkit && python3 -m venv .venv && source .venv/bin/activate
python -m pip install -e .
mpctk --help
```

## Fallback

Bytebot / MPC Software UI only when MPCTK lacks a feature. Confirm before writing into user project folders.
