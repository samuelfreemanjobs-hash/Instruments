# Cloud Agent VM — skills & subagents

This repo’s Cloud environment (**Disklordz / Instruments**) is tuned for **JUCE factory work**, **Gearmulator multisampling**, and **mpc-agent / DawDreamer** capture — not Docker-in-Docker.

Config: [.cursor/environment.json](../.cursor/environment.json) · install: [.cursor/scripts/cloud-install.sh](../.cursor/scripts/cloud-install.sh)

---

## Repository skills (read `SKILL.md` first)

| Skill | Path | Use when |
|-------|------|----------|
| **instruments-cloud-bootstrap** | [.cursor/skills/instruments-cloud-bootstrap/SKILL.md](../.cursor/skills/instruments-cloud-bootstrap/SKILL.md) | VM setup, missing deps, submodule init |
| **juce-factory-elite** | [.cursor/skills/juce-factory-elite/SKILL.md](../.cursor/skills/juce-factory-elite/SKILL.md) | `Source/`, Wave909, plugin CI |
| **gearmulator-multisample-capture** | [.cursor/skills/gearmulator-multisample-capture/SKILL.md](../.cursor/skills/gearmulator-multisample-capture/SKILL.md) | VST3 capture, Track E |
| **multisample-to-instrument** | [.cursor/skills/multisample-to-instrument/SKILL.md](../.cursor/skills/multisample-to-instrument/SKILL.md) | SFZ, rompler, MPC kits |

Cursor loads project skills from `.cursor/skills/` when present. [AGENTS.md](../AGENTS.md) indexes commands.

---

## Subagent routing (Task tool)

Use subagents for **parallelism or specialization**; the parent Cloud Agent still owns commits and PRs.

| Subagent | When to launch | Example |
|----------|----------------|---------|
| **explore** | Find files/APIs; Gearmulator upstream tree | “Where is JE-8086 ROM loader?” `thoroughness: medium` |
| **debug** | Reproducible render/DSP bug after evidence | Failed note grid, golden mismatch |
| **generalPurpose** | Multi-step research outside repo | MPC Software export format docs |
| **computerUse** | Manual VST UI preset save once | Gearmulator preset not reachable via program index |
| **ci-investigator** | Red **`cmake`** on PR | Summarize pluginval/golden failure |
| **videoReview** | Review UI walkthrough artifact | After `computerUse` demo |

Do **not** use `computerUse` for batch multisample renders — use DawDreamer scripts.

### JUCE-specific

| Subagent | When |
|----------|------|
| **bugbot** | User explicitly asks for Bugbot review |
| **security-review** | User explicitly asks for security review |

---

## Autonomous roles (prompt personas)

| Role ID | Doc | Branch prefix |
|---------|-----|----------------|
| `juce-factory-elite` | [JUCE_FACTORY_AGENT.md](JUCE_FACTORY_AGENT.md) | `cursor/juce-*-62b1` |
| Gearmulator capture | [GEARMULATOR_LANE.md](GEARMULATOR_LANE.md) | `cursor/gearmulator-*-62b1` |
| MPC multisample | [MPC_AGENT_DAWDREAMER.md](MPC_AGENT_DAWDREAMER.md) | work in **mpc-agent** submodule |

---

## Environment secrets (dashboard)

| Secret / path | Purpose |
|---------------|---------|
| `GEARMULATOR_ROM_DIR` | Read-only ROM/SysEx for LLE plugins (never git) |
| `PLUGINVAL_*` | Optional; see [tools/ARCHITECTURE.md](../tools/ARCHITECTURE.md) |

---

## MCP (optional)

| Server | Use |
|--------|-----|
| **cursor-subscriptions** | `subscribe_github_ci` after opening PR — auto-follow CI |
| **Slack** | Read CI channel; writes need user confirm |
| **Supabase / Stripe** | SaaS lane only — not plugin capture |

---

## Related

- [CURSOR_AGENT_PLAYBOOK.md](CURSOR_AGENT_PLAYBOOK.md)
- [DISKLORDZ_SAAS_AGENT_LANES.md](DISKLORDZ_SAAS_AGENT_LANES.md)
