# Cursor MPC-agent (import landing zone)

This directory is reserved for your **MPC-agent** specialist (rules, skills, batch scripts, MPCTK glue).

**Source of truth (pending import):**  
[Origin handoff](https://origin.cursor.com/git/samuelfreemanjobs/tmp-1e342cd8506744c8.git) — see [docs/MPC_AGENT_ORIGIN_IMPORT.md](../../docs/MPC_AGENT_ORIGIN_IMPORT.md).

Until the handoff is pushed to GitHub, only the orchestrator skill [`.cursor/skills/cursor-mpc-agent`](../../.cursor/skills/cursor-mpc-agent/SKILL.md) and sound-factory handoff files (`HANDOFF_MPC.md`) apply.

## After import

- Wire MPCTK: [MPC-Sample-Toolkit](https://github.com/samuelfreemanjobs-hash/MPC-Sample-Toolkit)
- Input: `disklordz/sound-factory/out/<jobId>/` (manifest, instrument-map, samples)
- Output: `.xpj`, `_[ProjectData]/`, `keygroup-report.json`
