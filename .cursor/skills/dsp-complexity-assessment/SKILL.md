---
name: dsp-complexity-assessment
description: Scores plugin/DSP complexity (L0-L3) using architecture spec §8. Use before large implementations, refactors, or when user asks how hard a feature will be.
---

# DSP Complexity Assessment subagent

## Read first

`docs/dsp-architecture-specification.md` — **§8 Complexity assessment**

## Procedure

1. List planned features: voices, oscs, filters, mod, FX, surfaces, targets.
2. Score each **dimension** 0–3 per spec §8.1 table.
3. Sum → **total score** (max 21).
4. Assign **tier** L0–L3 per §8.2.
5. Scan **risk flags** §8.3; list any that apply.

## Output template (copy to user/PR)

```markdown
## Complexity assessment

| Dimension | Score | Notes |
|-----------|-------|-------|
| Voices | | |
| Oscillators/voice | | |
| Filters | | |
| Modulation | | |
| FX | | |
| Surfaces | | |
| Targets | | |
| **Total** | | |

**Tier:** L_
**Risk flags:** none | list

**Recommended architecture:**
- (e.g. fixed 8-voice pool, params.json, shared/dsp/biquad.h)
```

## Recommendations by tier

| Tier | Required artifacts |
|------|-------------------|
| L0 | Inline Processor only |
| L1 | `Source/Dsp/`, `params.json`, voice pool |
| L2 | Mod matrix, tests for `shared/dsp/` |
| L3 | Oversampling plan, perf budget doc |

## When to re-assess

- Adding second oscillator, FX slot, or hardware target
- Polyphony increase
- Porting to mkII + VST simultaneously

## Do not

- Write implementation code unless user asks to proceed after assessment
- Under-score to skip `params.json` on L1+ work
