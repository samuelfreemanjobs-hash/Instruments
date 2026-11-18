---
name: audio-analysis
description: Measures and interprets audio/WAV technical metrics — peaks, RMS, clip fraction, pitch screens, diversity hashes, optional ffmpeg LUFS/true-peak — without listening or releasing. Use when running verify_kit, pitch_check, diversity_audit, CALIPER review, reading qa-report.json, or when the user mentions audio-analysis, metering, or kit QA numbers.
---

# Audio Analysis

## Read first

| Tool / doc | Path |
|------------|------|
| Kit verifier | `claude/drumsmith/scripts/verify_kit.py` |
| Pitch screen | `claude/drumsmith/scripts/pitch_check.py` |
| Diversity | `claude/drumsmith/scripts/diversity_audit.py` |
| Package QA skill | `claude/drumsmith/.claude/skills/sample-delivery-qa/SKILL.md` |
| Release honesty | `claude/drumsmith/docs/RELEASE_GATE.md`, `claude/drumsmith/README.md` |
| CALIPER | `claude/drumsmith/.claude/agents/CALIPER.md` |

## Scope

**In**

- PCM/WAV inventory vs `manifest.json`
- Sample-peak, RMS, duration, channels, bit depth, clip counts
- Autocorrelation + target-centered spectral pitch **screens**
- Exact-duplicate / advisory envelope similarity
- Optional ffmpeg EBU R128 / true-peak when installed
- Interpreting JSON reports for humans (CALIPER-style: challenge claims)

**Out**

- Ear / commercial sign-off → Sam HITL  
- Writing new synthesis → `audio-dsp-engineering`  
- Product taxonomy → `product-design`  
- Reconstructing a competitor hit → `drum-sample-reverse-engineering`

## Hard rules

1. Scripts are source of truth — quote the JSON; do not invent meter values.
2. Sample-peak ≠ true-peak; clip fraction ≠ ITU certification.
3. Pitch `PASS` is a screen; `INCONCLUSIVE` needs human review before “tuned” claims.
4. Missing ffmpeg → mark “not measured”; never fabricate LUFS/TP.
5. Technical `PASS` ≠ release approval.
6. Do not claim the agent listened.

## Commands (DRUMSMITH)

```bash
cd claude/drumsmith
python3 scripts/verify_kit.py --kit PATH --manifest PATH/manifest.json --json PATH/qa-report.json
# optional: --ffmpeg
python3 scripts/diversity_audit.py --kit PATH --json PATH/diversity-report.json
# pitched 808s: pitch_check via verify / pitch_check.py on mono PCM16
```

## Report template

```markdown
# Audio analysis: <kit or file>

## Inventory
files vs manifest; format limits (e.g. mono/stereo PCM16)

## Metrics (from JSON)
| File | Peak | RMS | Clip frac | Duration | Notes |

## Pitch screens
| File | Status | Auto Hz | Spec Hz | Cents err |

## Diversity
exact dupes; advisory similar pairs

## Limits
what was not measured (ffmpeg, listening, true-peak…)

## Gate
technical PASS/FAIL only — release gate separate
```

## Checklist

- [ ] Ran verifier (or stated why not)
- [ ] Cited report paths
- [ ] Tagged approximate meters
- [ ] No listening / storefront claims
- [ ] Handed FAIL items to DSP or product skills as needed

## Related

`audio-dsp-engineering`, `drum-sample-reverse-engineering`, `product-design`, `deep-research`
