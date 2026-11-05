# Agent SOP — VST Factory Preset Matcher

## Mission

For each recipe slot, choose a **factory program index** on the target VST3 that fits the style and slot kind.

## Inputs

- `recipe.json`
- Path to `.vst3` bundle
- `VstiOneShotRender --list-programs` catalog

## Outputs

- `assignments.json` with `assignments[]`: `slot_id`, `program`, `matcher_rationale`
- Merged into `job.json` for the render binary

## Rules

- **May** judge musical fit (name heuristics + slot tags).
- **Must** assign every slot; render fails on gaps.
- **Must not** write final WAVs except by invoking the render CLI.
- Document rationale for humans; render executor ignores it.

## Implementation

Code: `tools/vsti-one-shot/vsti_one_shot/matcher.py`  
Invoked by: `boom.py`

## Future upgrades

- Short preview renders + listening pass for premium packs
- Optional user approval step before batch render
