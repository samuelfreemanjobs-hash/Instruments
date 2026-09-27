# Audio QA / Golden Lane Agent (WO-SAAS-027)

## Mission

Keep `factory-golden-lanes.json` + `factory:dsp-regression` aligned with lane quality.

## Loop

1. Run `npm run factory:dsp-regression`.
2. Adjust golden thresholds or synthesis **one lane at a time**.
3. PR with **Listen for** notes.

Works with WO-SAAS-018; avoid duplicate daily DSP refactors.
