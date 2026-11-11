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

## Preview matching (v0.2)

Default **`hybrid`** mode (via `boom.py --match-mode hybrid`):

1. Shortlist **8 candidate** factory programs per slot (name score + slot hash).
2. **`VstiOneShotRender --preview`** (~1.5 s) for each candidate.
3. Score WAV transients vs slot **kind** (`stab`, `key`, `bell`, `pad_hit`).
4. Blend name + audio scores; **penalize reuse** so 100 slots do not collapse to one preset.

Modes: `names` (fast), `preview` (audio only), `hybrid` (default).

## Future upgrades

- Optional user approval step before batch render
- LLM listen pass on preview folder for premium curation
