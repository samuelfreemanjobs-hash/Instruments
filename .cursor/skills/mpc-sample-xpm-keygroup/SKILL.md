---
name: mpc-sample-xpm-keygroup
description: Build and validate MPC keygroup .xpm programs (MPCVObject XML + co-located WAV) for Live, Force, Melodee, and KeyGroupBuilder. Use when exporting multisample instruments — not MPC Sample .xpj.
---

# MPC Sample / Live — `.xpm` keygroups

## Authority

- Spec: [JefroB Multisample-format-docs `xpm.md`](https://github.com/JefroB/Multisample-format-docs/blob/main/xpm.md)
- Reference generator: [MPC-KeyGroupBuilder](https://github.com/intelliriffer/MPC-KeyGroupBuilder) (`MPCVObject`)
- Monorepo writer: `disklordz/trap-lord-synth/Source/MpcXpmWriter.cpp` (**legacy `KeygroupProgram`** — migrate)

## Canonical XML shape

```text
MPCVObject → Program[@type="Keygroup"] → Instruments/Instrument → Layers/Layer
```

**Conventions (xpm.md):**

- `RootNote` = MIDI note **+ 1** (single-zone: prefer `LowNote` as true root)
- Pan **0.5** = center (0.0–1.0 linear)
- `SampleName` without extension; files **`NAME.WAV`** beside `.xpm`
- Prefer **slice loop** fields when `SliceLoop` is 1

## Host batch (KeyGroupBuilder)

```text
config.txt:  MULTIS /path/to/folder   # or SCW, SINGLES
run-mac-linux.sh
```

Filenames must encode pitch: `myinst C3.wav`, `myinst-60.wav`, etc.

## Monorepo export (Trap Lord)

```bash
python3 disklordz/trap-lord-synth/tests/test_mpc_xpm_export.py [folder]
```

Today’s writer targets **`KeygroupProgram`** — Melodee/Live may need **`MPCVObject`** migration.

## Melodee import

Melodee browser loads `.xpm` keygroups streamed from SD (see `melodee-sample` docs). Test on card after export.

## Do not

- Ship ~900 KB KeyGroupBuilder templates blindly — disk cost per program
- Confuse MPC 3 `.xty` (gzip JSON) with plain XML without reading Melodee’s format notes

## Verify

- `test_mpc_xpm_export.py` (schema smoke; not hardware)
- MPC Software or Melodee load test `[assumed]` until device proof