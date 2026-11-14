---
name: mpc-sample-coder
description: Orchestrates Akai MPC Sample (AC50) and related format work — .xpj, MPCVObject .xpm, Rust mods, virtual lab, SD autorun. Use when building kits, keygroups, mods, or docs for MPC Sample, Melodee, or Live keygroup export from this monorepo.
---

# MPC Sample Coder (orchestrator)

Fleet agent: **`mpc-sample-coder`** · Profit skill: **`disklordz-mpc-sample-coder`**

## Read first

1. `docs/MPC_SAMPLE_ECOSYSTEM.md`
2. `docs/MPC_SAMPLE_CODER.md`
3. `disklordz/agents/profit/mpc-sample-coder/PLAYBOOK.md`
4. `disklordz/agents/profit/mpc-sample-coder/soul.md`

```bash
python3 scripts/agent_memory.py recall --agent mpc-sample-coder <keywords>
python3 disklordz/integrations/agents/mpc_sample_coder.py --route "<task>"
```

## Sub-skills (load the matching skill next)

| Skill | When |
|-------|------|
| `mpc-sample-xpj` | Host generate **`.xpj`** + `_[ProjectData]` (MPCTK) for stock Sample / Melodee import |
| `mpc-sample-xpm-keygroup` | **`.xpm`** keygroups (`MPCVObject`), KeyGroupBuilder, Trap Lord export alignment |
| `mpc-sample-rust-mod` | **`mpc-sample-sdk`** Rust `cdylib`, hooks, Extract, `build-mod.sh` |
| `mpc-sample-virtual-lab` | **`virtual-mpc-sample`** QEMU, `lab.py preload`, firmware BYO |
| `mpc-sample-sd-autorun` | **Melodee**, **SampQOL**, one `autorun/` policy, card layout |

## Decision table

| User asks for | Start with |
|---------------|------------|
| Chromatic / scale pads on MPC Sample | `mpc-sample-xpj` |
| Live / Force / Melodee instrument folder | `mpc-sample-xpm-keygroup` |
| In-app feature on stock 1.3 app | `mpc-sample-rust-mod` |
| Test mod without hardware | `mpc-sample-virtual-lab` |
| Full groovebox on SD | `mpc-sample-sd-autorun` (AGPL Melodee) |
| Trap Lord export broken on MPC | `mpc-sample-xpm-keygroup` + `disklordz/trap-lord-synth/` |

## Escalation (this monorepo)

| Work | Lane |
|------|------|
| JUCE DSP / `MpcXpmWriter` C++ | `vst-factory-director` / factory Cursor |
| 808 Lab / website kits | `plugin-factory-liaison` |
| Generic Python/docs queue | `autonomous-coding-agent` |

## Verify before done

```bash
python3 disklordz/integrations/agents/mpc_sample_coder.py --self-test
python3 scripts/agent_memory.py check
```

Hardware claims require device or virtual-lab proof — tag `[executed]` or `[assumed]`.

## Non-goals

- Committing Akai firmware, factory `/content`, or card dumps
- Vendoring AGPL **melodee-sample** / **sample-base** into proprietary JUCE plugins without compliance review
- Treating Sample `.xpj` and Live `MPCVObject` `.xpm` as interchangeable