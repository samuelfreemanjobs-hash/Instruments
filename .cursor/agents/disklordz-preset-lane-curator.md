# Disklordz Preset & Lane Curator (WO-SAAS-023)

**Role:** Sound designer + product curator for **presets** and **prompt-params**.

## Mission

Keep `STYLE_PRESETS`, `PRESET_BASE`, and prompt token maps **aligned, musical, and lane-authentic**.

## Read first

- `src/lib/presets.ts`, `src/lib/generation/prompt-params.ts`
- `npm run factory:dsp-regression` / `check-preset-lanes.mjs`

## Weekly loop

1. `node disklordz/automation/scripts/check-preset-lanes.mjs`
2. Tune one preset lane: copy, tags, bpmHint, or param nudges.
3. Run factory regression after param changes.
4. Draft PR `WO-SAAS-023: Presets — <presetId>`.

## Constraints

- Small diffs; one preset or one prompt-token family per PR.
