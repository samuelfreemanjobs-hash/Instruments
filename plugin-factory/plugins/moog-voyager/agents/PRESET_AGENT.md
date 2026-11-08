# Preset Agent — V Voyager (`v-voyager-preset-agent`)

**Backed by fleet agent:** `hardware-preset-designer` (Software preset lane for this SKU, not SynthForge SysEx).

## Mission

Own the **300-program Moog factory bank**: names, archetypes, **stack mode** (0–3), pink-noise hints, genre slots **025–032**, and regression after generator edits.

## Source of truth

| Artifact | Path |
|----------|------|
| Catalog | [`../PRESETS.md`](../PRESETS.md) |
| Generator | [`../scripts/generate_factory_presets.py`](../scripts/generate_factory_presets.py) |
| Compiled bank | [`../VoyagerPresetsGenerated.inc`](../VoyagerPresetsGenerated.inc) |
| Tests | [`../VoyagerPresetTests.cpp`](../VoyagerPresetTests.cpp) |

## Loop

1. Edit curated block or `_infer_stack` / `_infer_pink` in the generator.
2. Regenerate: `python3 plugin-factory/plugins/moog-voyager/scripts/generate_factory_presets.py`
3. QA: `python3 disklordz/integrations/agents/v_voyager_preset_agent.py --qa`
4. Optional demos: build `VoyagerBassDemo` / `VoyagerLeadDemo` for style presets.
5. Hand off to **`cursor-v-voyager`** if APVTS IDs or `Snapshot` shape changes.

## Acceptance

- `VoyagerPresetTests` → 300 programs
- Every preset: non-silent amp sustain, cutoff ≥ 20 Hz, valid stack 0–3
- Document new curated indices in `PRESETS.md`

## Work order

Airtable: **`WO-2026-VVOY-004`** · owner **`hardware-preset-designer`** · co-owner **`v-voyager-preset-agent`**
