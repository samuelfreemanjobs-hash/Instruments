# MPC Sample ecosystem map

Index for the **`mpc-sample-coder`** profit agent and operators. Covers **Akai MPC Sample (AC50)** firmware **1.3.0.12** unless noted.

## Product lines (do not mix)

| Line | Format | Primary repos / docs |
|------|--------|----------------------|
| **MPC Sample (AC50)** | `.xpj` + `_[ProjectData]/`, SD `autorun/` | [MPCTK](https://github.com/tarikcampos/MPC-Sample-Toolkit), [melodee-sample](https://github.com/keremimo/melodee-sample), [mpc-sample-sdk](https://github.com/ioma8/mpc-sample-sdk) |
| **MPC Live / One / X / Force** | `.xpm` keygroups (`MPCVObject`) | [Multisample-format-docs `xpm.md`](https://github.com/JefroB/Multisample-format-docs/blob/main/xpm.md), [MPC-KeyGroupBuilder](https://github.com/intelliriffer/MPC-KeyGroupBuilder) |
| **This monorepo (Trap Lord)** | `KeygroupProgram` `.xpm` (legacy writer) | `disklordz/trap-lord-synth/Source/MpcXpmWriter.cpp`, `tests/test_mpc_xpm_export.py` |

## Reference stack (read order for new work)

1. **Ops / community** — [MPC-Sample-Secrets](https://github.com/c1t1zen1/MPC-Sample-Secrets) (firmware transfer, USB-C, 1.3.0)
2. **Host projects** — [MPCTK](https://github.com/tarikcampos/MPC-Sample-Toolkit) (generate `.xpj` from WAV + template)
3. **Keygroup XML** — [xpm.md](https://github.com/JefroB/Multisample-format-docs/blob/main/xpm.md) + [KeyGroupBuilder](https://github.com/intelliriffer/MPC-KeyGroupBuilder) (`MPCVObject` templates)
4. **On-device mods**
   - **Rust hooks (stock app)** — [mpc-sample-sdk](https://github.com/ioma8/mpc-sample-sdk) + [virtual lab preload](https://github.com/orrpunk-art/virtual-mpc-sample)
   - **SD autorun groovebox** — [melodee-sample](https://github.com/keremimo/melodee-sample) / [sample-base](https://github.com/keremimo/sample-base) (AGPL)
   - **SD UX mod** — [SampQOL-OS](https://github.com/snarebes/SampQOL-OS) (closed binary; competes with Melodee for `autorun`)
5. **Emulator** — [virtual-mpc-sample](https://github.com/orrpunk-art/virtual-mpc-sample) (BYO firmware; `lab.py`)

## In-repo touchpoints

| Path | Role |
|------|------|
| `disklordz/trap-lord-synth/` | Offline keygroup export (`MpcXpmWriter`, worker UI) |
| `disklordz/trap-lord-synth/tests/test_mpc_xpm_export.py` | Validates export folder schema (not `MPCVObject`) |
| `songwriting-engine/mpc.py` | MPC500 export (different hardware) |
| `disklordz/integrations/agents/mpc_sample_coder.py` | Agent CLI self-test and routing |
| `.cursor/skills/mpc-sample-*/SKILL.md` | Lane SOPs (xpj, xpm, rust-mod, virtual-lab, sd-autorun) |

## Autorun exclusivity

Only **one** top-level `autorun/` on the microSD. Pick **Melodee**, **SampQOL**, or a custom **mpc-sample-sdk** / MPCTK package — not multiple without manual renaming.

## Safety

- Never commit Akai firmware, factory `/content`, or user card images.
- Root-SSH / autorun research is owner-device only; document, do not distribute firmware zips.
- **Melodee / sample-base** is AGPL — do not vendor into proprietary JUCE products without compliance review.

## Escalation (this monorepo)

| Work | Lane |
|------|------|
| Trap Lord / JUCE VST keygroup export | `vst-factory-director` / Cursor factory + align writer to `MPCVObject` |
| SaaS / kits | `plugin-factory-liaison`, 808 Lab, Antigravity prep |
| Generic queue Python/docs | `autonomous-coding-agent` |