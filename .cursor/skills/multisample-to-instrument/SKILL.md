---
name: multisample-to-instrument
description: Turn captured WAV grids into new presets/instruments (SFZ, HISE, mpc-agent, rompler SKU). Use after multisample capture or when user mentions sample maps, MPC kits, romplers from Gearmulator.
---

# Skill: Multisample → instrument

Read [docs/MULTISAMPLE_TO_INSTRUMENT_WORKFLOW.md](../../../docs/MULTISAMPLE_TO_INSTRUMENT_WORKFLOW.md) and [docs/MULTISAMPLE_MAP_FORMAT.md](../../../docs/MULTISAMPLE_MAP_FORMAT.md).

## Steps

1. **Capture** — skill **gearmulator-multisample-capture** (writes `instrument.map.json` automatically), or `python3 mpc-agent/scripts/capture_and_export.py` for SFZ + MPC draft in one step
2. **SFZ starter** (optional if not using capture_and_export)

```bash
python3 scripts/gearmulator/map_to_sfz.py \
  gearmulator-lane/multisamples/out/<session>/instrument.map.json
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
