---
name: multisample-to-instrument
description: Turn captured WAV grids into new presets/instruments (SFZ, HISE, mpc-agent, rompler SKU). Use after multisample capture or when user mentions sample maps, MPC kits, romplers from Gearmulator.
---

# Skill: Multisample → instrument

Read [docs/MULTISAMPLE_TO_INSTRUMENT_WORKFLOW.md](../../../docs/MULTISAMPLE_TO_INSTRUMENT_WORKFLOW.md).

## Steps

1. **Capture** — skill **gearmulator-multisample-capture**
2. **SFZ starter**

```bash
python3 scripts/gearmulator/manifest_to_sfz.py \
  gearmulator-lane/multisamples/out/<session>/manifest.tsv
```

3. **Author** — HISE (Track D / Antigravity handoff) or **mpc-agent** submodule for MPC Software
4. **Ship** — new `product_id` in Airtable; JUCE port WO if promoting to Track C

## mpc-agent

```bash
git submodule update --init mpc-agent
```

Implement MPC export in **mpc-agent** repo; call Instruments render scripts from there.

## Subagents

| Situation | Subagent |
|-----------|----------|
| HISE / Windows handoff | Do **not** use Cloud for HISE build — use [docs/HISE_ANTIGRAVITY_LANE.md](../../../docs/HISE_ANTIGRAVITY_LANE.md) bridge |
| End-user rompler UI in JUCE | **juce-factory-elite** skill + `generalPurpose` for design spikes |

## Legal

Captures gitignored; retail = user compliance review. See [docs/INSTALLER_POLICY.md](../../../docs/INSTALLER_POLICY.md).
