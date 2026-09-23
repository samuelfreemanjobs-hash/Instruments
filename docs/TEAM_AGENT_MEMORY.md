# Team agent memory (Instruments / Disklordz)

Durable context for **Cloud Agents**, **Workflow Automation Agent**, and humans. Update this file when lanes, env vars, or schedules change.

**Structured mirror:** [.cursor/agent-memory/team-context.yaml](../.cursor/agent-memory/team-context.yaml)  
**Cloud agent store:** agents append notes to `/cursor/stores/self/` when a run learns something new (see below).

---

## Active lanes (2026)

| Lane | Branch example | Primary outputs |
|------|----------------|-----------------|
| **Track E — Gearmulator** | `cursor/gearmulator-lane-62b1` | JE8086 VST3, multisample maps, Docker docs |
| **mpc-agent** | submodule `mpc-agent/` | `capture_and_export.py`, MPC draft JSON, templates |
| **Osirus FX bus** | same | `build-gearmulator-osirus-fx.sh`, always-on FX env |
| **Track C — JUCE** | JD Upgraded, WAVE-909 | Clean-room ROM, trap wavetable |
| **Track D — HISE** | `hise-sketch/` | Rompler handoff |

---

## Environment (load order)

```bash
source gearmulator-lane/.gearmulator-vsti.env      # GEARMULATOR_JE8086_VST3
source gearmulator-lane/.gearmulator-osirus-fx.env # GEARMULATOR_OSIRUS_FX_VST3 + INSTRUMENTS_ALWAYS_OSIRUS_FX=1
pip install -r gearmulator-lane/requirements-dawdreamer.txt
```

Secrets (never commit): `GEARMULATOR_ROM_DIR`, Virus ROM paths, Supabase, Slack webhooks.

---

## Canonical multisample contract

Every session: **`instrument.map.json` v1** + `manifest.tsv` + WAVs (gitignored).  
SFZ and `exports/mpc_program.draft.json` are derived.

---

## Hardware capture rack (maintainer)

Full list and capture flows: **[HARDWARE_CAPTURE_RACK.md](HARDWARE_CAPTURE_RACK.md)**.

Summary: **MPC Live** (primary kit host), **Maschine+**, **MicroFreak**, **MicroKorg**, **Micro-X**, **Ultra Nova**, **Minilogue XD**, **SE-02**, **Volca FM2**, **Volca kick/filter**, **DrumBrute**, **DR-660**, **DR-5**, **SP-808**, **MV-1**, **Liven LoFi 12**, **MPC 500**. No Virus/JP/ASR/SU on rack—Gearmulator ROM lanes are optional add-ons.

Hardware → WAV locally; repo holds **maps + mpc-agent**, not samples.

---

## Product decisions (remember)

- **88emu** = GM/LA reference (~1987–2000), not trap brass source.
- **Trap timbres** = WAVE-909 / Virus synth + **Osirus FX bus** + sample maps → MPC/HISE.
- **JD Upgraded ROM** = procedural `JDUPGROM` only; no firmware PCM in repo.
- **mpc-agent submodule** may need manual `git push` from maintainer (bot 403).

---

## Workflow Automation Agent

Job catalog: [.cursor/workflow-automation/jobs.yaml](../.cursor/workflow-automation/jobs.yaml)  
Playbook: [WORKFLOW_AUTOMATION_AGENT.md](WORKFLOW_AUTOMATION_AGENT.md)

---

## How agents write memory

After a meaningful run, append a short YAML snippet to `/cursor/stores/self/team-memory-log.yaml`:

```yaml
- at: "2026-09-23T05:00:00Z"
  agent_run: "bc-..."
  learned: "Osirus FX build requires Virus ROM at runtime."
```

Then mirror one line into this doc’s **Changelog** if it affects the whole team.

### Changelog

- 2026-09-23: Osirus FX always-on lane, DawDreamer serial graph, workflow job manifest, team YAML.
- 2026-09-23: Maintainer hardware rack documented in HARDWARE_CAPTURE_RACK.md.
