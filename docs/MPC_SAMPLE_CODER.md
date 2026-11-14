# MPC Sample Coder — agent playbook

Fleet id: **`mpc-sample-coder`**  
Entry: `disklordz/agents/profit/mpc-sample-coder/agent.md`  
Copilot skill: `.github/skills/disklordz-mpc-sample-coder/SKILL.md`

## Cursor skills (lane SOPs)

| Skill | File |
|-------|------|
| Orchestrator | `.cursor/skills/mpc-sample-coder/SKILL.md` |
| `.xpj` / MPCTK | `.cursor/skills/mpc-sample-xpj/SKILL.md` |
| `.xpm` keygroups | `.cursor/skills/mpc-sample-xpm-keygroup/SKILL.md` |
| Rust mods | `.cursor/skills/mpc-sample-rust-mod/SKILL.md` |
| Virtual lab | `.cursor/skills/mpc-sample-virtual-lab/SKILL.md` |
| SD autorun | `.cursor/skills/mpc-sample-sd-autorun/SKILL.md` |

APC mirror: `.agents/skills/mpc-sample-coder/SKILL.md`

## Session bootstrap (paste once)

```text
Read AGENTS.md, docs/MPC_SAMPLE_ECOSYSTEM.md, docs/MPC_SAMPLE_CODER.md, and
disklordz/agents/profit/mpc-sample-coder/PLAYBOOK.md.

Agent: mpc-sample-coder. Task: <your task>

python3 scripts/agent_memory.py recall --agent mpc-sample-coder <keywords>
```

## Decision tree

```text
Need a playable project on MPC Sample?
  └─ Host: MPCTK generate (.xpj + ProjectData) OR Melodee project format
Need Live/Force/Melodee keygroup folder?
  └─ .xpm MPCVObject (xpm.md / KeyGroupBuilder) — NOT Trap Lord KeygroupProgram
Need hook inside stock 1.3 app?
  └─ mpc-sample-sdk Rust cdylib + virtual-mpc-sample lab.py preload
Need full replacement app on SD?
  └─ melodee-sample release zip (autorun) — check AGPL
Need docs / shortcuts only?
  └─ MPC-Sample-Secrets (no code)
```

## Verify before claiming done

```bash
python3 disklordz/integrations/agents/mpc_sample_coder.py --self-test
python3 scripts/agent_memory.py check
```

Hardware-dependent work: label **[assumed]** until device or `virtual-mpc-sample` run.

## Output format (fleet standard)

1. **Finding**
2. **Actions taken**
3. **Profit impact** (kits, presets, mod velocity, risk)
4. **Next automation**